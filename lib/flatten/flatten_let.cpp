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

EE flatten_let(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  EE ret;
  Let* let = e->cast<Let>();
  GC::mark();
  std::vector<EE> cs;
  std::vector<KeepAlive> flatmap;
  let->pushbindings();
  for (unsigned int i = 0; i < let->let().size(); i++) {
    Expression* le = let->let()[i];
    if (auto* vd = le->dynamicCast<VarDecl>()) {
      Expression* let_e = nullptr;
      if (vd->e() != nullptr) {
        Ctx nctx = ctx;
        nctx.neg = false;
        if (vd->e()->type().bt() == Type::BT_BOOL) {
          nctx.b = C_MIX;
        }

        EE ee = flat_exp(env, nctx, vd->e(), nullptr, nullptr);
        let_e = ee.r();
        cs.push_back(ee);
        if (vd->ti()->domain() != nullptr) {
          GCLock lock;
          std::vector<Expression*> domargs(2);
          domargs[0] = ee.r();
          if (vd->ti()->type().isfloat()) {
            FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
            if (fsv->size() == 1) {
              domargs[1] = FloatLit::a(fsv->min());
              domargs.push_back(FloatLit::a(fsv->max()));
            } else {
              domargs[1] = vd->ti()->domain();
            }
          } else {
            domargs[1] = vd->ti()->domain();
          }
          Call* c = new Call(vd->ti()->loc().introduce(), "var_dom", domargs);
          c->type(Type::varbool());
          c->decl(env.model->matchFn(env, c, false));
          if (c->decl() == nullptr) {
            throw InternalError("no matching declaration found for var_dom");
          }
          VarDecl* b_b = (nctx.b == C_ROOT && b == constants().varTrue) ? b : nullptr;
          VarDecl* r_r = (nctx.b == C_ROOT && b == constants().varTrue) ? b : nullptr;
          ee = flat_exp(env, nctx, c, r_r, b_b);
          cs.push_back(ee);
          ee.b = ee.r;
          cs.push_back(ee);
        }
        if (vd->type().dim() > 0) {
          check_index_sets(env, vd, let_e);
        }
      } else {
        if ((ctx.b == C_NEG || ctx.b == C_MIX) &&
            !vd->ann().contains(constants().ann.promise_total)) {
          CallStackItem csi_vd(env, vd);
          throw FlatteningError(env, vd->loc(), "free variable in non-positive context");
        }
        CallStackItem csi_vd(env, vd);
        GCLock lock;
        TypeInst* ti = eval_typeinst(env, ctx, vd);
        VarDecl* nvd = new_vardecl(env, ctx, ti, nullptr, vd, nullptr);
        let_e = nvd->id();
      }
      vd->e(let_e);
      flatmap.emplace_back(vd->flat());
      if (Id* id = Expression::dynamicCast<Id>(let_e)) {
        vd->flat(id->decl());
      } else {
        vd->flat(vd);
      }
    } else {
      if (ctx.b == C_ROOT || le->ann().contains(constants().ann.promise_total)) {
        (void)flat_exp(env, Ctx(), le, constants().varTrue, constants().varTrue);
      } else {
        EE ee = flat_exp(env, ctx, le, nullptr, constants().varTrue);
        ee.b = ee.r;
        cs.push_back(ee);
      }
    }
  }
  if (r == constants().varTrue && ctx.b == C_ROOT && !ctx.neg) {
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
    (void)flat_exp(env, ctx, let->in(), r, b);
    ret.r = conj(env, r, Ctx(), cs);
  } else {
    Ctx nctx = ctx;
    nctx.neg = false;
    VarDecl* bb = b;
    for (EE& ee : cs) {
      if (ee.b() != constants().literalTrue) {
        bb = nullptr;
        break;
      }
    }
    EE ee = flat_exp(env, nctx, let->in(), nullptr, bb);
    if (let->type().isbool() && !let->type().isOpt()) {
      ee.b = ee.r;
      cs.push_back(ee);
      ret.r = conj(env, r, ctx, cs);
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
    } else {
      cs.push_back(ee);
      ret.r = bind(env, Ctx(), r, ee.r());
      ret.b = conj(env, b, Ctx(), cs);
    }
  }
  let->popbindings();
  // Restore previous mapping
  for (unsigned int i = 0; i < let->let().size(); i++) {
    if (auto* vd = let->let()[i]->dynamicCast<VarDecl>()) {
      vd->flat(Expression::cast<VarDecl>(flatmap.back()()));
      flatmap.pop_back();
    }
  }
  return ret;
}
}  // namespace MiniZinc
