
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>
// CMakeLists.txt needs OSICBC_HOME defined
// #include <coin/CoinPackedVector.hpp>
// #include <coin/CoinPackedMatrix.hpp>
// #include <coin/CoinShallowPackedVector.hpp>
// #include <coin/CoinTime.hpp>
// #include <coin/OsiSolverInterface.hpp>
//  #include <coin/OsiCbcSolverInterface.hpp>
#include <coin/CbcModel.hpp>
#include <coin/OsiClpSolverInterface.hpp>
// #include <coin/CbcSolver.hpp>

class MIPosicbcWrapper : public MIPWrapper {
  //     OsiCbcSolverInterface osi;   // deprecated in Cbc 2.9.6
  OsiClpSolverInterface _osi;
  //     CoinPackedMatrix* matrix = 0;
  int _error;
  std::string _osicbcBuffer;  // [CBC_MESSAGEBUFSIZE];
                              //     string          osicbc_status_buffer; // [CBC_MESSAGEBUFSIZE];

  std::vector<double> _x;

  // To add constraints:
  //     vector<int> rowStarts, columns;
  std::vector<CoinPackedVector> _rows;
  std::vector<double>  // element,
      _rowlb, _rowub;

  std::unordered_map<VarId, double> _warmstart;  // this accumulates warmstart infos

public:
  class FactoryOptions {
  public:
    // NOLINTNEXTLINE(readability-convert-member-functions-to-static)
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir) {
      return false;
    }
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int nThreads = 1;
    std::string sExportModel;
    int nTimeout = 0;
    long int nSolLimit = -1;
    double nWorkMemLimit = -1;
    std::string sReadParams;
    std::string sWriteParams;
    bool flagIntermediate = false;

    double absGap = -1;
    double relGap = 1e-8;
    double intTol = 1e-8;
    double objDiff = 1.0;

    std::vector<std::string> cbcCmdOptions;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv,
                       const std::string& workingDir = std::string());
    static void printHelp(std::ostream& os);
  };

private:
  Options* _options = nullptr;

public:
  MIPosicbcWrapper(FactoryOptions& factoryOpt, Options* opt) : _options(opt) { openOSICBC(); }
  ~MIPosicbcWrapper() override { closeOSICBC(); }

  static std::string getDescription(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getVersion(FactoryOptions& factoryOpt,
                                MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getId();
  static std::string getName();
  static std::vector<std::string> getTags();
  static std::vector<std::string> getStdFlags();
  static std::vector<std::string> getRequiredFlags(FactoryOptions& factoryOpt) { return {}; };
  static std::vector<std::string> getFactoryFlags() { return {}; };

  static std::vector<MiniZinc::SolverConfig::ExtraFlag> getExtraFlags(FactoryOptions& factoryOpt);

  void printVersion(std::ostream&);
  void printHelp(std::ostream&);
  //       Statistics& getStatistics() { return _statistics; }

  //      IloConstraintArray *userCuts, *lazyConstraints;

  /// derived should overload and call the ancestor
  //     virtual void cleanup();
  void openOSICBC() {}
  void closeOSICBC() {}

  /// actual adding new variables to the solver
  void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                 std::string* names) override;

  void addPhase1Vars() override {
    if (fVerbose) {
      std::cerr << "  MIPosicbcWrapper: delaying physical addition of variables..." << std::endl;
    }
  }

  /// adding a linear constraint
  void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
              int mask = MaskConsType_Normal, const std::string& rowName = "") override;
  /// adding an implication
  //     virtual void addImpl() = 0;

  bool addWarmStart(const std::vector<VarId>& vars, const std::vector<double>& vals) override;

  void setObjSense(int s) override;  // +/-1 for max/min

  double getInfBound() override { return _osi.getInfinity(); }

  int getNCols() override {
    int nc = _osi.getNumCols();
    return nc != 0 ? nc : static_cast<int>(colLB.size());
  }
  int getNColsModel() override { return _osi.getNumCols(); }
  int getNRows() override {
    if (!_rowlb.empty()) {
      return _rowlb.size();
    }
    return _osi.getNumRows();
  }

  //     void setObjUB(double ub) { objUB = ub; }
  //     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

  void solve() override;

  /// OUTPUT:
  const double* getValues() override { return output.x; }
  double getObjValue() override { return output.objVal; }
  double getBestBound() override { return output.bestBound; }
  double getCPUTime() override { return output.dCPUTime; }

  Status getStatus() override { return output.status; }
  std::string getStatusName() override { return output.statusName; }

  int getNNodes() override { return output.nNodes; }
  int getNOpen() override { return output.nOpenNodes; }

  //     virtual int getNNodes() = 0;
  //     virtual double getTime() = 0;

protected:
  //     OsiSolverInterface& getOsiSolver() { return osi; }

  void wrapAssert(bool cond, const std::string& msg, bool fTerm = true);

  /// Need to consider the 100 status codes in OSICBC and change with every version? TODO
  Status convertStatus(CbcModel* pModel);
  Status convertStatus();
};
