/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/astexception.hh>
#include <minizinc/model.hh>

#include <vector>

namespace MiniZinc {

/// EXPERIMENTAL: black-box propagator body synthesis.
///
/// For each bodyless predicate/function annotated with a black-box propagator
/// annotation (`minizinc_value_propagator` / `minizinc_bounds_propagator`), this
/// generates its body as a call to the generic `blackbox` / `blackbox_bounds`
/// constraint carrying the propagator's `blackbox_exec` / `blackbox_dll`
/// annotation.
///
/// Must be called during type checking after the left-hand sides (function
/// signatures) have been typed, but before the function bodies are type checked,
/// so the synthesised bodies are type checked by the normal machinery.
void synthesize_blackbox_bodies(EnvI& env, Model* m, const std::vector<FunctionI*>& functionItems,
                                std::vector<TypeError>& typeErrors);

}  // namespace MiniZinc
