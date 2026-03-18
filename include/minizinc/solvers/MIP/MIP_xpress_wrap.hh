/*
 *  Main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/plugin.hh>
#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <minizinc/_thirdparty/xpress_interface.h>

using namespace std;

class XpressPlugin {
public:
  XpressPlugin();
  XpressPlugin(const std::string& dll);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSinit)(const char* path);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSfree)();
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRSgetversion)(char* version);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetlicerrmsg)(char* buffer, int length);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRScreateprob)(XPRSprob* p_prob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSdestroyprob)(XPRSprob prob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSloadlp)(XPRSprob prob, const char* probname, int ncols, int nrows,
                           const char rowtype[], const double rhs[], const double rng[],
                           const double objcoef[], const int start[], const int collen[],
                           const int rowind[], const double rowcoef[], const double lb[],
                           const double ub[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSloadmip)(XPRSprob prob, const char* probname, int ncols, int nrows,
                            const char rowtype[], const double rhs[], const double rng[],
                            const double objcoef[], const int start[], const int collen[],
                            const int rowind[], const double rowcoef[], const double lb[],
                            const double ub[], int nentities, int nsets, const char coltype[],
                            const int entind[], const double limit[], const char settype[],
                            const int setstart[], const int setind[], const double refval[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddrows)(XPRSprob prob, int nrows, int ncoefs, const char rowtype[],
                            const double rhs[], const double rng[], const int start[],
                            const int colind[], const double rowcoef[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddcols)(XPRSprob prob, int ncols, int ncoefs, const double objcoef[],
                            const int start[], const int rowind[], const double rowcoef[],
                            const double lb[], const double ub[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSoptimize)(XPRSprob prob, const char* flags, int* solution_status,
                             int* objective_status);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetsolution)(XPRSprob prob, int* status, double x[], int first, int last);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSchgobjsense)(XPRSprob prob, int objsense);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSchgbounds)(XPRSprob prob, int nbounds, const int colind[], const char bndtype[],
                              const double bndval[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSchgcoltype)(XPRSprob prob, int ncols, const int colind[], const char coltype[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSwriteprob)(XPRSprob prob, const char* filename, const char* flags);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetlogfile)(XPRSprob prob, const char* logname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetintcontrol)(XPRSprob prob, int index, int ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetdblcontrol)(XPRSprob prob, int index, double dvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetintattrib)(XPRSprob prob, int index, int* ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetdblattrib)(XPRSprob prob, int index, double* dvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetlasterror)(XPRSprob prob, char* errmsg);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddcbintsol)(XPRSprob prob,
                                void(XPRS_CC* f_intsol)(XPRSprob prob, void* vContext), void* p,
                                int priority);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddcbmessage)(XPRSprob prob,
                                 void(XPRS_CC* f_message)(XPRSprob prob, void* vContext,
                                                          const char* msg, int len, int msgtype),
                                 void* p, int priority);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetcontrolinfo)(XPRSprob prob, const char* sCaName, int* iHeaderId,
                                   int* iTypeinfo);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetintcontrol)(XPRSprob prob, int index, int* ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetintcontrol64)(XPRSprob prob, int index, XPRSint64* ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetdblcontrol)(XPRSprob prob, int index, double* dvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetintcontrol64)(XPRSprob prob, int index, XPRSint64 ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetstringcontrol)(XPRSprob prob, int index, char* svalue, int svaluesize,
                                     int* controlsize);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetstrcontrol)(XPRSprob prob, int index, const char* svalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddmipsol)(XPRSprob prob, int ilength, const double mipsolval[],
                              const int mipsolcol[], const char* solname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddindicators)(XPRSprob prob, int nrows, const int rowind[], const int colind[],
                                  const int complement[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSaddqmatrix)(XPRSprob prob, int row, int ncoefs, const int rowqcol1[],
                               const int rowqcol2[], const double rowqcoef[]);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsaveas)(XPRSprob prob, const char* filename);

  const std::string& path() const { return _inner.path(); }

private:
  MiniZinc::Plugin _inner;
  void loadDll();
  static const std::vector<std::string>& dlls();
};

class MIPxpressWrapper : public MIPWrapper {
public:
  class FactoryOptions {
  public:
    bool processOption(int& i, std::vector<std::string>& argv, const std::string& workingDir);

    std::string xpressDll;
    std::string xprsPassword;
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int msgLevel = 0;
    int nTimeout = -1;
    int numSolutions = 0;
    std::string logFile;
    std::string writeModelFile;
    std::string writeModelFormat = "lp";
    double absGap = 0;
    double relGap = 0.0001;
    bool intermediateSolutions = false;

    int numThreads = 0;
    int randomSeed = 0;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv,
                       const std::string& workingDir = std::string());
    static void printHelp(std::ostream& os);
  };

private:
  FactoryOptions& _factoryOptions;
  Options* _options = nullptr;
  XpressPlugin* _plugin = nullptr;

public:
  void doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                 string* names) override;
  void addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
              int mask = MaskConsType_Normal, const string& rowName = "") override;
  void setObjSense(int s) override;
  void solve() override;
  void setVarLB(int iVar, double lb) override;
  void setVarUB(int iVar, double ub) override;
  void setVarBounds(int iVar, double lb, double ub) override;
  void addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind, double* rmatval,
                              LinConType sense, double rhs,
                              const std::string& rowName = "") override;
  bool addWarmStart(const std::vector<VarId>& vars, const std::vector<double>& vals) override;
  void addTimes(int x, int y, int z, const std::string& rowName = "") override;

  int getNCols() override { return static_cast<int>(_nCols); }
  int getNRows() override { return static_cast<int>(_nRows); }
  double getInfBound() override { return XPRS_PLUSINFINITY; }

  MIPxpressWrapper(FactoryOptions& factoryOpt, Options* opt)
      : _factoryOptions(factoryOpt), _options(opt) {
    openXpress();
  };
  ~MIPxpressWrapper() override { closeXpress(); };

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

private:
  XPRSprob _problem{nullptr};

  // Solution storage:
  std::vector<double> _x;  // Solution values

  // Problem data (batch mode for C API):
  std::vector<double> _obj;  // Objective coefficients
  std::vector<double> _lb;   // Variable lower bounds
  std::vector<double> _ub;   // Variable upper bounds
  std::vector<char> _vtype;  // Variable types ('C', 'B', 'I')

  // Constraint data:
  std::vector<char> _rowtype;    // 'L', 'E', 'G'
  std::vector<double> _rhs;      // RHS values
  std::vector<double> _rng;      // Range values (for ranged constraints)
  std::vector<int> _start;       // Start indices for constraint coefficients
  std::vector<int> _rowind;      // Column indices in constraints
  std::vector<double> _rowcoef;  // Coefficient values

  // Indicator constraint data:
  std::vector<int> _indicatorRows;         // Row indices for indicator constraints
  std::vector<int> _indicatorVars;         // Binary variable indices
  std::vector<int> _indicatorComplements;  // Complement flags

  // Bilinear term data (for constraints like z = x*y, implemented via XPRSaddqmatrix):
  struct BilinearTerm {
    int row;      // Row index where constraint is added
    int x, y, z;  // Variables: z = x * y
  };
  std::vector<BilinearTerm> _bilinearTerms;

  size_t _nCols{0};
  size_t _nRows{0};
  int _objsense{-1};  // Objective sense: XPRS_OBJ_MINIMIZE (-1) or XPRS_OBJ_MAXIMIZE (1)
  bool _problemLoaded{false};

  void openXpress();
  void closeXpress();

  void checkDLL();

  void setUserSolutionCallback();
  void setOptions();
  void writeModelIfRequested();
  void loadProblem();
};
