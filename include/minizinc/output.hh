/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/gc.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/values.hh>

#include <sstream>

namespace MiniZinc {

/// Remove all output annotations from \a vd
void remove_is_output(VarDecl* vd);

/// Copy output item to FlatZinc model
void copy_output(EnvI& e);

/// Copy all dependent variable declarations
void output_vardecls(EnvI& env, Item* ci, Expression* e);

/// Populate ::output annotations for model
void process_toplevel_output_vars(EnvI& e);

/// Create initial output model
void create_output(EnvI& e, FlatteningOptions::OutputMode outputMode, bool outputObjective,
                   bool includeOutputItem, bool hasChecker, bool encapsulateJSON);

void check_output_par_fn(EnvI& e, Call* rhs);

/// Finalise output model after flattening is complete
void finalise_output(EnvI& e);

/// Remove all links to variables in flat model from output model in \a env
void cleanup_output(EnvI& env);

ArrayLit* create_json_output(EnvI& env, bool includeObjective, bool includeOutputItem,
                             bool includeChecker);

inline void display_enum_range(std::ostringstream& ss, EnvI& env, IntVal min, IntVal max,
                               unsigned int enumId) {
  if (enumId == 0) {
    ss << min << ".." << max;
    return;
  }
  auto* vd = env.getEnum(enumId)->e();
  IntVal card;
  {
    GCLock lock;
    IntSetVal* isv = eval_intset(env, vd->e());
    card = isv->card();
  }
  if (card == (max + 1 - min)) {
    ss << *vd->id();
  } else if (max + 1 - min == 0) {
    ss << "{}";
  } else {
    GCLock lock;
    ASTString enumName(create_enum_to_string_name(vd->id(), "_toString_"));
    auto* call = Call::a(Location().introduce(), enumName,
                         {IntLit::a(min), env.constants.literalTrue, env.constants.literalFalse});
    auto* fi = env.model->matchFn(env, call, false, true);
    call->decl(fi);
    call->type(Type::parstring());
    ss << eval_string(env, call);
    call->arg(0, IntLit::a(max));
    ss << ".." << eval_string(env, call);
  }
}

}  // namespace MiniZinc
