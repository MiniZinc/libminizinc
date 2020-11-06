/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver.hh>

namespace MiniZinc {

class MZNSolverOptions : public SolverInstanceBase::Options {
public:
  std::string mznSolver;
  std::vector<std::string> mznFlags;
  int numSols = 1;
  bool allSols = false;
  std::string parallel;
  int mznTimeLimitMilliseconds = 0;
  int solverTimeLimitMilliseconds = 0;
  bool mznSigint = false;
  bool supportsT = false;
  std::vector<MZNFZNSolverFlag> mznSolverFlags;
};

class MZNSolverInstance : public SolverInstanceBase {
private:
  std::string _mznSolver;

public:
  MZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

  ~MZNSolverInstance() override;

  Status next() override { return SolverInstance::ERROR; }

  Status solve() override;

  void processFlatZinc() override;

  void resetSolver() override;
};

class MZNSolverFactory : public SolverFactory {
protected:
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

public:
  MZNSolverFactory();
  SolverInstanceBase::Options* createOptions() override;
  std::string getDescription(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getVersion(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getId() override;
  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
  void printHelp(std::ostream& os) override;
  static void setAcceptedFlags(SolverInstanceBase::Options* opt,
                               const std::vector<MZNFZNSolverFlag>& flags);
};

}  // namespace MiniZinc
