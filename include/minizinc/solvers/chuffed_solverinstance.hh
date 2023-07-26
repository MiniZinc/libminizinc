/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>

#include <chuffed/flatzinc/flatzinc.h>
#include <chuffed/vars/bool-view.h>
#include <chuffed/vars/int-var.h>

#undef ERROR

namespace MiniZinc {

class ChuffedOptions : public SolverInstanceBase::Options {
public:
  bool allSolutions = false;
  bool freeSearch = false;
  int nrSolutions = 1;
  int randomSeed = 0;
  bool statistics = false;
  std::chrono::milliseconds time = std::chrono::milliseconds(0);
};

class ChuffedVariable {
public:
  enum VarType { BOOL_TYPE, INT_TYPE };

protected:
  VarType _t;
  int _index;

public:
  static ChuffedVariable boolVar(int idx) {
    return ChuffedVariable(ChuffedVariable::BOOL_TYPE, idx);
  }
  static ChuffedVariable intVar(int idx) { return ChuffedVariable(ChuffedVariable::INT_TYPE, idx); }

  bool isInt() const { return _t == INT_TYPE; }
  bool isBool() const { return _t == BOOL_TYPE; }

  int index() const { return _index; }

  IntVar* intVar(FlatZinc::FlatZincSpace* space) const {
    assert(isInt());
    assert(index() < space->iv.size());
    return space->iv[index()];
  }

  BoolView& boolVar(FlatZinc::FlatZincSpace* space) const {
    assert(isBool());
    assert(index() < space->bv.size());
    return space->bv[index()];
  }

private:
  ChuffedVariable(VarType t, size_t idx) : _t(t), _index(static_cast<int>(idx)) {}
};

class ChuffedTypes {
public:
  typedef ChuffedVariable Variable;
  typedef MiniZinc::Statistics Statistics;
};

class ChuffedSolverInstance : public SolverInstanceImpl<ChuffedTypes> {
public:
  ChuffedSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  ~ChuffedSolverInstance() override { delete _space; };
  void processFlatZinc() override;

  Status solve() override;
  Status next() override { return SolverInstance::ERROR; }
  void resetSolver() override { assert(false); };

  Expression* getSolutionValue(Id* id) override;
  void printStatistics() override;

protected:
  Model* _flat;
  FlatZinc::FlatZincSpace* _space;
  bool _isSatisfaction = true;
};

class ChuffedSolverFactory : public SolverFactory {
public:
  SolverInstanceBase::Options* createOptions() override;
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

  std::string getDescription(SolverInstanceBase::Options* opt) override {
    return "Chuffed - a lazy clause generation solver";
  };
  std::string getVersion(SolverInstanceBase::Options* opt) override;
  std::string getId() override { return "org.minizinc.chuffed"; }

  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
};

}  // namespace MiniZinc
