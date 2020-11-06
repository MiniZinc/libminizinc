/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/nl/nl_file.hh>

namespace MiniZinc {

class NLSolverOptions : public SolverInstanceBase::Options {
public:
  std::string nlSolver;
  std::vector<std::string> nlFlags;
  std::vector<MZNFZNSolverFlag> nlSolverFlags;
  bool doHexafloat = false;
  bool doKeepfile = false;
};

class NLSolverInstance : public SolverInstanceBase {
protected:
  Model* _fzn;
  Model* _ozn;

  NLFile _nlFile;

public:
  NLSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);

  ~NLSolverInstance() override;

  Status next() override { return SolverInstance::Status::ERROR; }

  Status solve() override;

  void processFlatZinc() override;

  void resetSolver() override;

protected:
  static Expression* getSolutionValue(Id* id);

  void analyse(const Item* i);
};

class NLSolverFactory : public SolverFactory {
protected:
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

public:
  NLSolverFactory();
  SolverInstanceBase::Options* createOptions() override;
  std::string getDescription(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getVersion(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getId() override;
  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
  void printHelp(std::ostream& os) override;
  // void setAcceptedFlags(SolverInstanceBase::Options* opt, const std::vector<MZNFZNSolverFlag>&
  // flags);
};

}  // namespace MiniZinc
