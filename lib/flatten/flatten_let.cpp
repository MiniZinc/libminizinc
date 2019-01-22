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
  
  EE flatten_let(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    Let* let = e->cast<Let>();
    GC::mark();
    std::vector<EE> cs;
    std::vector<KeepAlive> flatmap;
    let->pushbindings();
    for (unsigned int i=0; i<let->let().size(); i++) {
      Expression* le = let->let()[i];
      if (VarDecl* vd = le->dyn_cast<VarDecl>()) {
        Expression* let_e = NULL;
        if (vd->e()) {
          Ctx nctx = ctx;
          nctx.neg = false;
          if (vd->e()->type().bt()==Type::BT_BOOL)
            nctx.b = C_MIX;
          
          EE ee = flat_exp(env,nctx,vd->e(),NULL,NULL);
          let_e = ee.r();
          cs.push_back(ee);
          if (vd->ti()->domain() != NULL) {
            GCLock lock;
            std::vector<Expression*> domargs(2);
            domargs[0] = ee.r();
            if (vd->ti()->type().isfloat()) {
              FloatSetVal* fsv = eval_floatset(env, vd->ti()->domain());
              if (fsv->size()==1) {
                domargs[1] = FloatLit::a(fsv->min());
                domargs.push_back(FloatLit::a(fsv->max()));
              } else {
                domargs[1] = vd->ti()->domain();
              }
            } else {
              domargs[1] = vd->ti()->domain();
            }
            Call* c = new Call(vd->ti()->loc().introduce(),"var_dom",domargs);
            c->type(Type::varbool());
            c->decl(env.model->matchFn(env,c,false));
            if (c->decl()==NULL)
              throw InternalError("no matching declaration found for var_dom");
            VarDecl* b_b = (nctx.b==C_ROOT && b==constants().var_true) ? b : NULL;
            VarDecl* r_r = (nctx.b==C_ROOT && b==constants().var_true) ? b : NULL;
            ee = flat_exp(env, nctx, c, r_r, b_b);
            cs.push_back(ee);
            ee.b = ee.r;
            cs.push_back(ee);
          }
          if (vd->type().dim() > 0) {
            checkIndexSets(env, vd, let_e);
          }
        } else {
          if ((ctx.b==C_NEG || ctx.b==C_MIX) && !vd->ann().contains(constants().ann.promise_total)) {
            CallStackItem csi_vd(env, vd);
            throw FlatteningError(env,vd->loc(),
                                  "free variable in non-positive context");
          }
          CallStackItem csi_vd(env, vd);
          GCLock lock;
          TypeInst* ti = eval_typeinst(env,vd);
          VarDecl* nvd = newVarDecl(env, ctx, ti, NULL, vd, NULL);
          let_e = nvd->id();
        }
        vd->e(let_e);
        flatmap.push_back(vd->flat());
        if (Id* id = Expression::dyn_cast<Id>(let_e)) {
          vd->flat(id->decl());
        } else {
          vd->flat(vd);
        }
      } else {
        if (ctx.b==C_ROOT || le->ann().contains(constants().ann.promise_total)) {
          (void) flat_exp(env,Ctx(),le,constants().var_true,constants().var_true);
        } else {
          EE ee = flat_exp(env,ctx,le,NULL,constants().var_true);
          ee.b = ee.r;
          cs.push_back(ee);
        }
      }
    }
    if (r==constants().var_true && ctx.b==C_ROOT && !ctx.neg) {
      ret.b = bind(env,Ctx(),b,constants().lit_true);
      (void) flat_exp(env,ctx,let->in(),r,b);
      ret.r = conj(env,r,Ctx(),cs);
    } else {
      Ctx nctx = ctx;
      nctx.neg = false;
      VarDecl* bb = b;
      for (EE& ee : cs) {
        if (ee.b() != constants().lit_true) {
          bb = NULL;
          break;
        }
      }
      EE ee = flat_exp(env,nctx,let->in(),NULL,bb);
      if (let->type().isbool() && !let->type().isopt()) {
        ee.b = ee.r;
        cs.push_back(ee);
        ret.r = conj(env,r,ctx,cs);
        ret.b = bind(env,Ctx(),b,constants().lit_true);
      } else {
        cs.push_back(ee);
        ret.r = bind(env,Ctx(),r,ee.r());
        ret.b = conj(env,b,Ctx(),cs);
      }
    }
    let->popbindings();
    // Restore previous mapping
    for (unsigned int i=0; i<let->let().size(); i++) {
      if (VarDecl* vd = let->let()[i]->dyn_cast<VarDecl>()) {
        vd->flat(Expression::cast<VarDecl>(flatmap.back()()));
        flatmap.pop_back();
      }
    }
    return ret;
  }
}
