/*
 *  main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* this source code form is subject to the terms of the mozilla public
 * license, v. 2.0. if a copy of the mpl was not distributed with this
 * file, you can obtain one at http://mozilla.org/mpl/2.0/. */

#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#include "minizinc/config.hh"
#include "minizinc/exception.hh"

#include "minizinc/solvers/MIP/MIP_xpress_wrap.hh"
#include "minizinc/utils.hh"

struct UserSolutionCallbackData {
  MIP_wrapper::CBUserInfo *info;
  XPRBprob *problem;
  vector<XPRBvar> *variables;
};

class XpressException : public runtime_error {
public:
  XpressException(string msg) : runtime_error(" MIP_xpress_wrapper: " + msg) {}
};

MIP_wrapper *MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_xpress_wrapper;
}

string MIP_WrapperFactory::getVersion() {
  ostringstream oss;
  oss << "  MIP wrapper for FICO Xpress Optimiser";
  oss << ".  Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}

static int msgLevel = 0;
static int timeout = 0;
static int numSolutions = 0;
static string logFile = "";
static string writeModelFile = "";
static string writeModelFormat = "lp";
static double absGap = 0;
static double relGap = 0.0001;
static bool printAllSolutions = false;

void MIP_WrapperFactory::printHelp(ostream &os) {
  os << "XPRESS MIP wrapper options:" << std::endl
     << "--msgLevel <n>       print solver output, default: " << msgLevel
     << std::endl
     << "--logFile <file>     log file" << std::endl
     << "--timeout <N>        stop search after N seconds, if negative, it "
        "will only stop if at least one solution was found"
     << std::endl
     << "--numSolutions <N>   stop search after N solutions" << std::endl
     << "--writeModel <file>  write model to <file>" << std::endl
     << "--writeModelFormat [lp|mps] the file format of the written model(lp "
        "or mps), default: "
     << writeModelFormat << std::endl
     << "--absGap <d>         absolute gap |primal-dual| to stop, default: "
     << absGap << std::endl
     << "--relGap <d>         relative gap |primal-dual|/<solver-dep> to stop, "
        "default: "
     << relGap << std::endl
     << "--printAllSolutions  print intermediate solution, default: "
     << printAllSolutions << std::endl
     << std::endl;
}

bool MIP_WrapperFactory::processOption(int &i, int argc, const char **argv) {
  MiniZinc::CLOParser cop(i, argc, argv);
  if (cop.get("--msgLevel", &msgLevel)) {
  } else if (cop.get("--logFile", &logFile)) {
  } else if (cop.get("--timeout", &timeout)) {
  } else if (cop.get("--numSolutions", &numSolutions)) {
  } else if (cop.get("--writeModel", &writeModelFile)) {
  } else if (cop.get("--writeModelFormat", &writeModelFormat)) {
  } else if (cop.get("--relGap", &relGap)) {
  } else if (cop.get("--absGap", &absGap)) {
  } else if (string(argv[i]) == "--printAllSolutions") {
    printAllSolutions = true;
  } else
    return false;
  return true;
}

void MIP_xpress_wrapper::setOptions() {
  XPRSprob xprsProblem = problem.getXPRSprob();

  problem.setMsgLevel(msgLevel);

  XPRSsetlogfile(xprsProblem, logFile.c_str());
  XPRSsetintcontrol(xprsProblem, XPRS_MAXTIME, timeout);
  XPRSsetintcontrol(xprsProblem, XPRS_MAXMIPSOL, numSolutions);
  XPRSsetdblcontrol(xprsProblem, XPRS_MIPABSSTOP, absGap);
  XPRSsetdblcontrol(xprsProblem, XPRS_MIPRELSTOP, relGap);
}

static MIP_wrapper::Status convertStatus(int xpressStatus) {
  switch (xpressStatus) {
  case XPRB_MIP_OPTIMAL:
    return MIP_wrapper::Status::OPT;
  case XPRB_MIP_INFEAS:
    return MIP_wrapper::Status::UNSAT;
  case XPRB_MIP_UNBOUNDED:
    return MIP_wrapper::Status::UNBND;
  case XPRB_MIP_NO_SOL_FOUND:
    return MIP_wrapper::Status::UNKNOWN;
  case XPRB_MIP_NOT_LOADED:
    return MIP_wrapper::Status::__ERROR;
  default:
    return MIP_wrapper::Status::UNKNOWN;
  }
}

static string getStatusName(int xpressStatus) {
  string rt = "Xpress stopped with status: ";
  switch (xpressStatus) {
  case XPRB_MIP_OPTIMAL:
    return rt + "Optimal";
  case XPRB_MIP_INFEAS:
    return rt + "Infeasible";
  case XPRB_MIP_UNBOUNDED:
    return rt + "Unbounded";
  case XPRB_MIP_NO_SOL_FOUND:
    return rt + "No solution found";
  case XPRB_MIP_NOT_LOADED:
    return rt + "No problem loaded or error";
  default:
    return rt + "Unknown status";
  }
}

static void setOutputVariables(MIP_xpress_wrapper::Output *output,
                               XPRSprob xprsProblem, XPRBprob *problem,
                               vector<XPRBvar> *variables) {
  problem->beginCB(xprsProblem);
  problem->sync(XPRB_XPRS_SOL);

  size_t nCols = variables->size();
  double *x = (double *)malloc(nCols * sizeof(double));
  for (size_t ii = 0; ii < nCols; ii++) {
    x[ii] = (*variables)[ii].getSol();
  }
  output->x = x;

  problem->endCB();

  int xpressStatus = 0;
  XPRSgetintattrib(xprsProblem, XPRS_MIPSTATUS, &xpressStatus);
  output->status = convertStatus(xpressStatus);
  output->statusName = getStatusName(xpressStatus);

  XPRSgetdblattrib(xprsProblem, XPRS_MIPOBJVAL, &output->objVal);
  XPRSgetdblattrib(xprsProblem, XPRS_BESTBOUND, &output->bestBound);

  XPRSgetintattrib(xprsProblem, XPRS_NODES, &output->nNodes);
  XPRSgetintattrib(xprsProblem, XPRS_ACTIVENODES, &output->nOpenNodes);

  output->dWallTime = std::chrono::duration<double>(
                          std::chrono::steady_clock::now() - output->dWallTime0)
                          .count();
  output->dCPUTime = double(std::clock() - output->cCPUTime0) / CLOCKS_PER_SEC;
}

static void XPRS_CC userSolNotifyCallback(XPRSprob xprsProblem, void *userData,
                                          const char *solname, int status) {
  UserSolutionCallbackData *data = (UserSolutionCallbackData *) userData;
  MIP_wrapper::CBUserInfo *info = data->info;

  if (status == 0) {
    throw XpressException("error while processing solution callback");
  }

  if (status != 1 || status != 2 || status != 3) {
    return;
  }

  setOutputVariables(info->pOutput, xprsProblem, data->problem, data->variables);

  if (info->solcbfn)
    (*info->solcbfn)(*info->pOutput, info->ppp);
}

void MIP_xpress_wrapper::doAddVars(size_t n, double *obj, double *lb,
                                   double *ub, VarType *vt, string *names) {
  if (obj == nullptr || lb == nullptr || ub == nullptr || vt == nullptr ||
      names == nullptr) {
    throw XpressException("invalid input");
  }
  for (size_t i = 0; i < n; ++i) {
    char *var_name = (char *)names[i].c_str();
    int var_type = convertVariableType(vt[i]);
    XPRBvar var = problem.newVar(var_name, var_type, lb[i], ub[i]);
    variables.push_back(var);
    xpressObj.setTerm(obj[i], var);
  }
}

void MIP_xpress_wrapper::addRow(int nnz, int *rmatind, double *rmatval,
                                LinConType sense, double rhs, int mask,
                                string rowName) {
  addConstraint(nnz, rmatind, rmatval, sense, rhs, mask, rowName);
}

XPRBctr MIP_xpress_wrapper::addConstraint(int nnz, int *rmatind,
                                          double *rmatval, LinConType sense,
                                          double rhs, int mask,
                                          string rowName) {
  nRows++;
  XPRBctr constraint = problem.newCtr(rowName.c_str());
  for (int i = 0; i < nnz; ++i) {
    constraint.setTerm(variables[rmatind[i]], rmatval[i]);
  }
  constraint.setTerm(rhs);

  if (constraint.setType(convertConstraintType(sense)) == 1) {
    throw XpressException("error while setting sense of constraint");
  }

  return constraint;
}

void MIP_xpress_wrapper::writeModelIfRequested() {
  int format = XPRB_LP;
  if (writeModelFormat == "lp") {
    format = XPRB_LP;
  } else if (writeModelFormat == "mps") {
    format = XPRB_MPS;
  }
  if (!writeModelFile.empty()) {
    problem.exportProb(format, writeModelFile.c_str());
  }
}

void MIP_xpress_wrapper::addDummyConstraint() {
  if (getNCols() == 0) {
    return;
  }

  XPRBctr constraint = problem.newCtr("dummy_constraint");
  constraint.setTerm(variables[0], 1);
  constraint.setType(convertConstraintType(LinConType::LQ));
  constraint.setTerm(variables[0].getUB());
}

void MIP_xpress_wrapper::solve() {
  if (getNRows() == 0) {
    addDummyConstraint();
  }

  setOptions();
  writeModelIfRequested();
  setUserSolutionCallback();

  problem.setObj(xpressObj);

  cbui.pOutput->dWallTime0 = output.dWallTime0 =
      std::chrono::steady_clock::now();
  cbui.pOutput->cCPUTime0 = output.dCPUTime = std::clock();

  if (problem.mipOptimize("c") == 1) {
    throw XpressException("error while solving");
  }

  setOutputVariables(&output, problem.getXPRSprob(), &problem, &variables);
}

void MIP_xpress_wrapper::setUserSolutionCallback() {
  if (!printAllSolutions) {
    return;
  }

  UserSolutionCallbackData data = {&cbui, &problem, &variables};

  XPRSaddcbusersolnotify(problem.getXPRSprob(), userSolNotifyCallback,
                         (void *)&data, 0);
}

void MIP_xpress_wrapper::setObjSense(int s) {
  problem.setSense(convertObjectiveSense(s));
}

void MIP_xpress_wrapper::setVarLB(int iVar, double lb) {
  variables[iVar].setLB(lb);
}

void MIP_xpress_wrapper::setVarUB(int iVar, double ub) {
  variables[iVar].setUB(ub);
}

void MIP_xpress_wrapper::setVarBounds(int iVar, double lb, double ub) {
  setVarLB(iVar, lb);
  setVarUB(iVar, ub);
}

void MIP_xpress_wrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz,
                                                int *rmatind, double *rmatval,
                                                LinConType sense, double rhs,
                                                string rowName) {
  if (bVal != 0 && bVal != 1) {
    throw XpressException("indicator bval not in 0/1");
  }
  XPRBctr constraint =
      addConstraint(nnz, rmatind, rmatval, sense, rhs, 0, rowName);
  constraint.setIndicator(2 * bVal - 1, variables[iBVar]);
}

bool MIP_xpress_wrapper::addWarmStart(const std::vector<VarId> &vars,
                                      const std::vector<double> vals) {
  XPRBsol warmstart = problem.newSol();
  for (size_t ii = 0; ii < vars.size(); ii++) {
    warmstart.setVar(variables[vars[ii]], vals[ii]);
  }
  return 1 - problem.addMIPSol(warmstart);
}

int MIP_xpress_wrapper::convertConstraintType(LinConType sense) {
  switch (sense) {
  case MIP_wrapper::LQ:
    return XPRB_L;
  case MIP_wrapper::EQ:
    return XPRB_E;
  case MIP_wrapper::GQ:
    return XPRB_G;
  default:
    throw XpressException("unkown constraint sense");
  }
}

int MIP_xpress_wrapper::convertVariableType(VarType varType) {
  switch (varType) {
  case REAL:
    return XPRB_PL;
  case INT:
    return XPRB_UI;
  case BINARY:
    return XPRB_BV;
  default:
    throw XpressException("unknown variable type");
  }
}

int MIP_xpress_wrapper::convertObjectiveSense(int s) {
  switch (s) {
  case 1:
    return XPRB_MAXIM;
  case -1:
    return XPRB_MINIM;
  default:
    throw XpressException("unknown objective sense");
  }
}
