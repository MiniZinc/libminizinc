 
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_CPLEX_WRAPPER_H__
#define __MIP_CPLEX_WRAPPER_H__

#include <minizinc/solvers/MIP/MIP_wrap.hh>
#include <ilcplex/cplex.h>     // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake

class MIP_cplex_wrapper : public MIP_wrapper {
    CPXENVptr     env = 0;
    CPXLPptr      lp = 0;
    int           status;
    char          cplex_buffer[CPXMESSAGEBUFSIZE];
    char          cplex_status_buffer[CPXMESSAGEBUFSIZE];
    
    std::vector<double> x;

  public:
    MIP_cplex_wrapper() { openCPLEX(); }
    virtual ~MIP_cplex_wrapper() { closeCPLEX(); }
    
    bool processOption(int& i, int argc, const char** argv);
    void printVersion(std::ostream& );
    void printHelp(std::ostream& );
//       Statistics& getStatistics() { return _statistics; }

//      IloConstraintArray *userCuts, *lazyConstraints;

    /// derived should overload and call the ancestor
//     virtual void cleanup();
    void openCPLEX();
    void closeCPLEX();
    
    /// actual adding new variables to the solver
    virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, std::string *names);

    /// adding a linear constraint
    virtual void addRow(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        std::string rowName = "");
    virtual void setVarBounds( int iVar, double lb, double ub );
    virtual void setVarLB( int iVar, double lb );
    virtual void setVarUB( int iVar, double ub );
    /// Indicator constraint: x[iBVar]==bVal -> lin constr
    virtual void addIndicatorConstraint(int iBVar, int bVal, int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        std::string rowName = "");
    /// adding an implication
//     virtual void addImpl() = 0;
    virtual void setObjSense(int s);   // +/-1 for max/min
    
    virtual double getInfBound() { return CPX_INFBOUND; }
                        
    virtual int getNCols() { return CPXgetnumcols (env, lp); }
    virtual int getNRows() { return CPXgetnumrows (env, lp); }
                        
//     void setObjUB(double ub) { objUB = ub; }
//     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

    virtual void solve(); 
    
    /// OUTPUT:
    virtual const double* getValues() { return output.x; }
    virtual double getObjValue() { return output.objVal; }
    virtual double getBestBound() { return output.bestBound; }
    virtual double getCPUTime() { return output.dCPUTime; }
    
    virtual Status getStatus()  { return output.status; }
    virtual std::string getStatusName() { return output.statusName; }

     virtual int getNNodes() { return output.nNodes; }
     virtual int getNOpen() { return output.nOpenNodes; }

//     virtual int getNNodes() = 0;
//     virtual double getTime() = 0;
    
  protected:
    void wrap_assert(bool , std::string , bool fTerm=true);
    
    /// Need to consider the 100 status codes in CPLEX and change with every version? TODO
    Status convertStatus(int cplexStatus);
};

#endif  // __MIP_CPLEX_WRAPPER_H__
