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
  auto* v = Expression::cast<VarDecl>(e);
  if (ctx.b != C_ROOT && !v->toplevel()) {
    throw InternalError("attempting to flatten non-toplevel VarDecl in context other than root");
  }
  VarDecl* it = v->flat();
  if (it == nullptr) {
    TypeInst* ti = eval_typeinst(env, Ctx(), v);
    bool isEmptyArray = false;
    for (auto* nti : ti->ranges()) {
      if (nti->domain() != nullptr &&
          (Expression::isa<SetLit>(nti->domain()) && eval_intset(env, nti->domain())->empty())) {
        isEmptyArray = true;
        break;
      }
    }
    if (!isEmptyArray && ti->domain() != nullptr && Expression::isa<SetLit>(ti->domain())) {
      if (ti->type().bt() == Type::BT_INT && ti->type().st() == Type::ST_PLAIN &&
          ti->type().ot() == Type::OT_PRESENT) {
        if (eval_intset(env, ti->domain())->empty()) {
          env.fail("domain is empty");
        }
      } else if (ti->type().bt() == Type::BT_FLOAT) {
        if (eval_floatset(env, ti->domain())->empty()) {
          env.fail("domain is empty");
        }
      }
    }
    bool reuseVarId =
        v->type().isAnn() ||
        (v->toplevel() && v->id()->idn() == -1 && v->type().dim() <= 1 &&
         Printer::quoteId(v->id()->v()).c_str()[0] != '\'' && v->id()->v().c_str()[0] != '_');
    if (reuseVarId && v->type().dim() == 1) {
      auto* arrayRanges = ti->ranges()[0]->domain();
      if (arrayRanges == nullptr || !Expression::isa<SetLit>(arrayRanges) ||
          Expression::cast<SetLit>(arrayRanges)->isv() == nullptr ||
          (!Expression::cast<SetLit>(arrayRanges)->isv()->empty() &&
           Expression::cast<SetLit>(arrayRanges)->isv()->min() != 1)) {
        reuseVarId = false;
      }
    }
    VarDecl* vd = new_vardecl(env, Ctx(), ti, reuseVarId ? v->id() : nullptr, v, nullptr);
    v->flat(vd);
    if (v->e() != nullptr) {
      Ctx nctx;
      if (Expression::type(v->e()).bt() == Type::BT_BOOL && Expression::type(v->e()).dim() == 0) {
        nctx.b = C_MIX;
      }
      (void)flat_exp(env, nctx, v->e(), vd, env.constants.varTrue);
      if (Expression::type(v->e()).dim() > 0) {
        Expression* ee = follow_id_to_decl(vd->e());
        if (Expression::isa<VarDecl>(ee)) {
          ee = Expression::cast<VarDecl>(ee)->e();
        }
        auto* al = Expression::cast<ArrayLit>(ee);
        if (vd->ti()->domain() != nullptr) {
          for (unsigned int i = 0; i < al->size(); i++) {
            if (Id* ali_id = Expression::dynamicCast<Id>((*al)[i])) {
              if (ali_id != env.constants.absent && ali_id->decl()->ti()->domain() == nullptr) {
                ali_id->decl()->ti()->domain(vd->ti()->domain());
              }
            }
          }
        }
      }
      if (vd->e() != nullptr && Expression::type(vd->e()).isPar() && !vd->ti()->type().isPar()) {
        // Flattening the RHS resulted in a par expression. Make the variable par.
        Type t(vd->ti()->type());
        t.mkPar(env);
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
