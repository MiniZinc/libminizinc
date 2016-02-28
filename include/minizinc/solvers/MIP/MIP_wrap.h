/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_WRAPPER__
#define __MIP_WRAPPER__

#include <vector>
//#include <map>
#include <unordered_map>
#include <string>
#include <iostream>
#include <sstream>
#include <cassert>

using namespace std;

class MIP_wrapper;
/// Namespace MIP_WrapperFactory providing static service functions
/// The implementation of a MIP wrapper should define these
/// !!! Assuming it's always 1 MIP solver in a static module
namespace MIP_WrapperFactory {
    /// static functions creating a MIP wrapper, defined in a .cpp
    MIP_wrapper* GetDefaultMIPWrapper();
//     MIP_wrapper* GetDefaultMILPWrapper();
//     MIP_wrapper* GetDefaultMIQPWrapper();
//     MIP_wrapper* GetCplexMILPWrapper();
//     Wrap_MIP* GetCplexMIQP();

    bool processOption(int& i, int argc, const char** argv);
    string getVersion( );
    void printHelp(std::ostream& );
};

/// An abstract MIP wrapper.
/// Does not include MZN stuff so can be used independently
/// although it's limited to the MZN solving needs.
class MIP_wrapper {
  public:
    typedef int VarId;    // CPLEX uses int
    enum VarType {
      REAL,
      INT,
      BINARY
    };
    enum LinConType {
      LQ = -1,
      EQ = 0,
      GQ = 1
    };
    
    static const int MaskConsType_Normal = 1;
    static const int MaskConsType_Usercut = 2;
    static const int MaskConsType_Lazy = 4;
    enum Status { OPT, SAT, UNSAT, UNBND, UNSATorUNBND, UNKNOWN, ERROR };
  protected:
    /// Columns for SCIP upfront and with obj coefs:
    vector<double> colObj, colLB, colUB;
    vector<VarType> colTypes;
    vector<string> colNames;
//     , rowLB, rowUB, elements;
//     veci whichInt
//     , starts, column;
//     double objUB;
//     double qpu;
    
  public:
    /// Parameter
    bool fVerbose = false;

  public:
    struct Output {
      Status status;
      string statusName="Untouched";
      double objVal = 1e308;
      double bestBound = 1e308;
      int nCols;
      int nObjVarIndex=-1;
      const double *x = 0;
      int nNodes=0;
      int nOpenNodes=0;
      double dCPUTime = 0;
    };      
    Output output;

  public:
    /// solution callback handler, the wrapper might not have these callbacks implemented
    typedef void (*SolCallbackFn)(const Output& , void* );
    struct CBUserInfo {
      MIP_wrapper::Output* pOutput=0;
      bool fVerb = false;              // used in Gurobi
      void *ppp=0;  // external info
      SolCallbackFn solcbfn=0;
    };
  protected:
    CBUserInfo cbui;

  public:
//     MIP_wrapper() { /*resetModel();*/ }
    virtual ~MIP_wrapper() { /* cleanup(); */ }

    /// derived should overload and call the ancestor
//     virtual void cleanup() {
//       colObj.clear(); colLB.clear(); colUB.clear();
//       colTypes.clear(); colNames.clear();
//     }
    /// re-create solver object. Called from the base class constructor
//     virtual void resetModel() { };
    
//     virtual void printVersion(ostream& os) { os << "Abstract MIP wrapper"; }
//     virtual void printHelp(ostream& ) { }

  public:
    bool fPhase1Over = false;
  private:
    /// adding a variable just internally (in Phase 1 only that). Not to be used directly.
    virtual VarId addVarLocal(double obj, double lb, double ub, 
                             VarType vt, string name="") {
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
      return colObj.size()-1;
    }
    /// add the given var to the solver. Asserts all previous are added. Phase >=2. No direct use
    virtual void addVar(int j) {
      assert(j == getNCols());
      assert(fPhase1Over);
      doAddVars(1, &colObj[j], &colLB[j], &colUB[j], &colTypes[j], &colNames[j]);
    }
    /// actual adding new variables to the solver. "Updates" the model (e.g., Gurobi). No direct use
    virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, string *names) = 0;

  public:
    /// debugging stuff
//     set<double> sLitValues;
    std::unordered_map<double, VarId> sLitValues;
    
    /// adding a variable, at once to the solver, this is for the 2nd phase
    virtual VarId addVar(double obj, double lb, double ub, 
                             VarType vt, string name=0) {
//       cerr << "  AddVar: " << lb << ":   ";
      VarId res = addVarLocal(obj, lb, ub, vt, name);
      if (fPhase1Over)
        addVar(res);
      return res;
    }
    int nLitVars=0;
    /// adding a literal as a variable. Should not happen in feasible models
    virtual VarId addLitVar(double v) {
      // Cannot do this: at least CBC does not support duplicated indexes    TODO??
//       ++nLitVars;
//       auto itFound = sLitValues.find(v);
//       if (sLitValues.end() != itFound)
//         return itFound->second;
      ostringstream oss;
      oss << "lit_" << v << "__" << (nLitVars++);
      string name = oss.str();
      size_t pos = name.find('.');
      if (string::npos != pos)
        name.replace(pos, 1, "p");
      VarId res = addVarLocal(0.0, v, v, REAL, name);
      if (fPhase1Over)
        addVar(res);
//       cerr << "  AddLitVar " << v << "   (PROBABLY WRONG)" << endl;
      sLitValues[v] = res;
      return res;
    }
    /// adding all local variables upfront. Makes sure it's called only once
    virtual void addPhase1Vars() {
      assert(0 == getNColsModel());
      assert(! fPhase1Over);
      if (fVerbose)
        cerr << "  MIP_wrapper: adding the " << colObj.size() << " Phase-1 variables..." << flush;
      doAddVars(colObj.size(), &colObj[0], &colLB[0], &colUB[0], &colTypes[0], &colNames[0]);
      if (fVerbose)
        cerr << " done." << endl;
      fPhase1Over = true;    // SCIP needs after adding
    }

    /// adding a linear constraint
    virtual void addRow(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        string rowName = "") = 0;
    int nAddedRows = 0;   // for name counting
    /// adding an implication
//     virtual void addImpl() = 0;
    virtual void setObjSense(int s) = 0;   // +/-1 for max/min
    
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
      cbui.ppp = info;
      cbui.solcbfn = cbfn;
    }
    virtual void solve() = 0; 
    
    /// OUTPUT, should also work in a callback
    virtual const double* getValues() = 0;
    virtual double getObjValue() = 0;
    virtual double getBestBound() = 0;
    virtual double getCPUTime() = 0;
    
    virtual Status getStatus() = 0;
    virtual string getStatusName() = 0;

     virtual int getNNodes() = 0;
     virtual int getNOpen() = 0;

  }; 

#endif  // __MIP_WRAPPER__