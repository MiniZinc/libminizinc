/*
 *  Main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MIP_XPRESS_WRAPPER_H__
#define __MIP_XPRESS_WRAPPER_H__
#include "minizinc/solvers/MIP/MIP_wrap.hh"

#include "xprb_cpp.h"
#include "xprs.h"

using namespace std;
using namespace dashoptimization;

class MIP_xpress_wrapper : public MIP_wrapper {
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

  virtual ~MIP_xpress_wrapper() {}

private:
  XPRBprob problem{};
  XPRBexpr xpressObj{};
  vector<XPRBvar> variables{};
  size_t nRows{0};

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
