/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flat_exp.hh>

namespace MiniZinc {

EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b,
              bool doNotFollowChains) {
  CallStackItem _csi(env, e);
  EE ret;
  Id* id = e->cast<Id>();
  if (id->decl() == nullptr) {
    if (id->type().isAnn()) {
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      ret.r = bind(env, ctx, r, e);
      return ret;
    }
    throw FlatteningError(env, e->loc(), "undefined identifier");
  }
  if (!doNotFollowChains) {
    Expression* id_f = follow_id_to_decl(id);
    if (id_f == constants().absent) {
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      ret.r = bind(env, ctx, r, id_f);
    } else {
      id = id_f->cast<VarDecl>()->id();
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
      gen_id[0] = new VarDecl(id->loc(), new TypeInst(id->loc(), Type::parint()), env.genId(),
                              IntLit::a(0));

      /// TODO: support arbitrary dimensions
      std::vector<Expression*> idxsetargs(1);
      idxsetargs[0] = id;
      Call* idxset = new Call(id->loc().introduce(), "index_set", idxsetargs);
      idxset->decl(env.model->matchFn(env, idxset, false));
      idxset->type(idxset->decl()->rtype(env, idxsetargs, false));
      Generator gen(gen_id, idxset, nullptr);
      std::vector<Expression*> idx(1);
      Generators gens;
      gens.g.push_back(gen);
      UnOp* aanot = new UnOp(id->loc(), UOT_NOT, nullptr);
      auto* cp = new Comprehension(id->loc(), aanot, gens, false);
      Id* bodyidx = cp->decl(0, 0)->id();
      idx[0] = bodyidx;
      auto* aa = new ArrayAccess(id->loc(), id, idx);
      aanot->e(aa);
      Type tt = id->type();
      tt.dim(0);
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
      if (id->decl()->e() == nullptr || id->decl()->e()->type().isAnn() ||
          id->decl()->e()->type().isvar() || id->decl()->e()->type().cv() ||
          id->decl()->e()->type().dim() > 0) {
        // New top-level id, need to copy into env.m
        vd = flat_exp(env, Ctx(), id->decl(), nullptr, constants().varTrue).r()->cast<Id>()->decl();
      } else {
        vd = id->decl();
      }
    }
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
    if (vd->e() != nullptr) {
      if (vd->e()->type().isPar() && vd->e()->type().dim() == 0) {
        rete = eval_par(env, vd->e());
        if (vd->toplevel() && (vd->ti()->domain() != nullptr) && !vd->ti()->computedDomain()) {
          // need to check if domain includes RHS value
          if (vd->type() == Type::varbool()) {
            if (!Expression::equal(rete, vd->ti()->domain())) {
              env.fail();
            }
            vd->ti()->domain(rete);
          } else if (vd->type() == Type::varint()) {
            IntSetVal* isv = eval_intset(env, vd->ti()->domain());
            IntVal v = eval_int(env, rete);
            if (!isv->contains(v)) {
              env.fail();
            }
            vd->ti()->domain(new SetLit(Location().introduce(), IntSetVal::a(v, v)));
          } else if (vd->type() == Type::varfloat()) {
            FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
            FloatVal v = eval_float(env, rete);
            if (!fsv->contains(v)) {
              env.fail();
            }
            vd->ti()->domain(new SetLit(Location().introduce(), FloatSetVal::a(v, v)));
          } else if (vd->type() == Type::varsetint()) {
            IntSetVal* isv = eval_intset(env, vd->ti()->domain());
            IntSetVal* v = eval_intset(env, rete);
            IntSetRanges isv_r(isv);
            IntSetRanges v_r(v);
            if (!Ranges::subset(v_r, isv_r)) {
              env.fail();
            }
            vd->ti()->domain(new SetLit(Location().introduce(), v));
          }
          // If we made it to here, the new domain is equal to the RHS
          vd->ti()->setComputedDomain(true);
        }
      } else if (vd->e()->isa<Id>()) {
        rete = vd->e();
      }
    } else if (vd->ti()->ranges().size() == 0 && (vd->ti()->domain() != nullptr) &&
               vd->type().st() == Type::ST_PLAIN && vd->type().ot() == Type::OT_PRESENT) {
      if (vd->type().bt() == Type::BT_BOOL) {
        rete = vd->ti()->domain();
      } else if (vd->type().bt() == Type::BT_INT && vd->ti()->domain()->isa<SetLit>() &&
                 (vd->ti()->domain()->cast<SetLit>()->isv() != nullptr) &&
                 vd->ti()->domain()->cast<SetLit>()->isv()->card() == 1) {
        rete = IntLit::a(vd->ti()->domain()->cast<SetLit>()->isv()->min());
      }
    } else if (vd->ti()->ranges().size() > 0) {
      // create fresh variables and array literal
      std::vector<std::pair<int, int> > dims;
      IntVal asize = 1;
      for (unsigned int i = 0; i < vd->ti()->ranges().size(); i++) {
        TypeInst* ti = vd->ti()->ranges()[i];
        if (ti->domain() == nullptr) {
          throw FlatteningError(env, ti->loc(), "array dimensions unknown");
        }
        IntSetVal* isv = eval_intset(env, ti->domain());
        if (isv->size() == 0) {
          dims.emplace_back(1, 0);
          asize = 0;
        } else {
          if (isv->size() != 1) {
            throw FlatteningError(env, ti->loc(), "invalid array index set");
          }
          asize *= (isv->max(0) - isv->min(0) + 1);
          dims.emplace_back(static_cast<int>(isv->min(0).toInt()),
                            static_cast<int>(isv->max(0).toInt()));
        }
      }
      Type tt = vd->ti()->type();
      tt.dim(0);

      if (asize > Constants::max_array_size) {
        std::ostringstream oss;
        oss << "array size (" << asize << ") exceeds maximum allowed size ("
            << Constants::max_array_size << ")";
        throw FlatteningError(env, vd->loc(), oss.str());
      }

      std::vector<Expression*> elems(static_cast<int>(asize.toInt()));
      for (int i = 0; i < static_cast<int>(asize.toInt()); i++) {
        CallStackItem csi(env, IntLit::a(i));
        auto* vti = new TypeInst(Location().introduce(), tt, vd->ti()->domain());
        VarDecl* nvd = new_vardecl(env, Ctx(), vti, nullptr, vd, nullptr);
        elems[i] = nvd->id();
      }
      // After introducing variables for each array element, the original domain can be
      // set to "computed" (since it is a consequence of the individual variable domains)
      vd->ti()->setComputedDomain(true);

      auto* al = new ArrayLit(Location().introduce(), elems, dims);
      al->type(vd->type());
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
          ArrayLit* vdea = vde != nullptr ? vde->dynamicCast<ArrayLit>() : nullptr;
          if ((vdea != nullptr) && vdea->size() == 0) {
            // Do not create names for empty arrays but return array literal directly
            rete = vdea;
          } else {
            VarDecl* nvd = new_vardecl(env, ctx, eval_typeinst(env, ctx, vd), nullptr, vd, nullptr);

            if (vd->e() != nullptr) {
              (void)flat_exp(env, Ctx(), vd->e(), nvd, constants().varTrue);
            }
            vd = nvd;
            EE ee(vd, nullptr);
            if (vd->e() != nullptr) {
              env.cseMapInsert(vd->e(), ee);
            }
          }
        } else {
          if (it->second.r()->isa<VarDecl>()) {
            vd = it->second.r()->cast<VarDecl>();
          } else {
            rete = it->second.r();
          }
        }
      }
      if (rete == nullptr) {
        if (id->type().bt() == Type::BT_ANN && (vd->e() != nullptr)) {
          rete = vd->e();
        } else {
          auto* vda = vd->dynamicCast<ArrayLit>();
          if ((vda != nullptr) && vda->size() == 0) {
            // Do not create names for empty arrays but return array literal directly
            rete = vda;
          } else {
            rete = vd->id();
          }
        }
      }
    }
    ret.r = bind(env, ctx, r, rete);
  }
  return ret;
}

EE flatten_id(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  return flatten_id(env, ctx, e, r, b, false);
}

}  // namespace MiniZinc
