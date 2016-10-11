 
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_GUROBI_WRAPPER_H__
#define __MIP_GUROBI_WRAPPER_H__

#include <minizinc/solvers/MIP/MIP_wrap.hh>
extern "C" {
  #include <gurobi_c.h>     // need GUROBI_HOME defined
}

class MIP_gurobi_wrapper : public MIP_wrapper {
    GRBenv        * env = 0;
    GRBmodel      * model = 0;
    int             error;
    string          gurobi_buffer;   // [GRB_MESSAGEBUFSIZE];
    string          gurobi_status_buffer; // [GRB_MESSAGEBUFSIZE];
    
    vector<double> x;

  public:
    MIP_gurobi_wrapper() { openGUROBI(); }
    virtual ~MIP_gurobi_wrapper() { closeGUROBI(); }
    
    bool processOption(int& i, int argc, const char** argv);
    void printVersion(ostream& );
    void printHelp(ostream& );
//       Statistics& getStatistics() { return _statistics; }

//      IloConstraintArray *userCuts, *lazyConstraints;

    /// derived should overload and call the ancestor
//     virtual void cleanup();
    void openGUROBI();
    void closeGUROBI();
    
    /// actual adding new variables to the solver
    virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, string *names);

    /// adding a linear constraint
    virtual void addRow(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        string rowName = "");
    int nRows=0;    // to count rows in order tp notice lazy constraints
    std::vector<int> nLazyIdx;
    std::vector<int> nLazyValue;
    /// adding an implication
//     virtual void addImpl() = 0;
    virtual void setObjSense(int s);   // +/-1 for max/min
    
    virtual double getInfBound() { return GRB_INFINITY; }
                        
    virtual int getNCols() {
      GRBupdatemodel(model);
      int cols; error = GRBgetintattr(model, GRB_INT_ATTR_NUMVARS, &cols); return cols;
    }
    virtual int getNRows() {
      GRBupdatemodel(model);
      int cols; error = GRBgetintattr(model, GRB_INT_ATTR_NUMCONSTRS, &cols); return cols;
    }
                        
//     void setObjUB(double ub) { objUB = ub; }
//     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

    virtual void solve(); 
    
    /// OUTPUT:
    virtual const double* getValues() { return output.x; }
    virtual double getObjValue() { return output.objVal; }
    virtual double getBestBound() { return output.bestBound; }
    virtual double getCPUTime() { return output.dCPUTime; }
    
    virtual Status getStatus()  { return output.status; }
    virtual string getStatusName() { return output.statusName; }

     virtual int getNNodes() { return output.nNodes; }
     virtual int getNOpen() { return output.nOpenNodes; }

//     virtual int getNNodes() = 0;
//     virtual double getTime() = 0;
    
  protected:
    void wrap_assert(bool , string , bool fTerm=true);
    
    /// Need to consider the 100 status codes in GUROBI and change with every version? TODO
    Status convertStatus(int gurobiStatus);
};

#endif  // __MIP_GUROBI_WRAPPER_H__
