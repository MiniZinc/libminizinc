/*
 *  Main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/plugin.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/solvers/MIP/MIP_wrap.hh>

#include <xprb.h>
#include <xprs.h>

using namespace std;

/// xprs.dll depends on this library
class XprlPlugin : MiniZinc::Plugin {
public:
  XprlPlugin();
  XprlPlugin(const std::string& dll);

private:
  static const std::vector<std::string>& dlls();
};

class XpressPlugin : MiniZinc::Plugin {
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
  struct xo_prob_struct*(XB_CC* XPRBgetXPRSprob)(struct Xbprob* prob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetmsglevel)(struct Xbprob* prob, int level);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetlogfile)(XPRSprob prob, const char* logname);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetintcontrol)(XPRSprob prob, int _index, int _ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetdblcontrol)(XPRSprob prob, int _index, double _dvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  double(XB_CC* XPRBgetsol)(struct Xbvar* var);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetintattrib)(XPRSprob prob, int _index, int* _ivalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSgetdblattrib)(XPRSprob prob, int _index, double* _dvalue);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBbegincb)(struct Xbprob* prob, struct xo_prob_struct* optprob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsync)(struct Xbprob* prob, int synctype);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBendcb)(struct Xbprob* prob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetterm)(struct Xbctr* lct, struct Xbvar* var, double coeff);
  // NOLINTNEXTLINE(readability-identifier-naming)
  struct Xbvar*(XB_CC* XPRBnewvar)(struct Xbprob* prob, int type, const char* name, double bdl,
                                   double bdu);
  // NOLINTNEXTLINE(readability-identifier-naming)
  struct Xbctr*(XB_CC* XPRBnewctr)(struct Xbprob* prob, const char* name, int qrtype);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetctrtype)(struct Xbctr* lct, int qrtype);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBexportprob)(struct Xbprob* prob, int format, const char* filename);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBgetbounds)(struct Xbvar* var, double* lbd, double* ubd);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetobj)(struct Xbprob* prob, struct Xbctr* ctr);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBmipoptimize)(struct Xbprob* prob, const char* alg);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetsense)(struct Xbprob* prob, int dir);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XPRS_CC* XPRSsetcbintsol)(XPRSprob prob,
                                void(XPRS_CC* f_intsol)(XPRSprob prob, void* vContext), void* p);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetub)(struct Xbvar* var, double c);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetlb)(struct Xbvar* var, double c);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetindicator)(struct Xbctr* lct, int dir, struct Xbvar* var);
  // NOLINTNEXTLINE(readability-identifier-naming)
  struct Xbsol*(XB_CC* XPRBnewsol)(struct Xbprob* prob);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBsetsolvar)(struct Xbsol* sol, struct Xbvar* var, double coeff);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBaddmipsol)(struct Xbprob* prob, struct Xbsol* sol, const char* name);
  // NOLINTNEXTLINE(readability-identifier-naming)
  struct Xbprob*(XB_CC* XPRBnewprob)(const char* name);
  // NOLINTNEXTLINE(readability-identifier-naming)
  int(XB_CC* XPRBdelprob)(struct Xbprob* prob);

private:
  void loadDll();
  static const std::vector<std::string>& dlls();
};

class MIPxpressWrapper : public MIPWrapper {
public:
  class Options : public MiniZinc::SolverInstanceBase::Options {
  public:
    int msgLevel = 0;
    int timeout = 0;
    int numSolutions = 0;
    std::string logFile = "";
    std::string writeModelFile = "";
    std::string writeModelFormat = "lp";
    double absGap = 0;
    double relGap = 0.0001;
    bool intermediateSolutions = false;
    bool processOption(int& i, std::vector<std::string>& argv);
    std::string xprsPassword;
    std::string xprsRoot;
    static void printHelp(std::ostream& os);
  };

private:
  Options* _options = nullptr;
  XprlPlugin* _pluginDep = nullptr;
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

  int getNCols() override { return _variables.size(); }
  int getNRows() override { return _nRows; }
  double getInfBound() override { return XPRB_INFINITY; }
  const double* getValues() override { return output.x; }
  double getObjValue() override { return output.objVal; }
  double getBestBound() override { return output.bestBound; }
  double getCPUTime() override { return output.dCPUTime; }
  Status getStatus() override { return output.status; }
  string getStatusName() override { return output.statusName; }
  int getNNodes() override { return output.nNodes; }
  int getNOpen() override { return output.nOpenNodes; }

  MIPxpressWrapper(Options* opt) : _options(opt) { openXpress(); };
  ~MIPxpressWrapper() override { closeXpress(); };

  static std::string getDescription(MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getVersion(MiniZinc::SolverInstanceBase::Options* opt = nullptr);
  static std::string getId();
  static std::string getName();
  static std::vector<std::string> getTags();
  static std::vector<std::string> getStdFlags();
  static std::vector<std::string> getRequiredFlags();

private:
  XPRBprob _problem;
  XPRBctr _xpressObj;
  vector<XPRBvar> _variables;
  size_t _nRows{0};

  void openXpress();
  void closeXpress();

  void setUserSolutionCallback();
  void setOptions();
  void writeModelIfRequested();
  static int convertConstraintType(LinConType sense);
  static int convertVariableType(VarType varType);
  static int convertObjectiveSense(int s);
  XPRBctr addConstraint(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
                        int mask, const string& rowName);
  void addDummyConstraint();
};
