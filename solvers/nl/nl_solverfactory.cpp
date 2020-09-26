/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/nl/nl_solverfactory.hh>
#include <minizinc/solvers/nl/nl_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static NLSolverFactory _nl_solverfactory; }
}  // namespace

NLSolverFactoryInitialiser::NLSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
