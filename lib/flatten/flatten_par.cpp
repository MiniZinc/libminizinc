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

EE flatten_par(EnvI& env, const Ctx& ctx, Expression* e, VarDecl* r, VarDecl* b) {
  EE ret;
  if (e->type().cv()) {
    KeepAlive ka = flat_cv_exp(env, ctx, e);
    ret.r = bind(env, ctx, r, ka());
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
    return ret;
  }
  if (e->type().dim() > 0) {
    EnvI::CSEMap::iterator it;
    Id* id = e->dynamicCast<Id>();
    if ((id != nullptr) && (id->decl()->flat() == nullptr || id->decl()->toplevel())) {
      VarDecl* vd = id->decl()->flat();
      if (vd == nullptr) {
        vd = flat_exp(env, Ctx(), id->decl(), nullptr, constants().varTrue).r()->cast<Id>()->decl();
        id->decl()->flat(vd);
        auto* al = follow_id(vd->id())->cast<ArrayLit>();
        if (al->size() == 0) {
          if (r == nullptr) {
            ret.r = al;
          } else {
            ret.r = bind(env, ctx, r, al);
          }
          ret.b = bind(env, Ctx(), b, constants().literalTrue);
          return ret;
        }
      }
      ret.r = bind(env, ctx, r, e->cast<Id>()->decl()->flat()->id());
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      return ret;
    }
    if ((it = env.cseMapFind(e)) != env.cseMapEnd()) {
      ret.r = bind(env, ctx, r, it->second.r()->cast<VarDecl>()->id());
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      return ret;
    }
    GCLock lock;
    auto* al = follow_id(eval_par(env, e))->cast<ArrayLit>();
    if (al->size() == 0 || ((r != nullptr) && r->e() == nullptr)) {
      if (r == nullptr) {
        ret.r = al;
      } else {
        ret.r = bind(env, ctx, r, al);
      }
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      return ret;
    }
    if ((it = env.cseMapFind(al)) != env.cseMapEnd()) {
      ret.r = bind(env, ctx, r, it->second.r()->cast<VarDecl>()->id());
      ret.b = bind(env, Ctx(), b, constants().literalTrue);
      return ret;
    }
    std::vector<TypeInst*> ranges(al->dims());
    for (unsigned int i = 0; i < ranges.size(); i++) {
      ranges[i] =
          new TypeInst(e->loc(), Type(),
                       new SetLit(Location().introduce(), IntSetVal::a(al->min(i), al->max(i))));
    }
    ASTExprVec<TypeInst> ranges_v(ranges);
    assert(!al->type().isbot());
    auto* ti = new TypeInst(e->loc(), al->type(), ranges_v, nullptr);
    VarDecl* vd = new_vardecl(env, ctx, ti, nullptr, nullptr, al);
    EE ee(vd, nullptr);
    env.cseMapInsert(al, ee);
    env.cseMapInsert(vd->e(), ee);

    ret.r = bind(env, ctx, r, vd->id());
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
    return ret;
  }
  GCLock lock;
  try {
    ret.r = bind(env, ctx, r, eval_par(env, e));
    ret.b = bind(env, Ctx(), b, constants().literalTrue);
  } catch (ResultUndefinedError&) {
    ret.r = create_dummy_value(env, e->type());
    ret.b = bind(env, Ctx(), b, constants().literalFalse);
  }
  return ret;
}

}  // namespace MiniZinc
