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

/// Remove all output annotations from \a vd
void remove_is_output(VarDecl* vd);

/// Copy output item to FlatZinc model
void copy_output(EnvI& e);

/// Copy all dependent variable declarations
void output_vardecls(EnvI& env, Item* ci, Expression* e);

/// Create initial output model
void create_output(EnvI& e, FlatteningOptions::OutputMode outputMode, bool outputObjective,
                   bool includeOutputItem, bool hasChecker);

void check_output_par_fn(EnvI& e, Call* rhs);

/// Finalise output model after flattening is complete
void finalise_output(EnvI& e);

/// Remove all links to variables in flat model from output model in \a env
void cleanup_output(EnvI& env);

ArrayLit* create_json_output(EnvI& env, bool outputObjective, bool includeOutputItem,
                             bool hasChecker);

}  // namespace MiniZinc
