/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/MIP/MIP_highs_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_highs_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPHiGHSWrapper> _highs_solver_factory; }
}  // namespace
HiGHSSolverFactoryInitialiser::HiGHSSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
