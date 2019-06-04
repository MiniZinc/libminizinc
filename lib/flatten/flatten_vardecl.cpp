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
  
  EE flatten_vardecl(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    GCLock lock;
    if (ctx.b != C_ROOT)
      throw FlatteningError(env,e->loc(), "not in root context");
    VarDecl* v = e->cast<VarDecl>();
    VarDecl* it = v->flat();
    if (it==NULL) {
      TypeInst* ti = eval_typeinst(env,v);
      bool reuseVarId = v->type().isann() || ( v->toplevel() && v->id()->idn()==-1 && v->id()->v().c_str()[0]!='\'' && v->id()->v().c_str()[0]!='_' );
      VarDecl* vd = newVarDecl(env, ctx, ti, reuseVarId ? v->id() : NULL, v, NULL);
      v->flat(vd);
      Ctx nctx;
      if (v->e() && v->e()->type().bt() == Type::BT_BOOL)
        nctx.b = C_MIX;
      if (v->e()) {
        (void) flat_exp(env,nctx,v->e(),vd,constants().var_true);
        if (v->e()->type().dim() > 0) {
          Expression* ee = follow_id_to_decl(vd->e());
          if (ee->isa<VarDecl>())
            ee = ee->cast<VarDecl>()->e();
          assert(ee && ee->isa<ArrayLit>());
          ArrayLit* al = ee->cast<ArrayLit>();
          if (vd->ti()->domain()) {
            for (unsigned int i=0; i<al->size(); i++) {
              if (Id* ali_id = (*al)[i]->dyn_cast<Id>()) {
                if (ali_id->decl()->ti()->domain()==NULL) {
                  ali_id->decl()->ti()->domain(vd->ti()->domain());
                }
              }
            }
          }
        }
      }
      
      ret.r = bind(env,Ctx(),r,vd->id());
    } else {
      ret.r = bind(env,Ctx(),r,it);
    }
    ret.b = bind(env,Ctx(),b,constants().lit_true);
    return ret;
  }
}
