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

EE flatten_vardecl(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  GCLock lock;
  auto* v = e->cast<VarDecl>();
  if (ctx.b != C_ROOT && !v->toplevel()) {
    throw InternalError("attempting to flatten non-toplevel VarDecl in context other than root");
  }
  VarDecl* it = v->flat();
  if (it == nullptr) {
    TypeInst* ti = eval_typeinst(env, Ctx(), v);
    if ((ti->domain() != nullptr) && ti->domain()->isa<SetLit>()) {
      if (ti->type().bt() == Type::BT_INT && ti->type().st() == Type::ST_PLAIN) {
        if (eval_intset(env, ti->domain())->empty()) {
          env.fail("domain is empty");
        }
      } else if (ti->type().bt() == Type::BT_FLOAT) {
        if (eval_floatset(env, ti->domain())->empty()) {
          env.fail("domain is empty");
        }
      }
    }
    bool reuseVarId = v->type().isAnn() || (v->toplevel() && v->id()->idn() == -1 &&
                                            Printer::quoteId(v->id()->v()).c_str()[0] != '\'' &&
                                            v->id()->v().c_str()[0] != '_');
    VarDecl* vd = new_vardecl(env, Ctx(), ti, reuseVarId ? v->id() : nullptr, v, nullptr);
    v->flat(vd);
    if (v->e() != nullptr) {
      Ctx nctx;
      if (v->e()->type().bt() == Type::BT_BOOL && v->e()->type().dim() == 0) {
        nctx.b = C_MIX;
      }
      (void)flat_exp(env, nctx, v->e(), vd, env.constants.varTrue);
      if (v->e()->type().dim() > 0) {
        Expression* ee = follow_id_to_decl(vd->e());
        if (ee->isa<VarDecl>()) {
          ee = ee->cast<VarDecl>()->e();
        }
        assert(ee && ee->isa<ArrayLit>());
        auto* al = ee->cast<ArrayLit>();
        if (vd->ti()->domain() != nullptr) {
          for (unsigned int i = 0; i < al->size(); i++) {
            if (Id* ali_id = (*al)[i]->dynamicCast<Id>()) {
              if (ali_id != env.constants.absent && ali_id->decl()->ti()->domain() == nullptr) {
                ali_id->decl()->ti()->domain(vd->ti()->domain());
              }
            }
          }
        }
      }
      if (vd->e() != nullptr && vd->e()->type().isPar() && !vd->ti()->type().isPar()) {
        // Flattening the RHS resulted in a par expression. Make the variable par.
        Type t(vd->ti()->type());
        t.ti(Type::TI_PAR);
        vd->ti()->type(t);
        vd->type(t);
      }
    }

    ret.r = bind(env, Ctx(), r, vd->id());
  } else {
    ret.r = bind(env, Ctx(), r, it);
  }
  ret.b = bind(env, Ctx(), b, env.constants.literalTrue);
  return ret;
}
}  // namespace MiniZinc
