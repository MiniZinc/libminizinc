/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
//#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {

class FZNSolverOptions : public SolverInstanceBase::Options {
public:
  std::string fznSolver;
  std::string backend;
  std::vector<std::string> fznFlags;
  int numSols = 1;
  std::string parallel;
  int fznTimeLimitMilliseconds = 0;
  int solverTimeLimitMilliseconds = 0;
  bool fznSigint = false;
  /// Number of (optimal) solutions to output
  bool numOptimal = true;
  bool allOptimal = false;

  bool fznNeedsPaths = false;
  bool fznOutputPassthrough = false;

  bool supportsA = false;
  bool supportsN = false;
  bool supportsF = false;
  bool supportsP = false;
  bool supportsS = false;
  bool supportsR = false;
  bool supportsV = false;
  bool supportsT = false;
  bool supportsI = false;
  bool supportsNO = false;
  bool supportsAO = false;
  bool supportsCpprofiler = false;
  std::vector<MZNFZNSolverFlag> fznSolverFlags;
};

class FZNSolverInstance : public SolverInstanceBase {
private:
  std::string _fznSolver;

protected:
  Model* _fzn;
  Model* _ozn;

public:
  FZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

  ~FZNSolverInstance() override;

  Status next() override { return SolverInstance::ERROR; }

  Status solve() override;

  void processFlatZinc() override;

  void resetSolver() override;

protected:
  static Expression* getSolutionValue(Id* id);
};

class FZNSolverFactory : public SolverFactory {
protected:
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

public:
  FZNSolverFactory();
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
