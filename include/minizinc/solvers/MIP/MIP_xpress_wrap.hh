/*
 *  Main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_XPRESS_WRAPPER_H__
#define __MIP_XPRESS_WRAPPER_H__
#include <minizinc/solvers/MIP/MIP_wrap.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/plugin.hh>

#include <xprb.h>
#include <xprs.h>

using namespace std;

/// xprs.dll depends on this library
class XprlPlugin : MiniZinc::Plugin {
public:
  XprlPlugin();
  XprlPlugin(const std::string& dll);
private:
  static const std::vector<std::string>& dlls(void);
};

class XpressPlugin : MiniZinc::Plugin {
public:
  XpressPlugin();
  XpressPlugin(const std::string& dll);
  int (XB_CC *XPRSgetversion)(char* version);
  struct xo_prob_struct* (XB_CC* XPRBgetXPRSprob)(struct Xbprob *  prob);
  int (XB_CC *XPRBsetmsglevel)(struct Xbprob * prob, int level);
  int (XPRS_CC *XPRSsetlogfile)(XPRSprob prob, const char* logname);
  int (XPRS_CC *XPRSsetintcontrol)(XPRSprob prob, int _index, int _ivalue);
  int (XPRS_CC *XPRSsetdblcontrol)(XPRSprob prob, int _index, double _dvalue);
  double (XB_CC *XPRBgetsol)(struct Xbvar * var);
  int (XPRS_CC *XPRSgetintattrib)(XPRSprob prob, int _index, int* _ivalue);
  int (XPRS_CC *XPRSgetdblattrib)(XPRSprob prob, int _index, double* _dvalue);
  int (XB_CC *XPRBbegincb)(struct Xbprob * prob, struct xo_prob_struct* optprob);
  int (XB_CC *XPRBsync)(struct Xbprob * prob, int synctype);
  int (XB_CC *XPRBendcb)(struct Xbprob * prob);
  int (XB_CC *XPRBsetterm)(struct Xbctr * lct, struct Xbvar * var, double coeff);
  struct Xbvar* (XB_CC *XPRBnewvar)(struct Xbprob * prob, int type, const char *name, double bdl, double bdu);
  struct Xbctr* (XB_CC *XPRBnewctr)(struct Xbprob * prob, const char *name, int qrtype);
  int (XB_CC *XPRBsetctrtype)(struct Xbctr * lct, int qrtype);
  int (XB_CC *XPRBexportprob)(struct Xbprob * prob, int format, const char *filename);
  int (XB_CC *XPRBgetbounds)(struct Xbvar * var, double *lbd, double *ubd);
  int (XB_CC *XPRBsetobj)(struct Xbprob * prob, struct Xbctr * ctr);
  int (XB_CC *XPRBmipoptimize)(struct Xbprob * prob, const char *alg);
  int (XB_CC *XPRBsetsense)(struct Xbprob * prob, int dir);
  int (XPRS_CC *XPRSsetcbintsol)(XPRSprob prob, void (XPRS_CC *f_intsol)(XPRSprob prob, void* vContext), void* p);
  int (XB_CC *XPRBsetub)(struct Xbvar * var, double c);
  int (XB_CC *XPRBsetlb)(struct Xbvar * var, double c);
  int (XB_CC *XPRBsetindicator)(struct Xbctr * lct, int dir, struct Xbvar * var);
  struct Xbsol* (XB_CC *XPRBnewsol)(struct Xbprob * prob);
  int (XB_CC *XPRBsetsolvar)(struct Xbsol * sol, struct Xbvar * var, double coeff);
  int (XB_CC *XPRBaddmipsol)(struct Xbprob * prob, struct Xbsol * sol, const char *name);
  struct Xbprob* (XB_CC *XPRBnewprob)(const char *name);
private:
  void load_dll(void);
  static const std::vector<std::string>& dlls(void);
};

class MIP_xpress_wrapper : public MIP_wrapper {
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
      bool printAllSolutions = false;
      bool processOption(int& i, std::vector<std::string>& argv);
      std::string xprlFile;
      std::string xprsFile;
      static void printHelp(std::ostream& );
    };
  private:
    Options* options=nullptr;
    XprlPlugin* plugin_dep = nullptr;
    XpressPlugin* plugin = nullptr;
  public:

public:
  virtual void doAddVars(size_t n, double *obj, double *lb, double *ub,
                         VarType *vt, string *names);
  virtual void addRow(int nnz, int *rmatind, double *rmatval, LinConType sense,
                      double rhs, int mask = MaskConsType_Normal,
                      string rowName = "");
  virtual void setObjSense(int s);
  virtual void solve();
  virtual void setVarLB(int iVar, double lb);
  virtual void setVarUB(int iVar, double ub);
  virtual void setVarBounds(int iVar, double lb, double ub);
  virtual void addIndicatorConstraint(int iBVar, int bVal, int nnz,
                                      int *rmatind, double *rmatval,
                                      LinConType sense, double rhs,
                                      std::string rowName = "");
  virtual bool addWarmStart(const std::vector<VarId> &vars,
                            const std::vector<double> vals);

  virtual int getNCols() {return variables.size();}
  virtual int getNRows() {return nRows;}
  virtual double getInfBound() { return XPRB_INFINITY; }
  virtual const double *getValues() { return output.x; }
  virtual double getObjValue() { return output.objVal; }
  virtual double getBestBound() { return output.bestBound; }
  virtual double getCPUTime() { return output.dCPUTime; }
  virtual Status getStatus() { return output.status; }
  virtual string getStatusName() { return output.statusName; }
  virtual int getNNodes() { return output.nNodes; }
  virtual int getNOpen() { return output.nOpenNodes; }

  MIP_xpress_wrapper(Options* opt) : options(opt) {
    openXpress();
  };
  virtual ~MIP_xpress_wrapper() {
    closeXpress();
  };

  static std::string getDescription(MiniZinc::SolverInstanceBase::Options* opt=NULL);
  static std::string getVersion(MiniZinc::SolverInstanceBase::Options* opt=NULL);
  static std::string getId(void);
  static std::string getName(void);
  static std::vector<std::string> getTags(void);
  static std::vector<std::string> getStdFlags(void);
  static std::vector<std::string> getRequiredFlags(void);

private:
  XPRBprob problem;
  XPRBctr xpressObj;
  vector<XPRBvar> variables;
  size_t nRows{0};

  void openXpress(void);
  void closeXpress(void);

  void setUserSolutionCallback();
  void setOptions();
  void writeModelIfRequested();
  int convertConstraintType(LinConType sense);
  int convertVariableType(VarType varType);
  int convertObjectiveSense(int s);
  XPRBctr addConstraint(int nnz, int *rmatind, double *rmatval,
                        LinConType sense, double rhs, int mask, string rowName);
  void addDummyConstraint();
};

#endif // __MIP_XPRESS_WRAPPER_H__
