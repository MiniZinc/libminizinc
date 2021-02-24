
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/plugin.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <scip/scip.h>
#include <scip/scipdefplugins.h>

#ifndef _WIN32
// NOLINTNEXTLINE(bugprone-reserved-identifier)
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

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPmajorVersion)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPminorVersion)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPtechVersion)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPsubversion)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* SCIPprintError)(SCIP_RETCODE retcode);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreate)(SCIP** scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPincludeDefaultPlugins)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateProbBasic)(SCIP* scip, const char* name);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPfree)(SCIP** scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateVarBasic)
  (SCIP* scip, SCIP_VAR** var, const char* name, SCIP_Real lb, SCIP_Real ub, SCIP_Real obj,
   SCIP_VARTYPE vartype);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPaddVar)(SCIP* scip, SCIP_VAR* var);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPreleaseVar)(SCIP* scip, SCIP_VAR** var);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPinfinity)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicLinear)
  (SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars, SCIP_Real* vals,
   SCIP_Real lhs, SCIP_Real rhs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicQuadratic)
  (SCIP* scip,           /**< SCIP data structure */
   SCIP_CONS** cons,     /**< pointer to hold the created constraint */
   const char* name,     /**< name of constraint */
   int nlinvars,         /**< number of linear terms (n) */
   SCIP_VAR** linvars,   /**< array with variables in linear part (x_i) */
   SCIP_Real* lincoefs,  /**< array with coefficients of variables in linear part (b_i) */
   int nquadterms,       /**< number of quadratic terms (m) */
   SCIP_VAR** quadvars1, /**< array with first variables in quadratic terms (y_j) */
   SCIP_VAR** quadvars2, /**< array with second variables in quadratic terms (z_j) */
   SCIP_Real* quadcoefs, /**< array with coefficients of quadratic terms (a_j) */
   SCIP_Real lhs,        /**< left hand side of quadratic equation (ell) */
   SCIP_Real rhs         /**< right hand side of quadratic equation (u) */
  );
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPaddCons)(SCIP* scip, SCIP_CONS* cons);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPreleaseCons)(SCIP* scip, SCIP_CONS** cons);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgVarLbGlobal)(SCIP* scip, SCIP_VAR* var, SCIP_Real newbound);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgVarUbGlobal)(SCIP* scip, SCIP_VAR* var, SCIP_Real newbound);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPgetNegatedVar)(SCIP* scip, SCIP_VAR* var, SCIP_VAR** negvar);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicIndicator)
  (SCIP* scip, SCIP_CONS** cons, const char* name, SCIP_VAR* binvar, int nvars, SCIP_VAR** vars,
   SCIP_Real* vals, SCIP_Real rhs);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicBounddisjunction)
  (SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars,
   SCIP_BOUNDTYPE* boundtypes, SCIP_Real* bounds);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicCumulative)
  (SCIP* scip, SCIP_CONS** cons, const char* name, int nvars, SCIP_VAR** vars, int* durations,
   int* demands, int capacity);

  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicOrbisack)
  (SCIP* scip, SCIP_CONS** cons, const char* name, SCIP_VAR** vars1, SCIP_VAR** vars2, int nrows,
   SCIP_Bool ispporbisack, SCIP_Bool isparttype, SCIP_Bool ismodelcons);

  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcreateConsBasicOrbitope)
  (SCIP* scip,                     /**< SCIP data structure */
   SCIP_CONS** cons,               /**< pointer to hold the created constraint */
   const char* name,               /**< name of constraint */
   SCIP_VAR*** vars,               /**< matrix of variables on which the symmetry acts */
   SCIP_ORBITOPETYPE orbitopetype, /**< type of orbitope constraint */
   int nspcons,                    /**< number of set partitioning/packing constraints  <=> p */
   int nblocks,                    /**< number of symmetric variable blocks             <=> q */
   SCIP_Bool resolveprop,          /**< should propagation be resolved? */
   SCIP_Bool ismodelcons           /**< whether the orbitope is a model constraint */
  );

  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPgetNSolsFound)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPgetNSols)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetIntParam)(SCIP* scip, const char* name, int value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetRealParam)(SCIP* scip, const char* name, SCIP_Real value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPwriteOrigProblem)
  (SCIP* scip, const char* filename, const char* extension, SCIP_Bool genericnames);
  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* SCIPsetMessagehdlrQuiet)(SCIP* scip, SCIP_Bool quiet);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPmessagehdlrCreate)
  (SCIP_MESSAGEHDLR** messagehdlr, SCIP_Bool bufferedoutput, const char* filename, SCIP_Bool quiet,
   SCIP_DECL_MESSAGEWARNING((*messagewarning)), SCIP_DECL_MESSAGEDIALOG((*messagedialog)),
   SCIP_DECL_MESSAGEINFO((*messageinfo)), SCIP_DECL_MESSAGEHDLRFREE((*messagehdlrfree)),
   SCIP_MESSAGEHDLRDATA* messagehdlrdata);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetMessagehdlr)(SCIP* scip, SCIP_MESSAGEHDLR* messagehdlr);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPreadParams)(SCIP* scip, const char* filename);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPwriteParams)
  (SCIP* scip, const char* filename, SCIP_Bool comments, SCIP_Bool onlychanged);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsolve)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_STATUS(__stdcall* SCIPgetStatus)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPgetPrimalbound)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPgetDualbound)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPgetSolVals)
  (SCIP* scip, SCIP_SOL* sol, int nvars, SCIP_VAR** vars, SCIP_Real* vals);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_SOL*(__stdcall* SCIPgetBestSol)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPgetNTotalNodes)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPgetNNodes)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPgetNNodesLeft)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPfreeTransform)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetObjsense)(SCIP* scip, SCIP_OBJSENSE objsense);
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char*(__stdcall* SCIPeventhdlrGetName)(SCIP_EVENTHDLR* eventhdlr);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPcatchEvent)
  (SCIP* scip, SCIP_EVENTTYPE eventtype, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENTDATA* eventdata,
   int* filterpos);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPdropEvent)
  (SCIP* scip, SCIP_EVENTTYPE eventtype, SCIP_EVENTHDLR* eventhdlr, SCIP_EVENTDATA* eventdata,
   int filterpos);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_EVENTTYPE(__stdcall* SCIPeventGetType)(SCIP_EVENT* event);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPgetSolOrigObj)(SCIP* scip, SCIP_SOL* sol);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPincludeEventhdlrBasic)
  (SCIP* scip, SCIP_EVENTHDLR** eventhdlrptr, const char* name, const char* desc,
   SCIP_DECL_EVENTEXEC((*eventexec)), SCIP_EVENTHDLRDATA* eventhdlrdata);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetEventhdlrInit)
  (SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_DECL_EVENTINIT((*eventinit)));
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPsetEventhdlrExit)
  (SCIP* scip, SCIP_EVENTHDLR* eventhdlr, SCIP_DECL_EVENTEXIT((*eventexit)));
  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* SCIPmessagePrintErrorHeader)(const char* sourcefile, int sourceline);
  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* SCIPmessagePrintError)(const char* formatstr, ...);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPgetNVars)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPgetNConss)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_PARAM**(__stdcall* SCIPgetParams)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPgetNParams)(SCIP* scip);
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char*(__stdcall* SCIPparamGetName)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_PARAMTYPE(__stdcall* SCIPparamGetType)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  const char*(__stdcall* SCIPparamGetDesc)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Bool(__stdcall* SCIPparamGetBoolDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  char*(__stdcall* SCIPparamGetCharAllowedValues)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  char(__stdcall* SCIPparamGetCharDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPparamGetIntDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPparamGetIntMin)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* SCIPparamGetIntMax)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPparamGetLongintDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPparamGetLongintMin)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Longint(__stdcall* SCIPparamGetLongintMax)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPparamGetRealDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPparamGetRealMin)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_Real(__stdcall* SCIPparamGetRealMax)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  char*(__stdcall* SCIPparamGetStringDefault)(SCIP_PARAM* param);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_PARAM*(__stdcall* SCIPgetParam)(SCIP* scip, const char* name);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgBoolParam)(SCIP* scip, SCIP_PARAM* param, SCIP_Bool value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgIntParam)(SCIP* scip, SCIP_PARAM* param, int value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgLongintParam)(SCIP* scip, SCIP_PARAM* param, SCIP_Longint value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgRealParam)(SCIP* scip, SCIP_PARAM* param, SCIP_Real value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgCharParam)(SCIP* scip, SCIP_PARAM* param, char value);
  // NOLINTNEXTLINE(readability-identifier-naming)
  SCIP_RETCODE(__stdcall* SCIPchgStringParam)(SCIP* scip, SCIP_PARAM* param, const char* value);

private:
  void load();
};

class MIPScipWrapper : public MIPWrapper {
  SCIP* _scip = nullptr;
  //     SCIP_Retcode           retcode = SCIP_OKAY;
  //     char          scip_buffer[SCIP_MESSAGEBUFSIZE];
  //     char          scip_status_buffer[SCIP_MESSAGEBUFSIZE];

  std::vector<SCIP_VAR*> _scipVars;
  virtual SCIP_RETCODE delSCIPVars();

  std::vector<double> _x;

public:
  class FactoryOptions {
  public:
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir);

    std::string scipDll;
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int nThreads = 1;
    std::string sExportModel;
    int nTimeout = -1;
    double nWorkMemLimit = -1;
    std::string sReadParams;
    std::string sWriteParams;
    bool flagIntermediate = false;

    double absGap = -1;
    double relGap = 1e-8;
    double intTol = 1e-8;
    double objDiff = 1.0;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir);
    static void printHelp(std::ostream& os);
  };

private:
  FactoryOptions& _factoryOptions;
  Options* _options = nullptr;
  ScipPlugin* _plugin = nullptr;

public:
  MIPScipWrapper(FactoryOptions& factoryOpt, Options* opt)
      : _factoryOptions(factoryOpt), _options(opt) {
    SCIP_PLUGIN_CALL(openSCIP());
  }
  ~MIPScipWrapper() override {
    SCIP_RETCODE ret = delSCIPVars();
    assert(ret == SCIP_OKAY);
    ret = closeSCIP();
    assert(ret == SCIP_OKAY);
  }

  static std::string getDescription(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getVersion(FactoryOptions& factoryOpt,
                                MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getId();
  static std::string getName();
  static std::vector<std::string> getTags();
  static std::vector<std::string> getStdFlags();
  static std::vector<std::string> getRequiredFlags(FactoryOptions& factoryOpt);
  static std::vector<std::string> getFactoryFlags();

  static std::vector<MiniZinc::SolverConfig::ExtraFlag> getExtraFlags(FactoryOptions& factoryOpt);

  bool processOption(int& i, int argc, const char** argv,
                     const std::string& workingDir = std::string());
  void printVersion(std::ostream& os);
  void printHelp(std::ostream& os);
  //       Statistics& getStatistics() { return _statistics; }

  //      IloConstraintArray *userCuts, *lazyConstraints;

  /// derived should overload and call the ancestor
  //     virtual void cleanup();
  SCIP_RETCODE openSCIP();
  SCIP_RETCODE closeSCIP();

  SCIP_RETCODE includeEventHdlrBestsol();

  /// actual adding new variables to the solver
  void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                 std::string* names) override {
    SCIP_PLUGIN_CALL(doAddVarsSCIP(n, obj, lb, ub, vt, names));
  }
  virtual SCIP_RETCODE doAddVarsSCIP(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                                     std::string* names);
  void setVarBounds(int iVar, double lb, double ub) override;
  void setVarLB(int iVar, double lb) override;
  void setVarUB(int iVar, double ub) override;

  /// adding a linear constraint
  void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
              int mask = MaskConsType_Normal, const std::string& rowName = "") override {
    SCIP_PLUGIN_CALL(addRowSCIP(nnz, rmatind, rmatval, sense, rhs, mask, rowName));
  }
  virtual SCIP_RETCODE addRowSCIP(int nnz, int* rmatind, double* rmatval, LinConType sense,
                                  double rhs, int mask = MaskConsType_Normal,
                                  const std::string& rowName = "");
  /// adding an implication
  //     virtual void addImpl() = 0;
  /// Indicator constraint: x[iBVar]==bVal -> lin constr
  void addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind, double* rmatval,
                              LinConType sense, double rhs,
                              const std::string& rowName = "") override;
  /// Bounds disj for SCIP
  void addBoundsDisj(int n, double* fUB, double* bnd, int* vars, int nF, double* fUBF, double* bndF,
                     int* varsF, const std::string& rowName = "") override;

  /// Cumulative, currently SCIP only
  void addCumulative(int nnz, int* rmatind, double* d, double* r, double b,
                     const std::string& rowName = "") override;

  /// Lex-lesseq binary, currently SCIP only
  void addLexLesseq(int nnz, int* rmatind1, int* rmatind2, bool isModelCons,
                    const std::string& rowName = "") override;

  void addLexChainLesseq(int m, int n, int* rmatind, int nOrbitopeType, bool resolveprop,
                         bool isModelCons, const std::string& rowName = "") override;

  /// Times constraint: var[x]*var[y] == var[z]
  void addTimes(int x, int y, int z, const std::string& rowName = "") override;

  void setObjSense(int s) override {  // +/-1 for max/min
    SCIP_PLUGIN_CALL(setObjSenseSCIP(s));
  }
  virtual SCIP_RETCODE setObjSenseSCIP(int s);

  double getInfBound() override { return SCIPinfinityPlugin(_plugin, _scip); }

  int getNCols() override { return _plugin->SCIPgetNVars(_scip); }
  int getNRows() override { return _plugin->SCIPgetNConss(_scip); }

  //     void setObjUB(double ub) { objUB = ub; }
  //     void addQPUniform(double c) { qpu = c; } // also sets problem type to MIQP unless c=0

  void solve() override { SCIP_PLUGIN_CALL(solveSCIP()); }
  virtual SCIP_RETCODE solveSCIP();

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
  // NOLINTNEXTLINE(readability-identifier-naming)
  void SCIP_PLUGIN_CALL(SCIP_RETCODE retcode, const std::string& msg = "", bool fTerm = true);

  /// Need to consider the 100 status codes in SCIP and change with every version? TODO
  Status convertStatus(SCIP_STATUS scipStatus);
};
