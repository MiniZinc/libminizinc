/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>

#include <string>
#include <utility>
#include <vector>

namespace MiniZinc {

class Env;

/// The constraints a solver implements itself, as it reports them.
///
/// A solver that can say what it supports needs no MiniZinc library to say it.
/// What it declares here replaces the standard library's decomposition of the
/// same constraint, and leaves an override from the solver's own \a mznlib —
/// which lives elsewhere — in place.
struct NativePredicates {
  /// The directories whose declarations may be overridden: the standard
  /// library, and any overlay placed in front of it.
  std::vector<std::string> libraryDirs;
  /// Identifier and the parameter types it must have for the solver's
  /// implementation to apply. A constraint appears twice, under the `fzn_`
  /// name a global lowers to and under its bare name, which is the FlatZinc
  /// builtin one.
  std::vector<std::pair<std::string, std::vector<Type>>> predicates;
};

/// Drop the decomposition of every declaration \a np says the solver
/// implements, and give the ones it only implements for fixed arguments a form
/// that says so.
///
/// Called from `typecheck` rather than after it, so that a body dropped here is
/// gone before monomorphisation could make a parameter-type copy of it.
void enable_native_predicates(Env& env, const NativePredicates& np);

/// Give the model body-less declarations for the constraints \a np says the
/// solver implements but that MiniZinc has no name of its own for, so that a
/// model can call them like any other predicate.
///
/// Only done when the model asked for them by including \a includeFile, which
/// is named after the solver. A solver shipping a library of that name keeps
/// whatever it declares there; this only adds what is missing.
void declare_solver_constraints(Env& env, const NativePredicates& np,
                                const std::string& includeFile);

}  // namespace MiniZinc
