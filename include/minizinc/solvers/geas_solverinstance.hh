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

#include <geas/solver/solver.h>

namespace MiniZinc {

class GeasOptions : public SolverInstanceBase::Options {
public:
  bool allSolutions = false;
  int conflicts = 0;
  bool freeSearch = false;
  int nrSolutions = 1;
  int objProbeLimit = 0;
  bool statistics = false;
  std::chrono::milliseconds time = std::chrono::milliseconds(0);
};

class GeasVariable {
public:
  enum Type { BOOL_TYPE, FLOAT_TYPE, INT_TYPE };

protected:
  Type _t;  // Type of the variable
  union {
    geas::patom_t bv;
    geas::fp::fpvar fv;
    geas::intvar iv;
  };

public:
  explicit GeasVariable(const geas::patom_t& bv0) : _t(BOOL_TYPE), bv(bv0){};
  explicit GeasVariable(const geas::fp::fpvar& fv0) : _t(FLOAT_TYPE), fv(fv0){};
  explicit GeasVariable(const geas::intvar& iv0) : _t(INT_TYPE), iv(iv0){};

  GeasVariable(const GeasVariable& gv) : _t(gv._t) {
    switch (_t) {
      case BOOL_TYPE:
        bv = gv.bv;
        break;
      case FLOAT_TYPE:
        fv = gv.fv;
        break;
      case INT_TYPE:
        iv = gv.iv;
        break;
    }
  }

  bool isBool() const { return _t == BOOL_TYPE; }
  bool isFloat() const { return _t == FLOAT_TYPE; }
  bool isInt() const { return _t == INT_TYPE; }

  geas::patom_t boolVar() const { return bv; }
  geas::fp::fpvar floatVar() const { return fv; }
  geas::intvar intVar() const { return iv; }
};

class GeasTypes {
public:
  typedef GeasVariable Variable;
  typedef MiniZinc::Statistics Statistics;
};

class GeasSolverInstance : public SolverInstanceImpl<GeasTypes> {
public:
  GeasSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt);
  ~GeasSolverInstance() override = default;
  void processFlatZinc() override;
  geas::solver_data* solverData() const { return _solver.data; }
  geas::solver& solver() { return _solver; }

  Status solve() override;
  Status next() override { return SolverInstance::ERROR; }  // TODO: Implement
  void resetSolver() override;

  Expression* getSolutionValue(Id* id) override;
  void printStatistics() override;

  // MiniZinc to Geas conversions
  bool asBool(Expression* e) { return eval_bool(env().envi(), e); }
  vec<bool> asBool(ArrayLit* al);
  geas::patom_t asBoolVar(Expression* e);
  vec<geas::patom_t> asBoolVar(ArrayLit* al);
  vec<int> asInt(ArrayLit* al);
  int asInt(Expression* e) { return static_cast<int>(eval_int(env().envi(), e).toInt()); }
  geas::intvar asIntVar(Expression* e);
  vec<geas::intvar> asIntVar(ArrayLit* al);

  // TODO: create only when necessary or use Geas internal
  geas::intvar zero;

protected:
  geas::solver _solver;
  Model* _flat;

  SolveI::SolveType _objType = SolveI::ST_SAT;
  std::unique_ptr<GeasTypes::Variable> _objVar;

  GeasTypes::Variable& resolveVar(Expression* e);
  bool addSolutionNoGood();

  void registerConstraint(const std::string& name, poster p);
  void registerConstraints();
};

class GeasSolverFactory : public SolverFactory {
public:
  GeasSolverFactory();
  SolverInstanceBase::Options* createOptions() override;
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override;

  std::string getDescription(SolverInstanceBase::Options* opt) override {
    return "Elsie Geas - Another Lazy Clause Generation Solver";
  };
  std::string getVersion(SolverInstanceBase::Options* opt) override { return "0.0.1"; }
  std::string getId() override { return "org.minizinc.geas"; }

  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
  void printHelp(std::ostream& os) override;
};

}  // namespace MiniZinc
