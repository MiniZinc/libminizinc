/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/eval_par.hh>
#include <minizinc/flat_exp.hh>

namespace MiniZinc {

EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b,
              bool doNotFollowChains) {
  CallStackItem _csi(env, e);
  EE ret;
  Id* id = Expression::cast<Id>(e);
  if (id->decl() == nullptr) {
    if (id->type().isAnn()) {
      ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
      ret.r = bind(env, ctx, r, e);
      return ret;
    }
    throw FlatteningError(env, Expression::loc(e), "undefined identifier");
  }
  if (!doNotFollowChains) {
    Expression* id_f = follow_id_to_decl(id);
    if (id_f == env.constants.absent) {
      ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
      ret.r = bind(env, ctx, r, id_f);
    } else {
      id = Expression::cast<VarDecl>(id_f)->id();
    }
  }
  if (ctx.neg && id->type().dim() > 0) {
    if (id->type().dim() > 1) {
      throw InternalError("multi-dim arrays in negative positions not supported yet");
    }
    KeepAlive ka;
    {
      GCLock lock;
      std::vector<VarDecl*> gen_id(1);
      gen_id[0] =
          new VarDecl(Expression::loc(id), new TypeInst(Expression::loc(id), Type::parint()),
                      env.genId(), IntLit::a(0));

      /// TODO: support arbitrary dimensions
      std::vector<Expression*> idxsetargs(1);
      idxsetargs[0] = id;
      Call* idxset = Call::a(Expression::loc(id).introduce(), "index_set", idxsetargs);
      idxset->decl(env.model->matchFn(env, idxset, false));
      idxset->type(idxset->decl()->rtype(env, idxsetargs, nullptr, false));
      Generator gen(gen_id, idxset, nullptr);
      std::vector<Expression*> idx(1);
      Generators gens;
      gens.g.push_back(gen);
      UnOp* aanot = new UnOp(Expression::loc(id), UOT_NOT, nullptr);
      auto* cp = new Comprehension(Expression::loc(id), aanot, gens, false);
      Id* bodyidx = cp->decl(0, 0)->id();
      idx[0] = bodyidx;
      auto* aa = new ArrayAccess(Expression::loc(id), id, idx);
      aanot->e(aa);
      Type tt = id->type().elemType(env);
      aa->type(tt);
      aanot->type(aa->type());
      cp->type(id->type());
      ka = cp;
    }
    Ctx nctx = ctx;
    nctx.neg = false;
    ret = flat_exp(env, nctx, ka(), r, b);
  } else {
    GCLock lock;
    VarDecl* vd = id->decl()->flat();
    Expression* rete = nullptr;
    if (vd == nullptr) {
      if (id->decl()->e() == nullptr || Expression::type(id->decl()->e()).isAnn() ||
          Expression::type(id->decl()->e()).isvar() || Expression::type(id->decl()->e()).cv() ||
          Expression::type(id->decl()->e()).dim() > 0) {
        // New top-level id, need to copy into env.m
        Ctx nctx;
        nctx.i = ctx.i;
        auto* flat_ident = Expression::cast<Id>(
            flat_exp(env, nctx, id->decl(), nullptr, env.constants.varTrue).r());
        if (flat_ident->decl() == nullptr && id->type().isAnn()) {
          ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
          ret.r = bind(env, ctx, r, flat_ident);
          return ret;
        }
        vd = flat_ident->decl();
      } else {
        vd = id->decl();
      }
    }
    ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
    if (vd->e() != nullptr) {
      if (Expression::type(vd->e()).isPar() && Expression::type(vd->e()).dim() == 0) {
        rete = eval_par(env, vd->e());
        if (vd->toplevel() && (vd->ti()->domain() != nullptr) && !vd->ti()->computedDomain()) {
          check_index_sets(env, vd, rete);
          check_par_domain(env, vd, rete);
          if (vd->type() == Type::varbool()) {
            vd->ti()->domain(rete);
          } else if (vd->type() == Type::varint()) {
            IntVal v = eval_int(env, rete);
            vd->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
          } else if (vd->type() == Type::varfloat()) {
            FloatVal v = eval_float(env, rete);
            vd->ti()->domain(new SetLit(Location().introduce(), FloatSetVal::a(v, v)));
          } else if (vd->type() == Type::varsetint()) {
            IntSetVal* v = eval_intset(env, rete);
            vd->ti()->domain(new SetLit(Location().introduce(), v));
          }
          vd->ti()->setComputedDomain(true);
        }
      } else if (Expression::isa<Id>(vd->e())) {
        rete = vd->e();
      }
    } else if (vd->ti()->ranges().empty() && (vd->ti()->domain() != nullptr) &&
               vd->type().st() == Type::ST_PLAIN && vd->type().ot() == Type::OT_PRESENT) {
      if (vd->type().bt() == Type::BT_BOOL) {
        rete = vd->ti()->domain();
      } else if (Expression::type(vd).bt() == Type::BT_INT &&
                 Expression::isa<SetLit>(vd->ti()->domain()) &&
                 (Expression::cast<SetLit>(vd->ti()->domain())->isv() != nullptr) &&
                 Expression::cast<SetLit>(vd->ti()->domain())->isv()->card() == 1) {
        rete = IntLit::a(Expression::cast<SetLit>(vd->ti()->domain())->isv()->min());
      } else if (vd->ti()->type().structBT()) {
        auto* fieldsti = Expression::cast<ArrayLit>(vd->ti()->domain());
        std::vector<Expression*> elems(fieldsti->size());
        for (unsigned int i = 0; i < fieldsti->size(); ++i) {
          CallStackItem csi(env, IntLit::a(static_cast<long long int>(i)));
          auto* nti = Expression::cast<TypeInst>((*fieldsti)[i]);
          Type nty(nti->type());
          auto* vti = new TypeInst(Location().introduce(), nty, nti->ranges(), nti->domain());
          VarDecl* nvd = new_vardecl(env, Ctx(), vti, nullptr, vd, nullptr);
          elems[i] =
              flatten_id(env, ctx, nvd->id(), nullptr, env.constants.varTrue, doNotFollowChains)
                  .r();
        }
        // After introducing variables for each tuple element, the original domain can be
        // set to "computed" (since it is a consequence of the individual variable domains)
        vd->ti()->setComputedDomain(true);

        ArrayLit* al = ArrayLit::constructTuple(Location().introduce(), elems);
        al->type(vd->type());

        // Set tuple instantiation as RHS
        vd->e(al);

        // count flattened version and add to CSE
        env.voAddExp(vd);
        EE ee;
        ee.r = vd;
        env.cseMapInsert(vd->e(), ee);
      }
    } else if (!vd->ti()->ranges().empty()) {
      // create fresh variables and array literal
      std::vector<std::pair<int, int> > dims;
      IntVal asize = 1;
      for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
        TypeInst* ti = vd->ti()->ranges()[i];
        if (ti->domain() == nullptr) {
          throw FlatteningError(env, Expression::loc(ti), "array dimensions unknown");
        }
        IntSetVal* isv = eval_intset(env, ti->domain());
        if (isv->empty()) {
          dims.emplace_back(1, 0);
          asize = 0;
        } else {
          if (isv->size() != 1) {
            throw FlatteningError(env, Expression::loc(ti), "invalid array index set");
          }
          asize *= (isv->max(0) - isv->min(0) + 1);
          dims.emplace_back(static_cast<int>(isv->min(0).toInt()),
                            static_cast<int>(isv->max(0).toInt()));
        }
      }
      Type tt = vd->ti()->type().elemType(env);

      if (asize > Constants::max_array_size) {
        std::ostringstream oss;
        oss << "array size (" << asize << ") exceeds maximum allowed size ("
            << Constants::max_array_size << ")";
        throw FlatteningError(env, Expression::loc(vd), oss.str());
      }

      std::vector<Expression*> elems(static_cast<int>(asize.toInt()));
      for (int i = 0; i < static_cast<int>(asize.toInt()); i++) {
        CallStackItem csi(env, IntLit::a(i));
        auto* vti = new TypeInst(Location().introduce(), tt, vd->ti()->domain());
        VarDecl* nvd = new_vardecl(env, Ctx(), vti, nullptr, vd, nullptr);
        elems[i] = nvd->id();
        if (tt.structBT()) {
          elems[i] =
              flatten_id(env, ctx, nvd->id(), nullptr, env.constants.varTrue, doNotFollowChains)
                  .r();
        }
      }
      // After introducing variables for each array element, the original domain can be
      // set to "computed" (since it is a consequence of the individual variable domains)
      vd->ti()->setComputedDomain(true);

      auto* al = new ArrayLit(Location().introduce(), elems, dims);
      al->type(elems.empty() ? Type::bot(vd->type().dim()) : vd->type());
      vd->e(al);
      env.voAddExp(vd);
      EE ee;
      ee.r = vd;
      env.cseMapInsert(vd->e(), ee);
    }
    if (rete == nullptr) {
      if (!vd->toplevel()) {
        // create new VarDecl in toplevel, if decl doesnt exist yet
        auto it = env.cseMapFind(vd->e());
        if (it == env.cseMapEnd()) {
          Expression* vde = follow_id(vd->e());
          auto* vdea = Expression::dynamicCast<ArrayLit>(vde);
          if ((vdea != nullptr) && vdea->empty()) {
            // Do not create names for empty arrays but return array literal directly
            rete = vdea;
          } else {
            VarDecl* nvd = new_vardecl(env, ctx, eval_typeinst(env, ctx, vd), nullptr, vd, nullptr);

            if (vd->e() != nullptr) {
              (void)flat_exp(env, Ctx(), vd->e(), nvd, env.constants.varTrue);
            }
            vd = nvd;
            EE ee(vd, nullptr);
            if (vd->e() != nullptr) {
              env.cseMapInsert(vd->e(), ee);
            }
          }
        } else {
          if (Expression::isa<VarDecl>(it->second.r)) {
            vd = Expression::cast<VarDecl>(it->second.r);
          } else {
            rete = it->second.r;
          }
        }
      }
      if (rete == nullptr) {
        if (id->type().bt() == Type::BT_ANN && (vd->e() != nullptr)) {
          rete = vd->e();
        } else {
          auto* vda = Expression::dynamicCast<ArrayLit>(vd);
          if ((vda != nullptr) && vda->empty()) {
            // Do not create names for empty arrays but return array literal directly
            rete = vda;
          } else {
            rete = vd->id();
          }
        }
      }
    }
    // Add reverse mapper for tuple var decls
    // TODO: This only has to happen on first flatten_id call.
    if ((vd->type().istuple() || vd->type().isrecord()) && vd->e() != nullptr) {
      Expression* lit = follow_id(vd->e());
      assert(Expression::isa<ArrayLit>(lit));
      env.reverseMappers.insert(vd->id(), lit);
    }
    ret.r = bind(env, ctx, r, rete);
  }
  return ret;
}

EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  return flatten_id(env, ctx, e, r, b, false);
}

}  // namespace MiniZinc
