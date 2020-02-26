 
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
#include <minizinc/plugin.hh>
#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#ifndef _WIN32
#define __stdcall
#endif

// Workaround for SCIP replacing function calls with macros in release mode
#ifdef NDEBUG
#define SCIPinfinityPlugin(plugin, scip) SCIPinfinity(scip)
#else
#define SCIPinfinityPlugin(plugin, scip) plugin->SCIPinfinity(scip)
#endif

class ScipPlugin : public MiniZinc::Plugin {
public:
  ScipPlugin();
  ScipPlugin(const std::string& dll);
  
  int(__stdcall *SCIPmajorVersion) (void);
  int(__stdcall *SCIPminorVersion) (void);
  int(__stdcall *SCIPtechVersion) (void);
  int(__stdcall *SCIPsubversion) (void);
  void(__stdcall *SCIPprintError)(SCIP_RETCODE retcode);
  SCIP_RETCODE(__stdcall *SCIPcreate)(SCIP** scip);
  SCIP_RETCODE(__stdcall *SCIPincludeDefaultPlugins)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPcreateProbBasic)(SCIP* scip, const char* name);
  SCIP_RETCODE(__stdcall *SCIPfree)(SCIP** scip);
  SCIP_RETCODE(__stdcall *SCIPcreateVarBasic)(SCIP* scip, SCIP_VAR** var, const char* name, SCIP_Real lb, SCIP_Real ub, SCIP_Real obj, SCIP_VARTYPE vartype);
  SCIP_RETCODE(__stdcall *SCIPaddVar)(SCIP* scip, SCIP_VAR* var);
  SCIP_RETCODE(__stdcall *SCIPreleaseVar)(SCIP* scip, SCIP_VAR** var);
  SCIP_Real(__stdcall *SCIPinfinity)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPcreateConsBasicLinear)(SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars, SCIP_Real* vals, SCIP_Real lhs, SCIP_Real rhs);
  SCIP_RETCODE(__stdcall *SCIPaddCons)(SCIP* scip, SCIP_CONS* cons);
  SCIP_RETCODE(__stdcall *SCIPreleaseCons)(SCIP* scip, SCIP_CONS** cons);
  SCIP_RETCODE(__stdcall *SCIPchgVarLbGlobal)(SCIP* scip, SCIP_VAR* var, SCIP_Real newbound);
  SCIP_RETCODE(__stdcall *SCIPchgVarUbGlobal)(SCIP* scip, SCIP_VAR* var, SCIP_Real newbound);
  SCIP_RETCODE(__stdcall *SCIPgetNegatedVar)(SCIP* scip, SCIP_VAR* var, SCIP_VAR** negvar);
  SCIP_RETCODE(__stdcall *SCIPcreateConsBasicIndicator)(SCIP* scip, SCIP_CONS** cons, const char* name, SCIP_VAR* binvar, int nvars, SCIP_VAR** vars, SCIP_Real* vals, SCIP_Real rhs);
  SCIP_RETCODE(__stdcall *SCIPcreateConsBasicBounddisjunction)(SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars, SCIP_BOUNDTYPE* boundtypes, SCIP_Real* bounds);
  SCIP_RETCODE(__stdcall *SCIPcreateConsBasicCumulative)(SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars, int* durations, int* demands, int capacity);
  SCIP_Longint(__stdcall *SCIPgetNSolsFound)(SCIP* scip);
  int (__stdcall *SCIPgetNSols)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPsetIntParam)(SCIP* scip, const char* name, int value);
  SCIP_RETCODE(__stdcall *SCIPsetRealParam)(SCIP* scip, const char* name, SCIP_Real value);
  SCIP_RETCODE(__stdcall *SCIPwriteOrigProblem)(SCIP* scip, const char* filename, const char* extension, SCIP_Bool genericnames);
  void(__stdcall *SCIPsetMessagehdlrQuiet)(SCIP* scip, SCIP_Bool quiet);
  SCIP_RETCODE(__stdcall *SCIPmessagehdlrCreate)(SCIP_MESSAGEHDLR** messagehdlr, SCIP_Bool bufferedoutput, const char* filename, SCIP_Bool quiet, SCIP_DECL_MESSAGEWARNING((*messagewarning)), SCIP_DECL_MESSAGEDIALOG((*messagedialog)), SCIP_DECL_MESSAGEINFO((*messageinfo)), SCIP_DECL_MESSAGEHDLRFREE((*messagehdlrfree)), SCIP_MESSAGEHDLRDATA* messagehdlrdata);
  SCIP_RETCODE(__stdcall *SCIPsetMessagehdlr)(SCIP* scip, SCIP_MESSAGEHDLR* messagehdlr);
  SCIP_RETCODE(__stdcall *SCIPreadParams)(SCIP* scip, const char* filename);
  SCIP_RETCODE(__stdcall *SCIPwriteParams)(SCIP* scip, const char* filename, SCIP_Bool comments, SCIP_Bool onlychanged);
  SCIP_RETCODE(__stdcall *SCIPsolve)(SCIP* scip);
  SCIP_STATUS(__stdcall *SCIPgetStatus)(SCIP* scip);
  SCIP_Real(__stdcall *SCIPgetPrimalbound)(SCIP* scip);
  SCIP_Real(__stdcall *SCIPgetDualbound)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPgetSolVals)(SCIP* scip, SCIP_SOL* sol, int nvars, SCIP_VAR** vars, SCIP_Real* vals);
  SCIP_SOL*(__stdcall *SCIPgetBestSol)(SCIP* scip);
  SCIP_Longint(__stdcall *SCIPgetNNodes)(SCIP* scip);
  int(__stdcall *SCIPgetNNodesLeft)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPfreeTransform)(SCIP* scip);
  SCIP_RETCODE(__stdcall *SCIPsetObjsense)(SCIP* scip, SCIP_OBJSENSE objsense);
  const char*(__stdcall *SCIPeventhdlrGetName)(SCIP_EVENTHDLR* eventhdlr);
  SCIP_RETCODE(__stdcall *SCIPcatchEvent)(SCIP* scip, SCIP_EVENTTYPE eventtype, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENTDATA* eventdata, int* filterpos);
  SCIP_RETCODE(__stdcall *SCIPdropEvent)(SCIP* scip, SCIP_EVENTTYPE eventtype, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENTDATA* eventdata, int filterpos);
  SCIP_EVENTTYPE(__stdcall *SCIPeventGetType)(SCIP_EVENT* event);
  SCIP_Real(__stdcall *SCIPgetSolOrigObj)(SCIP* scip, SCIP_SOL* sol);
  SCIP_RETCODE(__stdcall *SCIPincludeEventhdlrBasic)(SCIP* scip, SCIP_EVENTHDLR** eventhdlrptr, const char* name, const char* desc, SCIP_DECL_EVENTEXEC((*eventexec)), SCIP_EVENTHDLRDATA* eventhdlrdata);
  SCIP_RETCODE(__stdcall *SCIPsetEventhdlrInit)(SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_DECL_EVENTINIT((*eventinit)));
  SCIP_RETCODE(__stdcall *SCIPsetEventhdlrExit)(SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_DECL_EVENTEXIT((*eventexit)));
  void(__stdcall *SCIPmessagePrintErrorHeader)(const char* sourcefile, int sourceline);
  void(__stdcall *SCIPmessagePrintError)(const char* formatstr, ...);
  int(__stdcall *SCIPgetNVars)(SCIP* scip);
  int(__stdcall *SCIPgetNConss)(SCIP* scip);

private:
  void load(void);
};

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

      std::string scip_dll;

      bool processOption(int& i, std::vector<std::string>& argv);
      static void printHelp(std::ostream& );
    };
  private:
    Options* options = nullptr;
    ScipPlugin* plugin = nullptr;
  public:

    MIP_scip_wrapper(Options* opt) : options(opt) { wrap_assert( openSCIP() ); }
    virtual ~MIP_scip_wrapper() { wrap_assert( delSCIPVars() ); wrap_assert( closeSCIP() ); }
    
    static std::string getDescription(MiniZinc::SolverInstanceBase::Options* opt=NULL);
    static std::string getVersion(MiniZinc::SolverInstanceBase::Options* opt=NULL);
    static std::string getId(void);
    static std::string getName(void);
    static std::vector<std::string> getTags(void);
    static std::vector<std::string> getStdFlags(void);
    static std::vector<std::string> getRequiredFlags(void);

    bool processOption(int& i, int argc, const char** argv);
    void printVersion(std::ostream& );
    void printHelp(std::ostream& );
//       Statistics& getStatistics() { return _statistics; }

//      IloConstraintArray *userCuts, *lazyConstraints;

    /// derived should overload and call the ancestor
//     virtual void cleanup();
    SCIP_RETCODE openSCIP();
    SCIP_RETCODE closeSCIP();

    SCIP_RETCODE includeEventHdlrBestsol();
    
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
    /// Bounds disj for SCIP
    virtual void addBoundsDisj(int n, double *fUB, double *bnd, int* vars,
                               int nF, double *fUBF, double *bndF, int* varsF,
                        std::string rowName = "");

    /// Cumulative, currently SCIP only
    virtual void addCumulative(int nnz, int *rmatind, double* d, double* r, double b, std::string rowName="");
    virtual void setObjSense(int s) {   // +/-1 for max/min
      wrap_assert( setObjSense_SCIP(s) );
    }
    virtual SCIP_RETCODE setObjSense_SCIP(int s);
    
    virtual double getInfBound() { return SCIPinfinityPlugin(plugin, scip); }
                        
    virtual int getNCols() { return plugin->SCIPgetNVars (scip); }
    virtual int getNRows() { return plugin->SCIPgetNConss (scip); }
                        
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

  public:
    /// Default MZN library for SCIP
    static std::string getMznLib();
};

#endif  // __MIP_SCIP_WRAPPER_H__
