
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/config.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <ilcplex/cplex.h>  // add -DCPLEX_STUDIO_DIR=/opt/ibm/ILOG/CPLEX_Studio1261 to the 1st call of cmake

class MIPCplexWrapper : public MIPWrapper {
  CPXENVptr _env = nullptr;
  CPXLPptr _lp = nullptr;

  int _status;
  char _cplexBuffer[CPXMESSAGEBUFSIZE];
  char _cplexStatusBuffer[CPXMESSAGEBUFSIZE];

  std::vector<double> _x;

#ifdef CPLEX_PLUGIN
  void* _cplexDll;
#endif

public:
  class FactoryOptions {
  public:
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir);

    std::string cplexDll;
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int nMIPFocus = 0;
    int nThreads = 1;
    std::string sExportModel;
    int nTimeout = -1;
    long int nSolLimit = -1;
    int nSeed = -1;
    double nWorkMemLimit = 0.5;  // although CPLEX 12.10 has default 2GB
    std::string sNodefileDir;
    std::string sReadParams;
    std::string sWriteParams;
    bool flagIntermediate = false;

    double absGap = -1;
    double relGap = 1e-8;
    double intTol = 1e-8;
    double objDiff = 1.0;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv,
                       const std::string& workingDir = std::string());
    static void printHelp(std::ostream& os);
  };

private:
  FactoryOptions& _factoryOptions;
  Options* _options = nullptr;

public:
  MIPCplexWrapper(FactoryOptions& factoryOpt, Options* opt)
      : _factoryOptions(factoryOpt), _options(opt) {
    openCPLEX();
  }
  ~MIPCplexWrapper() override { closeCPLEX(); }

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

  //       Statistics& getStatistics() { return _statistics; }

  //      IloConstraintArray *userCuts, *lazyConstraints;

  /// derived should overload and call the ancestor
  //     virtual void cleanup();
  void checkDLL();
  void openCPLEX();
  void closeCPLEX();

  /// actual adding new variables to the solver
  void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                 std::string* names) override;

  /// adding a linear constraint
  void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
              int mask = MaskConsType_Normal, const std::string& rowName = "") override;
  void setVarBounds(int iVar, double lb, double ub) override;
  void setVarLB(int iVar, double lb) override;
  void setVarUB(int iVar, double ub) override;
  /// Indicator constraint: x[iBVar]==bVal -> lin constr
  void addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind, double* rmatval,
                              LinConType sense, double rhs,
                              const std::string& rowName = "") override;
  bool addWarmStart(const std::vector<VarId>& vars, const std::vector<double>& vals) override;
  /// adding an implication
  //     virtual void addImpl() = 0;
  void setObjSense(int s) override;  // +/-1 for max/min

  double getInfBound() override { return CPX_INFBOUND; }

  int getNCols() override { return dll_CPXgetnumcols(_env, _lp); }
  int getNRows() override { return dll_CPXgetnumrows(_env, _lp); }

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

  // CPLEX API

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddfuncdest)(CPXCENVptr, CPXCHANNELptr, void*,
                            void (*msgfunction)(void*, const char*));
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddindconstr)(CPXCENVptr, CPXLPptr, int, int, int, double, int, int const*,
                             double const*, char const*);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddlazyconstraints)(CPXCENVptr env, CPXLPptr lp, int rcnt, int nzcnt,
                                   double const* rhs, char const* sense, int const* rmatbeg,
                                   int const* rmatind, double const* rmatval, char** rowname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddmipstarts)(CPXCENVptr env, CPXLPptr lp, int mcnt, int nzcnt, int const* beg,
                             int const* varindices, double const* values, int const* effortlevel,
                             char** mipstartname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddrows)(CPXCENVptr env, CPXLPptr lp, int ccnt, int rcnt, int nzcnt,
                        double const* rhs, char const* sense, int const* rmatbeg,
                        int const* rmatind, double const* rmatval, char** colname, char** rowname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXaddusercuts)(CPXCENVptr env, CPXLPptr lp, int rcnt, int nzcnt, double const* rhs,
                            char const* sense, int const* rmatbeg, int const* rmatind,
                            double const* rmatval, char** rowname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXchgbds)(CPXCENVptr env, CPXLPptr lp, int cnt, int const* indices, char const* lu,
                       double const* bd);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXchgmipstarts)(CPXCENVptr env, CPXLPptr lp, int mcnt, int const* mipstartindices,
                             int nzcnt, int const* beg, int const* varindices, double const* values,
                             int const* effortlevel);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXchgobjsen)(CPXCENVptr env, CPXLPptr lp, int maxormin);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXcloseCPLEX)(CPXENVptr* env_p);

  // NOLINTNEXTLINE(readability-identifier-naming)
  CPXLPptr (*dll_CPXcreateprob)(CPXCENVptr env, int* status_p, char const* probname_str);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXcutcallbackadd)(CPXCENVptr env, void* cbdata, int wherefrom, int nzcnt, double rhs,
                               int sense, int const* cutind, double const* cutval, int purgeable);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXfreeprob)(CPXCENVptr env, CPXLPptr* lp_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetbestobjval)(CPXCENVptr env, CPXCLPptr lp, double* objval_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetcallbackincumbent)(CPXCENVptr env, void* cbdata, int wherefrom, double* x,
                                     int begin, int end);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetcallbackinfo)(CPXCENVptr env, void* cbdata, int wherefrom, int whichinfo,
                                void* result_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetcallbacknodeinfo)(CPXCENVptr env, void* cbdata, int wherefrom, int nodeindex,
                                    int whichinfo, void* result_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetcallbacknodex)(CPXCENVptr env, void* cbdata, int wherefrom, double* x, int begin,
                                 int end);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetchannels)(CPXCENVptr env, CPXCHANNELptr* cpxresults_p,
                            CPXCHANNELptr* cpxwarning_p, CPXCHANNELptr* cpxerror_p,
                            CPXCHANNELptr* cpxlog_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetdettime)(CPXCENVptr env, double* dettimestamp_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  CPXCCHARptr (*dll_CPXgeterrorstring)(CPXCENVptr env, int errcode, char* buffer_str);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetmipstartindex)(CPXCENVptr env, CPXCLPptr lp, char const* lname_str, int* index_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetnodecnt)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetnodeleftcnt)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetnumcols)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetnumrows)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetobjsen)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetobjval)(CPXCENVptr env, CPXCLPptr lp, double* objval_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetsolnpoolnumsolns)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetstat)(CPXCENVptr env, CPXCLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  CPXCHARptr (*dll_CPXgetstatstring)(CPXCENVptr env, int statind, char* buffer_str);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgettime)(CPXCENVptr env, double* timestamp_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetx)(CPXCENVptr env, CPXCLPptr lp, double* x, int begin, int end);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXmipopt)(CPXCENVptr env, CPXLPptr lp);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXnewcols)(CPXCENVptr env, CPXLPptr lp, int ccnt, double const* obj, double const* lb,
                        double const* ub, char const* xctype, char** colname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  CPXENVptr (*dll_CPXopenCPLEX)(int* status_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXreadcopyparam)(CPXENVptr env, char const* filename_str);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetdblparam)(CPXENVptr env, int whichparam, double newvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetinfocallbackfunc)(CPXENVptr env, int (*callback)(CPXCENVptr, void*, int, void*),
                                    void* cbhandle);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetintparam)(CPXENVptr env, int whichparam, CPXINT newvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetstrparam)(CPXENVptr env, int whichparam, char const* newvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetlazyconstraintcallbackfunc)(CPXENVptr env,
                                              int (*lazyconcallback)(CALLBACK_CUT_ARGS),
                                              void* cbhandle);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetusercutcallbackfunc)(CPXENVptr env, int (*cutcallback)(CALLBACK_CUT_ARGS),
                                       void* cbhandle);
  // NOLINTNEXTLINE(readability-identifier-naming)
  CPXCCHARptr (*dll_CPXversion)(CPXCENVptr env);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXwriteparam)(CPXCENVptr env, char const* filename_str);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXwriteprob)(CPXCENVptr env, CPXCLPptr lp, char const* filename_str,
                          char const* filetype_str);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetparamname)(CPXCENVptr env, int whichparam, char* name_str);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetparamnum)(CPXCENVptr env, char const* name_str, int* whichparam_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXgetparamtype)(CPXCENVptr env, int whichparam, int* paramtype);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXinfodblparam)(CPXCENVptr env, int whichparam, double* defvalue_p, double* minvalue_p,
                             double* maxvalue_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXinfointparam)(CPXCENVptr env, int whichparam, CPXINT* defvalue_p, CPXINT* minvalue_p,
                             CPXINT* maxvalue_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXinfolongparam)(CPXCENVptr env, int whichparam, CPXLONG* defvalue_p,
                              CPXLONG* minvalue_p, CPXLONG* maxvalue_p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXinfostrparam)(CPXCENVptr env, int whichparam, char* defvalue_str);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int (*dll_CPXsetlongparam)(CPXENVptr env, int whichparam, CPXLONG newvalue);

protected:
  void wrapAssert(bool cond, const std::string& msg, bool fTerm = true);

  /// Need to consider the 100 status codes in CPLEX and change with every version? TODO
  Status convertStatus(int cplexStatus);
};
