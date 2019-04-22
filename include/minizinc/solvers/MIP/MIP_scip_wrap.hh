 
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_SCIP_WRAPPER_H__
#define __MIP_SCIP_WRAPPER_H__

#include <minizinc/solvers/MIP/MIP_wrap.hh>
#include <minizinc/solver_instance_base.hh>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

class MIP_scip_wrapper : public MIP_wrapper {
    SCIP         *scip = 0;
//     SCIP_Retcode           retcode = SCIP_OKAY;
//     char          scip_buffer[SCIP_MESSAGEBUFSIZE];
//     char          scip_status_buffer[SCIP_MESSAGEBUFSIZE];
    
    std::vector<SCIP_VAR*> scipVars;
    virtual SCIP_RETCODE delSCIPVars();
    
    std::vector<double> x;

  public:
    class Options : public MiniZinc::SolverInstanceBase::Options {
    public:
      int nThreads=1;
      std::string sExportModel;
      int nTimeout=-1;
      double nWorkMemLimit=-1;
      std::string sReadParams;
      std::string sWriteParams;
      bool flag_all_solutions = false;

      double absGap=-1;
      double relGap=1e-8;
      double intTol=1e-8;
      double objDiff=1.0;

      bool processOption(int& i, std::vector<std::string>& argv);
      static void printHelp(std::ostream& );
    };
  private:
    Options* options=nullptr;
  public:

    MIP_scip_wrapper(Options* opt) : options(opt) { wrap_assert( openSCIP() ); }
    virtual ~MIP_scip_wrapper() { wrap_assert( delSCIPVars() ); wrap_assert( closeSCIP() ); }
    
    static std::string getDescription(MiniZinc::SolverInstanceBase::Options* opt=NULL);
    static std::string getVersion(MiniZinc::SolverInstanceBase::Options* opt=NULL);
    static std::string getId(void);
    static std::string getName(void);
    static std::vector<std::string> getTags(void);
    static std::vector<std::string> getStdFlags(void);
    static std::string needDllFlag(void);

    bool processOption(int& i, int argc, const char** argv);
    void printVersion(std::ostream& );
    void printHelp(std::ostream& );
//       Statistics& getStatistics() { return _statistics; }

//      IloConstraintArray *userCuts, *lazyConstraints;

    /// derived should overload and call the ancestor
//     virtual void cleanup();
    SCIP_RETCODE openSCIP();
    SCIP_RETCODE closeSCIP();
    
    /// actual adding new variables to the solver
    virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, std::string *names) {
        wrap_assert(doAddVars_SCIP(n, obj, lb, ub, vt, names)); 
    }
    virtual SCIP_RETCODE doAddVars_SCIP(size_t n, double *obj, double *lb, double *ub,
      VarType *vt, std::string *names);
    virtual void setVarBounds( int iVar, double lb, double ub );
    virtual void setVarLB( int iVar, double lb );
    virtual void setVarUB( int iVar, double ub );

    /// adding a linear constraint
    virtual void addRow(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        std::string rowName = "") {
      wrap_assert(addRow_SCIP(nnz, rmatind, rmatval, sense, rhs, mask, rowName));
    }
    virtual SCIP_RETCODE addRow_SCIP(int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        int mask = MaskConsType_Normal,
                        std::string rowName = "");
    /// adding an implication
//     virtual void addImpl() = 0;
    /// Indicator constraint: x[iBVar]==bVal -> lin constr
    virtual void addIndicatorConstraint(int iBVar, int bVal, int nnz, int *rmatind, double* rmatval,
                        LinConType sense, double rhs,
                        std::string rowName = "");
    virtual void setObjSense(int s) {   // +/-1 for max/min
      wrap_assert( setObjSense_SCIP(s) );
    }
    virtual SCIP_RETCODE setObjSense_SCIP(int s);
    
    virtual double getInfBound() { return SCIPinfinity(scip); }
                        
    virtual int getNCols() { return SCIPgetNVars (scip); }
    virtual int getNRows() { return SCIPgetNConss (scip); }
                        
//     void setObjUB(double ub) { objUB = ub; }
//     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

    virtual void solve() { wrap_assert(solve_SCIP()); }
    virtual SCIP_RETCODE solve_SCIP();
    
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
    void wrap_assert(SCIP_RETCODE , std::string="" , bool fTerm=true);
    
    /// Need to consider the 100 status codes in SCIP and change with every version? TODO
    Status convertStatus(SCIP_STATUS scipStatus);
};

#endif  // __MIP_SCIP_WRAPPER_H__
