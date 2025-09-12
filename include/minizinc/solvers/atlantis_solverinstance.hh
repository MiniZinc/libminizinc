/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip.dekker@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solver_instance.hh>

#include "minizinc/astmap.hh"
#include "minizinc/aststring.hh"

#include <atlantis/fznBackend.hpp>
#include <atlantis/logging/logger.hpp>

namespace MiniZinc {

class AtlantisOptions : public SolverInstanceBase::Options {
public:
  bool intermediateSolutions = false;
  int randomSeed = 0;
  std::chrono::milliseconds time = std::chrono::milliseconds(0);
  atlantis::logging::Level logLevel = atlantis::logging::Level::WARN;
  std::string annealingSchedule;
};

class AtlantisSolverInstance : public SolverInstanceBase2<true> {
public:
  AtlantisSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  ~AtlantisSolverInstance() override {};
  void processFlatZinc() override;

  Status solve() override;
  Status next() override { return SolverInstance::ERROR; }
  void resetSolver() override { assert(false); };

  Expression* getSolutionValue(Id* id) override;
  void printStatistics() override;

protected:
  Model* _fzn;
  atlantis::logging::Logger _logger;
  std::unique_ptr<atlantis::FznBackend> _backend;
  ManagedASTStringMap<Expression*> _assignment;
  atlantis::search::SearchStatistics _statistics;
  bool _hadSol = false;
};

class AtlantisSolverFactory : public SolverFactory {
public:
  SolverInstanceBase::Options* createOptions() override;
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

  std::string getDescription(SolverInstanceBase::Options* opt) override {
    return "Atlantis - a constraint based local search solver";
  };
  std::string getVersion(SolverInstanceBase::Options* opt) override;
  std::string getId() override { return "org.minizinc.atlantis"; }

  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
};

}  // namespace MiniZinc
