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

EE flatten_comp(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  auto* c = e->cast<Comprehension>();
  KeepAlive c_ka(c);

  bool isvarset = false;
  if (c->set()) {
    for (int i = 0; i < c->numberOfGenerators(); i++) {
      Expression* g_in = c->in(i);
      if (g_in != nullptr) {
        const Type& ty_in = g_in->type();
        if (ty_in == Type::varsetint()) {
          isvarset = true;
          break;
        }
        if (c->where(i) != nullptr) {
          if (c->where(i)->type() == Type::varbool()) {
            isvarset = true;
            break;
          }
        }
      }
    }
  }

  if (c->type().isOpt() || isvarset) {
    std::vector<Expression*> in(c->numberOfGenerators());
    std::vector<Expression*> orig_where(c->numberOfGenerators());
    std::vector<Expression*> where;
    GCLock lock;
    for (int i = 0; i < c->numberOfGenerators(); i++) {
      if (c->in(i) == nullptr) {
        in[i] = nullptr;
        orig_where[i] = c->where(i);
      } else {
        if (c->in(i)->type().isvar() && c->in(i)->type().dim() == 0) {
          std::vector<Expression*> args(1);
          args[0] = c->in(i);
          Call* ub = new Call(Location().introduce(), "ub", args);
          ub->type(Type::parsetint());
          ub->decl(env.model->matchFn(env, ub, false));
          in[i] = ub;
          for (int j = 0; j < c->numberOfDecls(i); j++) {
            auto* bo = new BinOp(Location().introduce(), c->decl(i, j)->id(), BOT_IN, c->in(i));
            bo->type(Type::varbool());
            where.push_back(bo);
          }
        } else {
          in[i] = c->in(i);
        }
        if ((c->where(i) != nullptr) && c->where(i)->type().isvar()) {
          // This is a generalised where clause. Split into par and var part.
          // The par parts can remain in where clause. The var parts are translated
          // into optionality constraints.
          if (c->where(i)->isa<BinOp>() && c->where(i)->cast<BinOp>()->op() == BOT_AND) {
            std::vector<Expression*> parWhere;
            std::vector<BinOp*> todo;
            todo.push_back(c->where(i)->cast<BinOp>());
            while (!todo.empty()) {
              BinOp* bo = todo.back();
              todo.pop_back();
              if (bo->rhs()->type().isPar()) {
                parWhere.push_back(bo->rhs());
              } else if (bo->rhs()->isa<BinOp>() && bo->rhs()->cast<BinOp>()->op() == BOT_AND) {
                todo.push_back(bo->rhs()->cast<BinOp>());
              } else {
                where.push_back(bo->rhs());
              }
              if (bo->lhs()->type().isPar()) {
                parWhere.push_back(bo->lhs());
              } else if (bo->lhs()->isa<BinOp>() && bo->lhs()->cast<BinOp>()->op() == BOT_AND) {
                todo.push_back(bo->lhs()->cast<BinOp>());
              } else {
                where.push_back(bo->lhs());
              }
            }
            switch (parWhere.size()) {
              case 0:
                orig_where[i] = nullptr;
                break;
              case 1:
                orig_where[i] = parWhere[0];
                break;
              case 2:
                orig_where[i] = new BinOp(c->where(i)->loc(), parWhere[0], BOT_AND, parWhere[1]);
                orig_where[i]->type(Type::parbool());
                break;
              default: {
                auto* parWhereAl = new ArrayLit(c->where(i)->loc(), parWhere);
                parWhereAl->type(Type::parbool(1));
                Call* forall = new Call(c->where(i)->loc(), constants().ids.forall, {parWhereAl});
                forall->type(Type::parbool());
                forall->decl(env.model->matchFn(env, forall, false));
                orig_where[i] = forall;
                break;
              }
            }
          } else {
            orig_where[i] = nullptr;
            where.push_back(c->where(i));
          }
        } else {
          orig_where[i] = c->where(i);
        }
      }
    }
    if (!where.empty()) {
      Generators gs;
      for (int i = 0; i < c->numberOfGenerators(); i++) {
        std::vector<VarDecl*> vds(c->numberOfDecls(i));
        for (int j = 0; j < c->numberOfDecls(i); j++) {
          vds[j] = c->decl(i, j);
        }
        gs.g.emplace_back(vds, in[i], orig_where[i]);
      }
      Expression* cond;
      if (where.size() > 1) {
        auto* al = new ArrayLit(Location().introduce(), where);
        al->type(Type::varbool(1));
        std::vector<Expression*> args(1);
        args[0] = al;
        Call* forall = new Call(Location().introduce(), constants().ids.forall, args);
        forall->type(Type::varbool());
        forall->decl(env.model->matchFn(env, forall, false));
        cond = forall;
      } else {
        cond = where[0];
      }

      Expression* new_e;

      Call* surround = env.surroundingCall();

      Type ntype = c->type();
      if ((surround != nullptr) && surround->id() == constants().ids.forall) {
        new_e = new BinOp(Location().introduce(), cond, BOT_IMPL, c->e());
        new_e->type(Type::varbool());
        ntype.ot(Type::OT_PRESENT);
      } else if ((surround != nullptr) && surround->id() == constants().ids.exists) {
        new_e = new BinOp(Location().introduce(), cond, BOT_AND, c->e());
        new_e->type(Type::varbool());
        ntype.ot(Type::OT_PRESENT);
      } else if ((surround != nullptr) && surround->id() == constants().ids.sum) {
        ITE* if_b_else_zero = new ITE(c->loc().introduce(), {cond, c->e()}, IntLit::a(0));
        Type tt;
        tt = c->e()->type();
        tt.ti(Type::TI_VAR);
        tt.ot(Type::OT_PRESENT);
        if_b_else_zero->type(tt);
        new_e = if_b_else_zero;
        ntype.ot(Type::OT_PRESENT);
      } else {
        ITE* if_b_else_absent = new ITE(c->loc().introduce(), {cond, c->e()}, constants().absent);
        Type tt;
        tt = c->e()->type();
        tt.ti(Type::TI_VAR);
        tt.ot(Type::OT_OPTIONAL);
        if_b_else_absent->type(tt);
        new_e = if_b_else_absent;
      }
      auto* nc = new Comprehension(c->loc(), new_e, gs, c->set());
      nc->type(ntype);
      c = nc;
      c_ka = c;
    }
  }

  class EvalF : public EvalBase {
  public:
    Ctx ctx;
    EvalF(const Ctx& ctx0) : ctx(ctx0) {}
    typedef EE ArrayVal;
    EE e(EnvI& env, Expression* e0) const {
      VarDecl* b = ctx.b == C_ROOT ? constants().varTrue : nullptr;
      VarDecl* r = (ctx.b == C_ROOT && e0->type().isbool() && !e0->type().isOpt())
                       ? constants().varTrue
                       : nullptr;
      return flat_exp(env, ctx, e0, r, b);
    }
    static Expression* flatten(EnvI& env, Expression* e0) {
      return flat_exp(env, Ctx(), e0, nullptr, constants().varTrue).r();
    }
  } _evalf(ctx);
  std::vector<EE> elems_ee;
  bool wasUndefined = false;
  try {
    elems_ee = eval_comp<EvalF>(env, _evalf, c);
  } catch (ResultUndefinedError&) {
    wasUndefined = true;
  }
  std::vector<Expression*> elems(elems_ee.size());
  Type elemType = Type::bot();
  bool allPar = true;
  bool someOpt = false;
  for (auto i = static_cast<unsigned int>(elems.size()); (i--) != 0U;) {
    elems[i] = elems_ee[i].r();
    if (elemType == Type::bot()) {
      elemType = elems[i]->type();
    }
    if (!elems[i]->type().isPar()) {
      allPar = false;
    }
    if (elems[i]->type().isOpt()) {
      someOpt = true;
    }
  }
  if (elemType.isbot()) {
    elemType = c->type();
    elemType.ti(Type::TI_PAR);
  }
  if (!allPar) {
    elemType.ti(Type::TI_VAR);
  }
  if (someOpt) {
    elemType.ot(Type::OT_OPTIONAL);
  }
  if (c->set()) {
    elemType.st(Type::ST_SET);
  } else {
    elemType.dim(c->type().dim());
  }
  KeepAlive ka;
  {
    GCLock lock;
    if (c->set()) {
      if (c->type().isPar() && allPar) {
        auto* sl = new SetLit(c->loc(), elems);
        sl->type(elemType);
        Expression* slr = eval_par(env, sl);
        slr->type(elemType);
        ka = slr;
      } else {
        auto* alr = new ArrayLit(Location().introduce(), elems);
        elemType.st(Type::ST_PLAIN);
        elemType.dim(1);
        alr->type(elemType);
        alr->flat(true);
        Call* a2s = new Call(Location().introduce(), "array2set", {alr});
        a2s->decl(env.model->matchFn(env, a2s, false));
        a2s->type(a2s->decl()->rtype(env, {alr}, false));
        EE ee = flat_exp(env, Ctx(), a2s, nullptr, constants().varTrue);
        ka = ee.r();
      }
    } else {
      auto* alr = new ArrayLit(Location().introduce(), elems);
      alr->type(elemType);
      alr->flat(true);
      ka = alr;
    }
  }
  assert(!ka()->type().isbot());
  if (wasUndefined) {
    ret.b = bind(env, Ctx(), b, constants().literalFalse);
  } else {
    ret.b = conj(env, b, Ctx(), elems_ee);
  }
  ret.r = bind(env, Ctx(), r, ka());
  return ret;
}

}  // namespace MiniZinc
