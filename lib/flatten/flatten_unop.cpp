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

EE flatten_unop(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  CallStackItem _csi(env, e);
  UnOp* uo = Expression::cast<UnOp>(e);

  bool isBuiltin = uo->decl() == nullptr || uo->decl()->e() == nullptr;

  if (isBuiltin) {
    switch (uo->op()) {
      case UOT_NOT: {
        Ctx nctx = ctx;
        nctx.b = -nctx.b;
        nctx.neg = !nctx.neg;
        return flat_exp(env, nctx, uo->e(), r, b);
      }
      case UOT_PLUS:
        return flat_exp(env, ctx, uo->e(), r, b);
      case UOT_MINUS: {
        GC::lock();
        if (UnOp* uo_inner = Expression::dynamicCast<UnOp>(uo->e())) {
          if (uo_inner->op() == UOT_MINUS) {
            return flat_exp(env, ctx, uo_inner->e(), r, b);
          }
        }
        Expression* zero;
        if (Expression::type(uo->e()).bt() == Type::BT_INT) {
          zero = IntLit::a(0);
        } else {
          zero = FloatLit::a(0.0);
        }
        auto* bo = new BinOp(Location().introduce(), zero, BOT_MINUS, uo->e());
        bo->type(uo->type());
        KeepAlive ka(bo);
        GC::unlock();
        return flat_exp(env, ctx, ka(), r, b);
      }
      default:
        throw InternalError("unhandled unary operator");
    }
  }  // else (!isBuiltin)
  GC::lock();
  Call* c = Call::a(Expression::loc(uo).introduce(), uo->opToString(), {uo->e()});
  c->decl(env.model->matchFn(env, c, false));
  c->type(uo->type());
  KeepAlive ka(c);
  GC::unlock();
  return flat_exp(env, ctx, c, r, b);
}
}  // namespace MiniZinc
