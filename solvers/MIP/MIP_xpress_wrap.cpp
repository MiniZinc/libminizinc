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
  XpressPlugin* plugin;
};

class XpressException : public runtime_error {
public:
  XpressException(string msg) : runtime_error(" MIP_xpress_wrapper: " + msg) {}
};

XprlPlugin::XprlPlugin() : Plugin(XprlPlugin::dlls()) {}

XprlPlugin::XprlPlugin(const std::string& dll_file) : Plugin(dll_file) {}

const std::vector<std::string>& XprlPlugin::dlls() {
  static std::vector<std::string> ret = {
#ifdef _WIN32
    "xprl",
    "C:\\xpressmp\\bin\\xprl.dll"
#elif __APPLE__
    "libxprl",
    " /Applications/FICO Xpress/xpressmp/lib/libxprl.dylib"
#else
    "libxprl",
    "/opt/xpressmp/lib/libxprl.so"
#endif
  };
  return ret;
}

XpressPlugin::XpressPlugin() : Plugin(XpressPlugin::dlls()) {
  load_dll();
}

XpressPlugin::XpressPlugin(const std::string& dll_file) : Plugin(dll_file) {
  load_dll();
}

void XpressPlugin::load_dll(void) {
  load_symbol(XPRSgetversion);
  load_symbol(XPRBgetXPRSprob);
  load_symbol(XPRBsetmsglevel);
  load_symbol(XPRSsetlogfile);
  load_symbol(XPRSsetintcontrol);
  load_symbol(XPRSsetdblcontrol);
  load_symbol(XPRBgetsol);
  load_symbol(XPRSgetintattrib);
  load_symbol(XPRSgetdblattrib);
  load_symbol(XPRBbegincb);
  load_symbol(XPRBsync);
  load_symbol(XPRBendcb);
  load_symbol(XPRBsetterm);
  load_symbol(XPRBnewvar);
  load_symbol(XPRBnewctr);
  load_symbol(XPRBsetctrtype);
  load_symbol(XPRBexportprob);
  load_symbol(XPRBgetbounds);
  load_symbol(XPRBsetobj);
  load_symbol(XPRBmipoptimize);
  load_symbol(XPRBsetsense);
  load_symbol(XPRSsetcbintsol);
  load_symbol(XPRBsetub);
  load_symbol(XPRBsetlb);
  load_symbol(XPRBsetindicator);
  load_symbol(XPRBnewsol);
  load_symbol(XPRBsetsolvar);
  load_symbol(XPRBaddmipsol);
  load_symbol(XPRBnewprob);
}

const std::vector<std::string>& XpressPlugin::dlls() {
  static std::vector<std::string> ret = {
#ifdef _WIN32
    "xprs",
    "C:\\xpressmp\\bin\\xprs.dll"
#elif __APPLE__
    "libxprs"
    " /Applications/FICO Xpress/xpressmp/lib/libxprs.dylib"
#else
    "libxprs",
    "/opt/xpressmp/lib/libxprs.so"
#endif
  };
  return ret;
}

void MIP_xpress_wrapper::openXpress(void) {
  if (options->xprlFile.size() && options->xprsFile.size()) {
    plugin_dep = new XprlPlugin(options->xprlFile);
    plugin = new XpressPlugin(options->xprsFile);
  }
  else {
    plugin_dep = new XprlPlugin();
    plugin = new XpressPlugin();
  }

  problem = plugin->XPRBnewprob(nullptr);
  xpressObj = plugin->XPRBnewctr(problem, nullptr, XB_N);
}

void MIP_xpress_wrapper::closeXpress(void) {
  delete plugin;
  delete plugin_dep;
}

string MIP_xpress_wrapper::getDescription(MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "  MIP wrapper for FICO Xpress Optimiser version " << getVersion(opt);
  oss << ".  Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}

string MIP_xpress_wrapper::getVersion(MiniZinc::SolverInstanceBase::Options* opt) {
  try {
    XprlPlugin p1;
    XpressPlugin p2;
    char v[16];
    p2.XPRSgetversion(v);
    return v;
  } catch (MiniZinc::Plugin::PluginError&) {
    return "<unknown version>";
  }
}

vector<string> MIP_xpress_wrapper::getRequiredFlags(void) {
  try {
    XpressPlugin p;
    return { "" };
  } catch (MiniZinc::Plugin::PluginError&) {
    return { "--xprl-dll", "--xprs-dll" };
  }
}

string MIP_xpress_wrapper::getId() {
  return "xpress";
}

string MIP_xpress_wrapper::getName() {
  return "Xpress";
}

vector<string> MIP_xpress_wrapper::getTags() {
  return {"mip","float","api"};
}

vector<string> MIP_xpress_wrapper::getStdFlags() {
  return {"-a", "-n", "-s"};
}

void MIP_xpress_wrapper::Options::printHelp(ostream &os) {
  os << "XPRESS MIP wrapper options:" << std::endl
     << "--msgLevel <n>       print solver output, default: 0"
     << std::endl
     << "--logFile <file>     log file" << std::endl
     << "--solver-time-limit <N>        stop search after N milliseconds, if negative, it "
        "will only stop if at least one solution was found"
     << std::endl
     << "-n <N>, --numSolutions <N>   stop search after N solutions" << std::endl
     << "--writeModel <file>  write model to <file>" << std::endl
     << "--writeModelFormat [lp|mps] the file format of the written model(lp "
        "or mps), default: lp"
     << std::endl
     << "--absGap <d>         absolute gap |primal-dual| to stop, default: "
     << 0 << std::endl
     << "--relGap <d>         relative gap |primal-dual|/<solver-dep> to stop, "
        "default: "
     << 0.0001 << std::endl
     << "-a, --printAllSolutions  print intermediate solution, default: false"
     << std::endl
     << "--xprl-dll <file>    path to xprl shared library (must also specify --xprs-dll)" << std::endl
     << "--xprs-dll <file>    path to xprs shared library (must also specify --xprl-dll)" << std::endl
     << std::endl;
}

bool MIP_xpress_wrapper::Options::processOption(int &i, std::vector<std::string>& argv) {
  MiniZinc::CLOParser cop(i, argv);
  if (cop.get("--msgLevel", &msgLevel)) {
  } else if (cop.get("--logFile", &logFile)) {
  } else if (cop.get("--solver-time-limit", &timeout)) {
  } else if (cop.get("-n --numSolutions", &numSolutions)) {
  } else if (cop.get("--writeModel", &writeModelFile)) {
  } else if (cop.get("--writeModelFormat", &writeModelFormat)) {
  } else if (cop.get("--relGap", &relGap)) {
  } else if (cop.get("--absGap", &absGap)) {
  } else if (string(argv[i]) == "--printAllSolutions" ||
             string(argv[i]) == "-a") {
    printAllSolutions = true;
  } else if (cop.get("--xprs-dll", &xprsFile)) {
  } else if (cop.get("--xprl-dll", &xprlFile)) {
  } else
    return false;
  return true;
}

void MIP_xpress_wrapper::setOptions() {
  XPRSprob xprsProblem = plugin->XPRBgetXPRSprob(problem);

  plugin->XPRBsetmsglevel(problem, options->msgLevel);

  plugin->XPRSsetlogfile(xprsProblem, options->logFile.c_str());
  if (options->timeout > 1000 || options->timeout < -1000) {
    plugin->XPRSsetintcontrol(xprsProblem, XPRS_MAXTIME, static_cast<int>(options->timeout / 1000));
  }
  plugin->XPRSsetintcontrol(xprsProblem, XPRS_MAXMIPSOL, options->numSolutions);
  plugin->XPRSsetdblcontrol(xprsProblem, XPRS_MIPABSSTOP, options->absGap);
  plugin->XPRSsetdblcontrol(xprsProblem, XPRS_MIPRELSTOP, options->relGap);
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

static void setOutputVariables(XpressPlugin* plugin, MIP_xpress_wrapper::Output *output, vector<XPRBvar> *variables) {
  size_t nCols = variables->size();
  double *x = (double *)malloc(nCols * sizeof(double));
  for (size_t ii = 0; ii < nCols; ii++) {
    x[ii] = plugin->XPRBgetsol((*variables)[ii]);
  }
  output->x = x;
}

static void setOutputAttributes(XpressPlugin* plugin, MIP_xpress_wrapper::Output *output, XPRSprob xprsProblem) {
  int xpressStatus = 0;
  plugin->XPRSgetintattrib(xprsProblem, XPRS_MIPSTATUS, &xpressStatus);
  output->status = convertStatus(xpressStatus);
  output->statusName = getStatusName(xpressStatus);

  plugin->XPRSgetdblattrib(xprsProblem, XPRS_MIPOBJVAL, &output->objVal);
  plugin->XPRSgetdblattrib(xprsProblem, XPRS_BESTBOUND, &output->bestBound);

  plugin->XPRSgetintattrib(xprsProblem, XPRS_NODES, &output->nNodes);
  plugin->XPRSgetintattrib(xprsProblem, XPRS_ACTIVENODES, &output->nOpenNodes);

  output->dWallTime = std::chrono::duration<double>(
                          std::chrono::steady_clock::now() - output->dWallTime0)
                          .count();
  output->dCPUTime = double(std::clock() - output->cCPUTime0) / CLOCKS_PER_SEC;
}

static void XPRS_CC userSolNotifyCallback(XPRSprob xprsProblem,
                                          void *userData) {
  UserSolutionCallbackData *data = (UserSolutionCallbackData *)userData;
  MIP_wrapper::CBUserInfo *info = data->info;

  setOutputAttributes(data->plugin, info->pOutput, xprsProblem);

  data->plugin->XPRBbegincb(*(data->problem), xprsProblem);
  data->plugin->XPRBsync(*(data->problem), XPRB_XPRS_SOL);
  setOutputVariables(data->plugin, info->pOutput, data->variables);
  data->plugin->XPRBendcb(*(data->problem));

  if (info->solcbfn) {
    (*info->solcbfn)(*info->pOutput, info->psi);
  }
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
    XPRBvar var = plugin->XPRBnewvar(problem, var_type, var_name, lb[i], ub[i]);
    variables.push_back(var);
    plugin->XPRBsetterm(xpressObj, var, obj[i]);
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
  XPRBctr constraint = plugin->XPRBnewctr(problem, rowName.c_str(), convertConstraintType(sense));
  for (int i = 0; i < nnz; ++i) {
    plugin->XPRBsetterm(constraint, variables[rmatind[i]], rmatval[i]);
  }
  plugin->XPRBsetterm(constraint, nullptr, rhs);

  return constraint;
}

void MIP_xpress_wrapper::writeModelIfRequested() {
  int format = XPRB_LP;
  if (options->writeModelFormat == "lp") {
    format = XPRB_LP;
  } else if (options->writeModelFormat == "mps") {
    format = XPRB_MPS;
  }
  if (!options->writeModelFile.empty()) {
    plugin->XPRBexportprob(problem, format, options->writeModelFile.c_str());
  }
}

void MIP_xpress_wrapper::addDummyConstraint() {
  if (getNCols() == 0) {
    return;
  }

  XPRBctr constraint = plugin->XPRBnewctr(problem, "dummy_constraint", XPRB_L);
  plugin->XPRBsetterm(constraint, variables[0], 1);
  double ub;
  plugin->XPRBgetbounds(variables[0], NULL, &ub);
  plugin->XPRBsetterm(constraint, nullptr, ub);
}

void MIP_xpress_wrapper::solve() {
  if (getNRows() == 0) {
    addDummyConstraint();
  }

  setOptions();
  writeModelIfRequested();
  setUserSolutionCallback();

  plugin->XPRBsetobj(problem, xpressObj);

  cbui.pOutput->dWallTime0 = output.dWallTime0 =
      std::chrono::steady_clock::now();
  cbui.pOutput->cCPUTime0 = output.dCPUTime = std::clock();

  if (plugin->XPRBmipoptimize(problem, "c") == 1) {
    throw XpressException("error while solving");
  }

  setOutputVariables(plugin, &output, &variables);
  setOutputAttributes(plugin, &output, plugin->XPRBgetXPRSprob(problem));

  if ( !options->printAllSolutions && cbui.solcbfn) {
    cbui.solcbfn(output, cbui.psi);
  }
}

void MIP_xpress_wrapper::setUserSolutionCallback() {
  if (!options->printAllSolutions) {
    return;
  }

  UserSolutionCallbackData *data =
      new UserSolutionCallbackData{&cbui, &problem, &variables, plugin};

  plugin->XPRSsetcbintsol(plugin->XPRBgetXPRSprob(problem), userSolNotifyCallback, data);
}

void MIP_xpress_wrapper::setObjSense(int s) {
  plugin->XPRBsetsense(problem, convertObjectiveSense(s));
}

void MIP_xpress_wrapper::setVarLB(int iVar, double lb) {
  plugin->XPRBsetlb(variables[iVar], lb);
}

void MIP_xpress_wrapper::setVarUB(int iVar, double ub) {
  plugin->XPRBsetub(variables[iVar], ub);
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
  plugin->XPRBsetindicator(constraint, 2 * bVal - 1, variables[iBVar]);
}

bool MIP_xpress_wrapper::addWarmStart(const std::vector<VarId> &vars,
                                      const std::vector<double> vals) {
  XPRBsol warmstart = plugin->XPRBnewsol(problem);
  for (size_t ii = 0; ii < vars.size(); ii++) {
    plugin->XPRBsetsolvar(warmstart, variables[vars[ii]], vals[ii]);
  }
  return 1 - plugin->XPRBaddmipsol(problem, warmstart, nullptr);
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
