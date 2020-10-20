
/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver_config.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

extern "C" {
#include <gurobi_c.h>  // need GUROBI_HOME defined
}

class MIPGurobiWrapper : public MIPWrapper {
  GRBenv* _env = nullptr;
  GRBmodel* _model = nullptr;
#ifdef GUROBI_PLUGIN
  void* _gurobiDll;
#endif
  int _error;
  std::string _gurobiBuffer;        // [GRB_MESSAGEBUFSIZE];
  std::string _gurobiStatusBuffer;  // [GRB_MESSAGEBUFSIZE];

  std::vector<double> _x;

public:
  class FactoryOptions {
  public:
    bool processOption(int& i, std::vector<std::string>& argv);

    std::string gurobiDll;
  };

  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int nMIPFocus = 0;
    int nFreeSearch = 1;
    int nThreads = 1;
    std::string sExportModel;
    int nTimeout1000 = -1;
    int nTimeoutFeas1000 = -1;
    int nSolLimit = -1;
    int nSeed = -1;
    double nWorkMemLimit = 0.5;
    std::string sNodefileDir;
    std::string sReadParams;
    std::string sWriteParams;
    std::vector<std::string> sConcurrentParamFiles;
    bool flagIntermediate = false;

    double absGap = -1;
    double relGap = 1e-8;
    double feasTol = 1e-8;
    double intTol = 1e-8;
    double objDiff = 1.0;
    int nonConvex = 2;

    std::unordered_map<std::string, std::string> extraParams;

    bool processOption(int& i, std::vector<std::string>& argv);
    static void printHelp(std::ostream& os);
  };

private:
  FactoryOptions& _factoryOptions;
  Options* _options = nullptr;

public:
  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* dll_GRBversion)(int*, int*, int*);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBaddconstr)(GRBmodel* model, int numnz, int* cind, double* cval, char sense,
                                   double rhs, const char* constrname);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBaddgenconstrMin)(GRBmodel* model, const char* name, int resvar, int nvars,
                                         const int* vars, double constant);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBaddqconstr)(GRBmodel* model, int numlnz, int* lind, double* lval,
                                    int numqnz, int* qrow, int* qcol, double* qval, char sense,
                                    double rhs, const char* QCname);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBaddgenconstrIndicator)(GRBmodel* model, const char* name, int binvar,
                                               int binval, int nvars, const int* ind,
                                               const double* val, char sense, double rhs);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBaddvars)(GRBmodel* model, int numvars, int numnz, int* vbeg, int* vind,
                                 double* vval, double* obj, double* lb, double* ub, char* vtype,
                                 char** varnames);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBcbcut)(void* cbdata, int cutlen, const int* cutind, const double* cutval,
                               char cutsense, double cutrhs);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBcbget)(void* cbdata, int where, int what, void* resultP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBcblazy)(void* cbdata, int lazylen, const int* lazyind,
                                const double* lazyval, char lazysense, double lazyrhs);

  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* dll_GRBfreeenv)(GRBenv* env);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBfreemodel)(GRBmodel* model);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetdblattr)(GRBmodel* model, const char* attrname, double* valueP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetdblattrarray)(GRBmodel* model, const char* attrname, int first, int len,
                                         double* values);

  // NOLINTNEXTLINE(readability-identifier-naming)
  GRBenv*(__stdcall* dll_GRBgetenv)(GRBmodel* model);

  // NOLINTNEXTLINE(readability-identifier-naming)
  const char*(__stdcall* dll_GRBgeterrormsg)(GRBenv* env);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetintattr)(GRBmodel* model, const char* attrname, int* valueP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBloadenv)(GRBenv** envP, const char* logfilename);

  // NOLINTNEXTLINE(readability-identifier-naming)
  GRBenv*(__stdcall* dll_GRBgetconcurrentenv)(GRBmodel* model, int num);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBnewmodel)(GRBenv* env, GRBmodel** modelP, const char* Pname, int numvars,
                                  double* obj, double* lb, double* ub, char* vtype,
                                  char** varnames);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBoptimize)(GRBmodel* model);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBreadparams)(GRBenv* env, const char* filename);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetcallbackfunc)(GRBmodel* model, int(__stdcall* cb)(CB_ARGS),
                                         void* usrdata);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetdblparam)(GRBenv* env, const char* paramname, double value);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetintparam)(GRBenv* env, const char* paramname, int value);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetintattr)(GRBmodel* model, const char* attrname, int newvalue);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetdblattrelement)(GRBmodel* model, const char* attrname, int iv, double v);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetintattrlist)(GRBmodel* model, const char* attrname, int len, int* ind,
                                        int* newvalues);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetdblattrlist)(GRBmodel* model, const char* attrname, int len, int* ind,
                                        double* newvalues);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetobjectiven)(GRBmodel* model, int index, int priority, double weight,
                                       double abstol, double reltol, const char* name,
                                       double constant, int lnz, int* lind, double* lval);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBsetstrparam)(GRBenv* env, const char* paramname, const char* value);

  // NOLINTNEXTLINE(readability-identifier-naming)
  void(__stdcall* dll_GRBterminate)(GRBmodel* model);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBupdatemodel)(GRBmodel* model);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBwrite)(GRBmodel* model, const char* filename);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBwriteparams)(GRBenv* env, const char* filename);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetintparam)(GRBenv* env, const char* paramname, int* valueP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBemptyenv)(GRBenv** envP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetnumparams)(GRBenv* env);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetparamname)(GRBenv* env, int i, char** paramnameP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetparamtype)(GRBenv* env, const char* paramname);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetintparaminfo)(GRBenv* env, const char* paramname, int* valueP, int* minP,
                                         int* maxP, int* defP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetdblparaminfo)(GRBenv* env, const char* paramname, double* valueP,
                                         double* minP, double* maxP, double* defP);

  // NOLINTNEXTLINE(readability-identifier-naming)
  int(__stdcall* dll_GRBgetstrparaminfo)(GRBenv* env, const char* paramname, char* valueP,
                                         char* defP);

  MIPGurobiWrapper(FactoryOptions& factoryOpt, Options* opt)
      : _factoryOptions(factoryOpt), _options(opt) {
    if (opt != nullptr) {
      openGUROBI();
    }
  }
  ~MIPGurobiWrapper() override { closeGUROBI(); }

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
  void openGUROBI();
  void closeGUROBI();

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
  void addMinimum(int iResultVar, int nnz, int* ind, const std::string& rowName = "") override;

  /// Times constraint: var[x]*var[y] == var[z]
  void addTimes(int x, int y, int z, const std::string& rowName = "") override;

  int getFreeSearch() override;
  bool addSearch(const std::vector<VarId>& vars, const std::vector<int>& pri) override;
  bool addWarmStart(const std::vector<VarId>& vars, const std::vector<double>& vals) override;
  bool defineMultipleObjectives(const MultipleObjectives& mo) override;

  int nRows = 0;  // to count rows in order tp notice lazy constraints
  std::vector<int> nLazyIdx;
  std::vector<int> nLazyValue;
  /// adding an implication
  //     virtual void addImpl() = 0;
  void setObjSense(int s) override;  // +/-1 for max/min

  double getInfBound() override { return GRB_INFINITY; }

  int getNCols() override {
    dll_GRBupdatemodel(_model);
    int cols;
    _error = dll_GRBgetintattr(_model, GRB_INT_ATTR_NUMVARS, &cols);
    return cols;
  }
  int getNRows() override {
    dll_GRBupdatemodel(_model);
    int cols;
    _error = dll_GRBgetintattr(_model, GRB_INT_ATTR_NUMCONSTRS, &cols);
    return cols;
  }

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

protected:
  void wrapAssert(bool cond, const std::string& msg, bool fTerm = true);

  /// Need to consider the 100 status codes in GUROBI and change with every version? TODO
  Status convertStatus(int gurobiStatus);
};
