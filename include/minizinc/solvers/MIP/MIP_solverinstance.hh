/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/flattener.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

namespace MiniZinc {

// can be redefined as compilation parameter
#ifndef GETMIPWRAPPER
#define GETMIPWRAPPER MIP_WrapperFactory::GetDefaultMIPWrapper()
#endif

class MIPSolver {
public:
  typedef MIPWrapper::VarId Variable;
  typedef MiniZinc::Statistics Statistics;
};

/// Generic cut generator
/// Callback should be able to produce previously generated cuts again if needed [Gurobi]
class CutGen {
public:
  virtual ~CutGen() {}
  /// Say what type of cuts
  virtual int getMask() = 0;
  /// Adds new cuts to the 2nd parameter
  virtual void generate(const MIPWrapper::Output&, MIPWrapper::CutInput&) = 0;
  virtual void print(std::ostream& /*os*/) {}
};

/// XBZ cut generator
class XBZCutGen : public CutGen {
  XBZCutGen() {}
  MIPWrapper* _pMIP = nullptr;

public:
  XBZCutGen(MIPWrapper* pw) : _pMIP(pw) {}
  std::vector<MIPWrapper::VarId> varX, varB;
  /// Say what type of cuts
  int getMask() override { return MIPWrapper::MaskConsType_Usercut; }
  MIPWrapper::VarId varZ;
  void generate(const MIPWrapper::Output& slvOut, MIPWrapper::CutInput& cutsIn) override;
  void print(std::ostream& os) override;
};

/// SEC cut generator for circuit
class SECCutGen : public CutGen {
  SECCutGen() {}
  MIPWrapper* _pMIP = nullptr;

public:
  SECCutGen(MIPWrapper* pw) : _pMIP(pw) {}
  /// Say what type of cuts
  int getMask() override {
    return MIPWrapper::MaskConsType_Lazy | MIPWrapper::MaskConsType_Usercut;
  }
  std::vector<MIPWrapper::VarId> varXij;
  int nN = 0;  // N nodes
  /// returns error message if fails
  std::string validate() const;
  void generate(const MIPWrapper::Output& slvOut, MIPWrapper::CutInput& cutsIn) override;
  void print(std::ostream& os) override;
};

template <class MIPWrapper>
class MIPSolverinstance : public SolverInstanceImpl<MIPSolver> {
  using SolverInstanceBase::_log;

protected:
  const std::unique_ptr<MIPWrapper> _mipWrapper;
  std::vector<std::unique_ptr<CutGen> > _cutGenerators;

public:
  void registerCutGenerator(std::unique_ptr<CutGen>&& pCG) {
    getMIPWrapper()->cbui.cutMask |= pCG->getMask();
    _cutGenerators.push_back(move(pCG));
  }

  double lastIncumbent;
  double dObjVarLB = -1e300, dObjVarUB = 1e300;

  MIPSolverinstance(Env& env, std::ostream& log, typename MIPWrapper::FactoryOptions& factoryOpt,
                    typename MIPWrapper::Options* opt)
      : SolverInstanceImpl(env, log, opt), _mipWrapper(new MIPWrapper(factoryOpt, opt)) {
    assert(_mipWrapper.get());
    registerConstraints();
  }
  virtual MIPWrapper* getMIPWrapper() const { return _mipWrapper.get(); }

  Status next() override {
    assert(0);
    return SolverInstance::UNKNOWN;
  }
  void processFlatZinc() override;
  virtual void processWarmstartAnnotations(const Annotation& ann);
  virtual void processSearchAnnotations(const Annotation& ann);
  virtual void processMultipleObjectives(const Annotation& ann);
  Status solve() override;
  void resetSolver() override {}

  virtual void genCuts(const typename MIPWrapper::Output& slvOut,
                       typename MIPWrapper::CutInput& cutsIn, bool fMIPSol);

  //       void assignSolutionToOutput();   // needs to be public for the callback?
  void printStatistics() override;
  void printStatisticsLine(bool fLegend = false) override;

  /// creates a var for a literal, if necessary
  VarId exprToVar(Expression* arg);
  void exprToArray(Expression* arg, std::vector<double>& vals);
  void exprToVarArray(Expression* arg, std::vector<VarId>& vars);
  std::pair<double, bool> exprToConstEasy(Expression* e);
  double exprToConst(Expression* e);

  Expression* getSolutionValue(Id* id) override;

  void registerConstraints();
};  // MIPSolverinstance

template <class MIPWrapper>
class MIPSolverFactory : public SolverFactory {
public:
  MIPSolverFactory();
  bool processFactoryOption(int& i, std::vector<std::string>& argv,
                            const std::string& workingDir = std::string()) override;
  void factoryOptionsFinished() override;
  SolverInstanceBase::Options* createOptions() override { return new typename MIPWrapper::Options; }
  SolverInstanceBase* doCreateSI(Env& env, std::ostream& log,
                                 SolverInstanceBase::Options* opt) override {
    return new MIPSolverinstance<MIPWrapper>(env, log, _factoryOptions,
                                             static_cast<typename MIPWrapper::Options*>(opt));
  }
  bool processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv,
                     const std::string& workingDir = std::string()) override;
  std::string getDescription(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getVersion(SolverInstanceBase::Options* opt = nullptr) override;
  std::string getId() override;
  void printHelp(std::ostream& os) override { MIPWrapper::Options::printHelp(os); }

private:
  typename MIPWrapper::FactoryOptions _factoryOptions;
  std::vector<SolverConfig::ExtraFlag> _extraFlags;
};

}  // namespace MiniZinc

#include <minizinc/solvers/MIP/MIP_solverinstance.hpp>
