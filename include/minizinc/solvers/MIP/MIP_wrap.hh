/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <utility>
#include <vector>
//#include <map>
#include <minizinc/solver_instance_defs.hh>

#include <cassert>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_map>

/// Facilitate lhs computation of a cut
inline double compute_sparse(int n, const int* ind, const double* coef, const double* dense,
                             int nVarsDense) {
  assert(ind && coef && dense);
  double val = 0.0;
  for (int i = 0; i < n; ++i) {
    assert(ind[i] >= 0);
    assert(ind[i] < nVarsDense);
    val += coef[i] * dense[ind[i]];
  }
  return val;
}

class MIPWrapper;

/// An abstract MIP wrapper.
/// Does not include MZN stuff so can be used independently
/// although it's limited to the MZN solving needs.
class MIPWrapper {
public:
  typedef int VarId;  // CPLEX uses int
  enum VarType { REAL, INT, BINARY };
  enum LinConType { LQ = -1, EQ = 0, GQ = 1 };

  // CPLEX 12.6.2 advises anti-symmetry constraints to be user+lazy
  static const int MaskConsType_Normal = 1;
  /// User cut. Only cuts off fractional points, no integer feasible points
  static const int MaskConsType_Usercut = 2;
  /// Lazy cut. Can cut off otherwise feasible integer solutions.
  /// Callback should be able to produce previously generated cuts again if needed [Gurobi]
  static const int MaskConsType_Lazy = 4;
  enum Status { OPT, SAT, UNSAT, UNBND, UNSATorUNBND, UNKNOWN, ERROR_STATUS };

  /// Search strategy for the solver
  enum SearchType { FIXED_SEARCH = 0, FREE_SEARCH = 1, UNIFORM_SEARCH = 2 };

  /// Columns for SCIP upfront and with obj coefs:
  std::vector<double> colObj, colLB, colUB;
  std::vector<VarType> colTypes;
  std::vector<std::string> colNames;
  //     , rowLB, rowUB, elements;
  //     veci whichInt
  //     , starts, column;
  //     double objUB;
  //     double qpu;

  /// Parameter
  bool fVerbose = false;

  int nProbType = -2;  // +-1: max/min; 0: sat

  struct Output {
    Status status;
    std::string statusName = "Untouched";
    double objVal = 1e308;
    double bestBound = 1e308;
    int nCols = 0;
    int nObjVarIndex = -1;
    const double* x = nullptr;
    int nNodes = 0;
    int nOpenNodes = 0;
    double dWallTime = 0.0;
    std::chrono::time_point<std::chrono::steady_clock> dWallTime0;
    double dCPUTime = 0;
    std::clock_t cCPUTime0 = 0;
  };
  Output output;

  /// General cut definition, could be used for addRow() too
  class CutDef {
    CutDef() {}

  public:
    CutDef(LinConType s, int m) : sense(s), mask(m) {}
    std::vector<int> rmatind;
    std::vector<double> rmatval;
    LinConType sense = LQ;
    double rhs = 0.0;
    int mask = 0;  // need to know what type of cuts are registered before solve()  TODO
    std::string rowName;
    void addVar(int i, double c) {
      rmatind.push_back(i);
      rmatval.push_back(c);
    }
    double computeViol(const double* x, int nCols) {
      double lhs = compute_sparse(static_cast<int>(rmatind.size()), rmatind.data(), rmatval.data(),
                                  x, nCols);
      if (LQ == sense) {
        return lhs - rhs;
      }
      if (GQ == sense) {
        return rhs - lhs;
      }
      assert(0);

      return 0.0;
    }
  };
  /// Cut callback fills one
  typedef std::vector<CutDef> CutInput;

  /// solution callback handler, the wrapper might not have these callbacks implemented
  typedef void (*SolCallbackFn)(const Output&, void*);
  /// cut callback handler, the wrapper might not have these callbacks implemented
  typedef void (*CutCallbackFn)(const Output&, CutInput&, void*,
                                bool fMIPSol  // if with a MIP feas sol - lazy cuts only
  );
  struct CBUserInfo {
    MIPWrapper* wrapper = nullptr;
    MIPWrapper::Output* pOutput = nullptr;
    MIPWrapper::Output* pCutOutput = nullptr;
    void* psi = nullptr;  // external info. Intended to keep MIPSolverinstance
    SolCallbackFn solcbfn = nullptr;
    CutCallbackFn cutcbfn = nullptr;
    /// Union of all flags used for the registered callback cuts
    /// See MaskConstrType_..
    /// Solvers need to know this
    /// In MIPSolverinstance, class CutGen defines getMask() which should return that
    int cutMask = 0;             // can be any combination of User/Lazy
    bool fVerb = false;          // used in Gurobi
    bool printed = false;        // whether any solution was output
    double nTimeoutFeas = -1.0;  // >=0 => stop that long after 1st feas
    double nTime1Feas = -1e100;  // time of the 1st feas
  };
  CBUserInfo cbui;

  MIPWrapper() { cbui.wrapper = this; }
  virtual ~MIPWrapper() { /* cleanup(); */
  }

  /// derived should overload and call the ancestor
  //     virtual void cleanup() {
  //       colObj.clear(); colLB.clear(); colUB.clear();
  //       colTypes.clear(); colNames.clear();
  //     }
  /// re-create solver object. Called from the base class constructor
  //     virtual void resetModel() { };

  //     virtual void printVersion(ostream& os) { os << "Abstract MIP wrapper"; }
  //     virtual void printHelp(ostream& ) { }

  bool fPhase1Over = false;

private:
  /// adding a variable just internally (in Phase 1 only that). Not to be used directly.
  virtual VarId addVarLocal(double obj, double lb, double ub, VarType vt,
                            const std::string& name = "") {
    //       cerr << "  addVarLocal: colObj.size() == " << colObj.size()
    //         << " obj == " <<obj
    //         << " lb == " << lb
    //         << " ub == " << ub
    //         << " vt == " << vt
    //         << " nm == " << name
    //         << endl;
    colObj.push_back(obj);
    colLB.push_back(lb);
    colUB.push_back(ub);
    colTypes.push_back(vt);
    colNames.push_back(name);
    return static_cast<VarId>(colObj.size() - 1);
  }
  /// add the given var to the solver. Asserts all previous are added. Phase >=2. No direct use
  virtual void addVar(int j) {
    assert(j == getNCols());
    assert(fPhase1Over);
    doAddVars(1, &colObj[j], &colLB[j], &colUB[j], &colTypes[j], &colNames[j]);
  }
  /// actual adding new variables to the solver. "Updates" the model (e.g., Gurobi). No direct use
  virtual void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                         std::string* names) = 0;

public:
  /// debugging stuff
  //     set<double> sLitValues;
  std::unordered_map<double, VarId> sLitValues;

  void setProbType(int t) { nProbType = t; }

  /// adding a variable, at once to the solver, this is for the 2nd phase
  virtual VarId addVar(double obj, double lb, double ub, VarType vt, const std::string& name = "") {
    //       cerr << "  AddVar: " << lb << ":   ";
    VarId res = addVarLocal(obj, lb, ub, vt, name);
    if (fPhase1Over) {
      addVar(res);
    }
    return res;
  }
  int nLitVars = 0;
  /// adding a literal as a variable. Should not happen in feasible models
  virtual VarId addLitVar(double v) {
    // Cannot do this: at least CBC does not support duplicated indexes    TODO??
    //       ++nLitVars;
    //       auto itFound = sLitValues.find(v);
    //       if (sLitValues.end() != itFound)
    //         return itFound->second;
    std::ostringstream oss;
    oss << "lit_" << v << "__" << (nLitVars++);
    std::string name = oss.str();
    size_t pos = name.find('.');
    if (std::string::npos != pos) {
      name.replace(pos, 1, "p");
    }
    VarId res = addVarLocal(0.0, v, v, REAL, name);
    if (fPhase1Over) {
      addVar(res);
    }
    //       cerr << "  AddLitVar " << v << "   (PROBABLY WRONG)" << endl;
    sLitValues[v] = res;
    return res;
  }
  /// adding all local variables upfront. Makes sure it's called only once
  virtual void addPhase1Vars() {
    assert(0 == getNColsModel());
    assert(!fPhase1Over);
    if (fVerbose) {
      std::cerr << "  MIPWrapper: adding the " << colObj.size() << " Phase-1 variables..."
                << std::flush;
    }
    if (!colObj.empty()) {
      doAddVars(colObj.size(), &colObj[0], &colLB[0], &colUB[0], &colTypes[0], &colNames[0]);
    }
    if (fVerbose) {
      std::cerr << " done." << std::endl;
    }
    fPhase1Over = true;  // SCIP needs after adding
  }

  /// var bounds
  virtual void setVarBounds(int iVar, double lb, double ub) { throw 0; }
  virtual void setVarLB(int iVar, double lb) { throw 0; }
  virtual void setVarUB(int iVar, double ub) { throw 0; }
  /// adding a linear constraint
  virtual void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
                      int mask = MaskConsType_Normal, const std::string& rowName = "") = 0;
  /// Indicator constraint: x[iBVar]==bVal -> lin constr
  virtual void addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind, double* rmatval,
                                      LinConType sense, double rhs,
                                      const std::string& rowName = "") {
    throw std::runtime_error("Indicator constraints not supported. ");
  }
  virtual void addMinimum(int iResultVar, int nnz, int* ind, const std::string& rowName = "") {
    throw std::runtime_error("This backend does not support the Minimum constraint");
  }

  /// Bounds disj for SCIP
  virtual void addBoundsDisj(int n, double* fUB, double* bnd, int* vars, int nF, double* fUBF,
                             double* bndF, int* varsF, const std::string& rowName = "") {
    throw std::runtime_error("Bounds disjunctions not supported. ");
  }
  /// Times constraint: var[x]*var[y] == var[z]
  virtual void addTimes(int x, int y, int z, const std::string& rowName = "") {
    throw std::runtime_error("Backend: [int/float]_times not supported. ");
  }

  /// Cumulative, currently SCIP only
  virtual void addCumulative(int nnz, int* rmatind, double* d, double* r, double b,
                             const std::string& rowName = "") {
    throw std::runtime_error("Cumulative constraints not supported. ");
  }

  /// Lex-lesseq binary, currently SCIP only
  virtual void addLexLesseq(int nnz, int* rmatind1, int* rmatind2, bool isModelCons,
                            const std::string& rowName = "") {
    throw std::runtime_error("MIP: lex_lesseq built-in not supported. ");
  }

  /// Lex-chain-lesseq binary, currently SCIP only
  virtual void addLexChainLesseq(int m, int n, int* rmatind, int nOrbitopeType, bool resolveprop,
                                 bool isModelCons, const std::string& rowName = "") {
    throw std::runtime_error("MIP: lex_chain_lesseq built-in not supported. ");
  }

  /// 0: model-defined level, 1: free, 2: uniform search
  virtual int getFreeSearch() { return SearchType::FREE_SEARCH; }
  /// Return 0 if ignoring searches
  virtual bool addSearch(const std::vector<VarId>& vars, const std::vector<int>& pri) {
    return false;
  }
  /// Return 0 if ignoring warm starts
  virtual bool addWarmStart(const std::vector<VarId>& vars, const std::vector<double>& vals) {
    return false;
  }

  using MultipleObjectives = MiniZinc::MultipleObjectivesTemplate<VarId>;
  virtual bool defineMultipleObjectives(const MultipleObjectives& mo) { return false; }

  int nAddedRows = 0;  // for name counting
  int nIndicatorConstr = 0;
  /// adding an implication
  //     virtual void addImpl() = 0;
  virtual void setObjSense(int s) = 0;  // +/-1 for max/min

  virtual double getInfBound() = 0;

  virtual int getNCols() = 0;
  virtual int getNColsModel() { return getNCols(); }  // from the solver
  virtual int getNRows() = 0;

  //     void setObjUB(double ub) { objUB = ub; }
  //     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

  /// Set solution callback. Thread-safety??
  /// solution callback handler, the wrapper might not have these callbacks implemented
  virtual void provideSolutionCallback(SolCallbackFn cbfn, void* info) {
    assert(cbfn);
    cbui.pOutput = &output;
    cbui.psi = info;
    cbui.solcbfn = cbfn;
  }
  /// solution callback handler, the wrapper might not have these callbacks implemented
  virtual void provideCutCallback(CutCallbackFn cbfn, void* info) {
    assert(cbfn);
    cbui.pCutOutput = nullptr;  // &outpCuts;   thread-safety: caller has to provide this
    cbui.psi = info;
    cbui.cutcbfn = cbfn;
  }

  virtual void solve() = 0;

  /// OUTPUT, should also work in a callback
  virtual const double* getValues() = 0;
  virtual double getObjValue() = 0;
  virtual double getBestBound() = 0;
  virtual double getWallTimeElapsed() { return output.dWallTime; }
  virtual double getCPUTime() = 0;

  virtual Status getStatus() = 0;
  virtual std::string getStatusName() = 0;

  virtual int getNNodes() = 0;
  virtual int getNOpen() = 0;

  /// Default MZN library for MIP
  static std::string getMznLib();
};
