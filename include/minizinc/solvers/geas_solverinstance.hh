/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_GEAS_SOLVER_INSTANCE_HH__
#define __MINIZINC_GEAS_SOLVER_INSTANCE_HH__

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>

namespace MiniZinc {

  class GeasOptions : public SolverInstanceBase::Options {};

  class GeasSolverInstance : public SolverInstanceBase {
  public:
    GeasSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

    ~GeasSolverInstance() override = default;

    Status next() override { return SolverInstance::ERROR; }

    Status solve() override;

    void processFlatZinc() override;

    void resetSolver() override;
  };

  class Geas_SolverFactory: public SolverFactory {
  public:
    Geas_SolverFactory();
    SolverInstanceBase::Options* createOptions() override;
    SolverInstanceBase* doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) override;

    std::string getDescription(SolverInstanceBase::Options* opt=nullptr) override { return "Elsie Geas - Another Lazy Clause Generation Solver"; };
    std::string getVersion(SolverInstanceBase::Options* opt=nullptr) override { return "0.0.1"; }
    std::string getId() override { return "org.minizinc.geas"; }

    bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv) override;
    void printHelp(std::ostream& os) override;
  };

}

#endif // __MINIZINC_GEAS_SOLVER_INSTANCE_HH__
