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
#include <string>
#include <iostream>
#include <cassert>

using namespace std;

class MIP_wrapper;
/// Namespace MIP_WrapperFactory providing static service functions
/// The implementation of a MIP wrapper should define these
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
    enum Status { OPT, SAT, UNSAT, UNBND, UNKNOWN, ERROR };
  private:
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
      const char* statusName="Untouched";
      double objVal;
      double bestBound;
      double *x = 0;
      int nNodes;
      int nOpenNodes;
      double dCPUTime;
    };      
  protected:
    Output output;

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

    /// adding a variable just internally, used in processFlatzinc()
    virtual VarId addVarLocal(double obj, double lb, double ub, 
                             VarType vt, string name="") {
//       cerr << "  addVarLocal: colObj.size() == " << colObj.size() << endl;
      colObj.push_back(obj);
      colLB.push_back(lb);
      colUB.push_back(ub);
      colTypes.push_back(vt);
      colNames.push_back(name);
      return colObj.size()-1;
    }
    /// adding a a variable, at once to the solver
    virtual VarId addVar(double obj, double lb, double ub, 
                             VarType vt, string name=0) {
      VarId res = addVarLocal(obj, lb, ub, vt, name);
      addVar(res);
//       cerr << "  AddVar: " << lb << endl;
      return res;
    }
    /// adding a literal as a variable, at once to the solver
    virtual VarId addLitVar(double v, string name="lit") {
      VarId res = addVarLocal(0.0, v, v, REAL, name);
      addVar(res);
//       cerr << "  AddLitVar " << v << endl;
      return res;
    }
    /// adding all local variables upfront. Makes sure it's called only once
    virtual void addVars() {
      assert(0 == getNCols());
      doAddVars(colObj.size(), &colObj[0], &colLB[0], &colUB[0], &colTypes[0], &colNames[0]);
    }
    /// add the given var and asserts all previous are added
    virtual void addVar(int j) {
      assert(j == getNCols());
      doAddVars(1, &colObj[j], &colLB[j], &colUB[j], &colTypes[j], &colNames[j]);
    }
    /// actual adding new variables to the solver
    virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, string *names) = 0;

    /// adding a linear constraint
    virtual void addRow(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        string rowName = "") = 0;
    /// adding an implication
//     virtual void addImpl() = 0;
    virtual void setObjSense(int s) = 0;   // +/-1 for max/min
    
    virtual double getInfBound() = 0;
                        
    virtual size_t getNCols() = 0;
    virtual size_t getNRows() = 0;
                        
//     void setObjUB(double ub) { objUB = ub; }
//     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

    /// solution callback handler, the wrapper might not have these callbacks implemented
    typedef void (*SolCallbackFn)(const Output& , void* );
    virtual void provideSolutionCallback(SolCallbackFn cbfn, void* info) = 0;
    virtual void solve() = 0; 
    
    /// OUTPUT, should also work in a callback
    virtual double* getValues() = 0;
    virtual double getObjValue() = 0;
    virtual double getBestBound() = 0;
    virtual double getCPUTime() = 0;
    
    virtual Status getStatus() = 0;
    virtual const char* getStatusName() = 0;

     virtual int getNNodes() = 0;
     virtual int getNOpen() = 0;

  }; 

#endif  // __MIP_WRAPPER__