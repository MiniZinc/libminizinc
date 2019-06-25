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
  
  EE flatten_unop(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    UnOp* uo = e->cast<UnOp>();
    switch (uo->op()) {
      case UOT_NOT:
      {
        Ctx nctx = ctx;
        nctx.b = -nctx.b;
        nctx.neg = !nctx.neg;
        ret = flat_exp(env,nctx,uo->e(),r,b);
      }
        break;
      case UOT_PLUS:
        ret = flat_exp(env,ctx,uo->e(),r,b);
        break;
      case UOT_MINUS:
      {
        GC::lock();
        if (UnOp* uo_inner = uo->e()->dyn_cast<UnOp>()) {
          if (uo_inner->op()==UOT_MINUS) {
            ret = flat_exp(env,ctx,uo_inner->e(),r,b);
            break;
          }
        }
        Expression* zero;
        if (uo->e()->type().bt()==Type::BT_INT)
          zero = IntLit::a(0);
        else
          zero = FloatLit::a(0.0);
        BinOp* bo = new BinOp(Location().introduce(),zero,BOT_MINUS,uo->e());
        bo->type(uo->type());
        KeepAlive ka(bo);
        GC::unlock();
        ret = flat_exp(env,ctx,ka(),r,b);
      }
        break;
      default: break;
    }
    return ret;
  }
}
