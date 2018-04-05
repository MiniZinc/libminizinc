 
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

#include <minizinc/config.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>
#include <minizinc/solver_instance_base.hh>
#include <ilcplex/cplex.h>     // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake

class MIP_cplex_wrapper : public MIP_wrapper {
    CPXENVptr     env = 0;
    CPXLPptr      lp = 0;
  
    int           status;
    char          cplex_buffer[CPXMESSAGEBUFSIZE];
    char          cplex_status_buffer[CPXMESSAGEBUFSIZE];
    
    std::vector<double> x;

#ifdef HAS_CPLEX_PLUGIN
      void*         _cplex_dll;
#endif

  public:

    class Options : public MiniZinc::SolverInstanceBase::Options {
    public:
      int nMIPFocus=0;
      int nThreads=1;
      std::string sExportModel;
      double nTimeout=-1;
      long int nSolLimit = -1;
      double nWorkMemLimit=-1;
      std::string sReadParams;
      std::string sWriteParams;
      bool flag_all_solutions = false;
      
      double absGap=0.99;
      double relGap=1e-8;
      double intTol=1e-6;
      double objDiff=1.0;
      std::string sCPLEXDLL;
      bool processOption(int& i, int argc, const char** argv);
      static void printHelp(std::ostream& );
    };
  private:
    Options* options;
  public:

  MIP_cplex_wrapper(Options* opt) : options(opt) { openCPLEX(); }
    virtual ~MIP_cplex_wrapper() { closeCPLEX(); }

    static std::string getDescription(void);
    static std::string getVersion(void);
    static std::string getId(void);
    static std::string getName(void);

//       Statistics& getStatistics() { return _statistics; }

//      IloConstraintArray *userCuts, *lazyConstraints;

    /// derived should overload and call the ancestor
//     virtual void cleanup();
    void checkDLL();
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
    virtual bool addWarmStart( const std::vector<VarId>& vars, const std::vector<double> vals );
    /// adding an implication
//     virtual void addImpl() = 0;
    virtual void setObjSense(int s);   // +/-1 for max/min
    
    virtual double getInfBound() { return CPX_INFBOUND; }
                        
    virtual int getNCols() { return dll_CPXgetnumcols (env, lp); }
    virtual int getNRows() { return dll_CPXgetnumrows (env, lp); }
                        
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
  
  // CPLEX API
  
  int (*dll_CPXaddfuncdest) (CPXCENVptr, CPXCHANNELptr, void *, void(*msgfunction)(void *, const char *));
  int (*dll_CPXaddindconstr) (CPXCENVptr, CPXLPptr, int,
                   int, int, double, int,
                   int const *, double const *,
                   char const *);
  int (*dll_CPXaddlazyconstraints) (CPXCENVptr env, CPXLPptr lp, int rcnt,
                         int nzcnt, double const *rhs,
                         char const *sense, int const *rmatbeg,
                         int const *rmatind, double const *rmatval,
                         char **rowname);
  int (*dll_CPXaddmipstarts) (CPXCENVptr env, CPXLPptr lp, int mcnt, int nzcnt,
                   int const *beg, int const *varindices,
                   double const *values, int const *effortlevel,
                   char **mipstartname);
  int (*dll_CPXaddrows) (CPXCENVptr env, CPXLPptr lp, int ccnt, int rcnt,
              int nzcnt, double const *rhs, char const *sense,
              int const *rmatbeg, int const *rmatind,
              double const *rmatval, char **colname, char **rowname);
  int (*dll_CPXaddusercuts) (CPXCENVptr env, CPXLPptr lp, int rcnt, int nzcnt,
                  double const *rhs, char const *sense,
                  int const *rmatbeg, int const *rmatind,
                  double const *rmatval, char **rowname);
  int (*dll_CPXchgbds) (CPXCENVptr env, CPXLPptr lp, int cnt, int const *indices,
             char const *lu, double const *bd);
  int (*dll_CPXchgmipstarts) (CPXCENVptr env, CPXLPptr lp, int mcnt,
                   int const *mipstartindices, int nzcnt,
                   int   const *beg, int const *varindices,
                   double const *values, int const *effortlevel);
  int (*dll_CPXchgobjsen) (CPXCENVptr env, CPXLPptr lp, int maxormin);

  int (*dll_CPXcloseCPLEX) (CPXENVptr *env_p);

  CPXLPptr (*dll_CPXcreateprob) (CPXCENVptr env, int *status_p,
                 char const *probname_str);

  int (*dll_CPXcutcallbackadd) (CPXCENVptr env, void *cbdata, int wherefrom,
                     int nzcnt, double rhs, int sense,
                     int const *cutind, double const *cutval,
                     int purgeable);

  int (*dll_CPXfreeprob) (CPXCENVptr env, CPXLPptr *lp_p);
  int (*dll_CPXgetbestobjval) (CPXCENVptr env, CPXCLPptr lp, double *objval_p);
  int (*dll_CPXgetcallbackincumbent) (CPXCENVptr env, void *cbdata,
                           int wherefrom, double *x, int begin,
                           int end);
  int (*dll_CPXgetcallbackinfo) (CPXCENVptr env, void *cbdata, int wherefrom,
                      int whichinfo, void *result_p);
  int (*dll_CPXgetcallbacknodeinfo) (CPXCENVptr env, void *cbdata, int wherefrom,
                          int nodeindex, int whichinfo,
                          void *result_p);
  int (*dll_CPXgetcallbacknodex) (CPXCENVptr env, void *cbdata, int wherefrom,
                       double *x, int begin, int end);
  int (*dll_CPXgetchannels) (CPXCENVptr env, CPXCHANNELptr *cpxresults_p,
                  CPXCHANNELptr *cpxwarning_p,
                  CPXCHANNELptr *cpxerror_p, CPXCHANNELptr *cpxlog_p);
  int (*dll_CPXgetdettime) (CPXCENVptr env, double *dettimestamp_p);
  CPXCCHARptr (*dll_CPXgeterrorstring) (CPXCENVptr env, int errcode, char *buffer_str);
  int (*dll_CPXgetmipstartindex) (CPXCENVptr env, CPXCLPptr lp,
                       char const *lname_str, int *index_p);
  int (*dll_CPXgetnodecnt) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetnodeleftcnt) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetnumcols) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetnumrows) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetobjsen) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetobjval) (CPXCENVptr env, CPXCLPptr lp, double *objval_p);
  int (*dll_CPXgetsolnpoolnumsolns) (CPXCENVptr env, CPXCLPptr lp);
  int (*dll_CPXgetstat) (CPXCENVptr env, CPXCLPptr lp);
  CPXCHARptr (*dll_CPXgetstatstring) (CPXCENVptr env, int statind, char *buffer_str);
  int (*dll_CPXgettime) (CPXCENVptr env, double *timestamp_p);
  int (*dll_CPXgetx) (CPXCENVptr env, CPXCLPptr lp, double *x, int begin,
           int end);
  int (*dll_CPXmipopt) (CPXCENVptr env, CPXLPptr lp);
  int (*dll_CPXnewcols) (CPXCENVptr env, CPXLPptr lp, int ccnt,
              double const *obj, double const *lb, double const *ub,
              char const *xctype, char **colname);
  CPXENVptr (*dll_CPXopenCPLEX) (int *status_p);
  int (*dll_CPXreadcopyparam) (CPXENVptr env, char const *filename_str);
  int (*dll_CPXsetdblparam) (CPXENVptr env, int whichparam, double newvalue);
  int (*dll_CPXsetinfocallbackfunc) (CPXENVptr env,
                          int(*callback)(CPXCENVptr, void *, int, void *),
                          void *cbhandle);
  int (*dll_CPXsetintparam) (CPXENVptr env, int whichparam, CPXINT newvalue);
  int (*dll_CPXsetlazyconstraintcallbackfunc) (CPXENVptr env,
                                    int(*lazyconcallback)(CALLBACK_CUT_ARGS),
                                    void *cbhandle);
  int (*dll_CPXsetusercutcallbackfunc) (CPXENVptr env,
                             int(*cutcallback)(CALLBACK_CUT_ARGS),
                             void *cbhandle);
  CPXCCHARptr (*dll_CPXversion) (CPXCENVptr env);
  int (*dll_CPXwriteparam) (CPXCENVptr env, char const *filename_str);
  
  int (*dll_CPXwriteprob) (CPXCENVptr env, CPXCLPptr lp,
                char const *filename_str, char const *filetype_str);
  
  protected:
    void wrap_assert(bool , std::string , bool fTerm=true);
    
    /// Need to consider the 100 status codes in CPLEX and change with every version? TODO
    Status convertStatus(int cplexStatus);
};

#endif  // __MIP_CPLEX_WRAPPER_H__
