/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
void add_path_annotation(EnvI& env, Expression* e);
void add_ctx_ann(VarDecl* vd, BCtx& c);
bool istrue(EnvI& env, Expression* e);
bool isfalse(EnvI& env, Expression* e);
Expression* create_dummy_value(EnvI& env, const Type& t);
TypeInst* eval_typeinst(EnvI& env, const Ctx& ctx, VarDecl* vd);

KeepAlive bind(EnvI& env, Ctx ctx, VarDecl* vd, Expression* e);
KeepAlive conj(EnvI& env, VarDecl* b, const Ctx& ctx, const std::vector<EE>& e);

VarDecl* new_vardecl(EnvI& env, const Ctx& ctx, TypeInst* ti, Id* origId, VarDecl* origVd,
                     Expression* rhs);

KeepAlive flat_cv_exp(EnvI& env, Ctx ctx, Expression* e);

void make_defined_var(VarDecl* vd, Call* c);
void check_index_sets(EnvI& env, VarDecl* vd, Expression* e);

class CallArgItem {
public:
  EnvI& env;
  CallArgItem(EnvI& env0);
  ~CallArgItem();
};

}  // namespace MiniZinc
