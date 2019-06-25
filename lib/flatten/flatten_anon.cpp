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

  EE flatten_anon(EnvI& env,Ctx ctx, Expression* e, VarDecl* r, VarDecl* b) {
    CallStackItem _csi(env,e);
    EE ret;
    AnonVar* av = e->cast<AnonVar>();
    if (av->type().isbot()) {
      throw InternalError("type of anonymous variable could not be inferred");
    }
    GCLock lock;
    VarDecl* vd = newVarDecl(env, Ctx(), new TypeInst(Location().introduce(), av->type()), NULL, NULL, NULL);
    ret.b = bind(env, Ctx(), b, constants().lit_true);
    ret.r = bind(env, ctx, r, vd->id());
    return ret;
  }

}
