/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/geas_solverfactory.hh>
#include <minizinc/solvers/geas_solverinstance.hh>

namespace MiniZinc {
  namespace {
    void getWrapper() {
      static Geas_SolverFactory _geas_solverfactory;
    }
  }
  Geas_SolverFactoryInitialiser::Geas_SolverFactoryInitialiser() {
    getWrapper();
  }
}