// * -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/solvers/MIP/MIP_scip_wrap.hh>
#include <minizinc/utils.hh>

#include <array>
#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

using namespace std;

/// Load SCIP DLL with the given path
ScipPlugin::ScipPlugin(const std::string& dll) : Plugin(dll) { load(); }

/// Load SCIP DLL with default search paths on Windows
ScipPlugin::ScipPlugin()
    : Plugin(
#ifdef _WIN32
          {
            "libscip", "scip", "C:\\Program Files\\SCIPOptSuite 7.0.1\\bin\\libscip.dll",
                "C:\\Program Files\\SCIPOptSuite 7.0.0\\bin\\libscip.dll",
                "C:\\Program Files\\SCIPOptSuite 6.0.2\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 6.0.1\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 6.0.0\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 5.0.1\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 5.0.0\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 4.0.1\\bin\\scip.dll",
                "C:\\Program Files\\SCIPOptSuite 4.0.0\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 7.0.1\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 7.0.0\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 6.0.2\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 6.0.1\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 6.0.0\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 5.0.1\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 5.0.0\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 4.0.1\\bin\\scip.dll",
                "C:\\Program Files (x86)\\SCIPOptSuite 4.0.0\\bin\\scip.dll",
          }
#else
          "libscip"
#endif
      ) {
  load();
}

void ScipPlugin::load() {
  load_symbol(SCIPmajorVersion);
  load_symbol(SCIPminorVersion);
  load_symbol(SCIPtechVersion);
  load_symbol(SCIPsubversion);
  load_symbol(SCIPprintError);
  load_symbol(SCIPcreate);
  load_symbol(SCIPincludeDefaultPlugins);
  load_symbol(SCIPcreateProbBasic);
  load_symbol(SCIPfree);
  load_symbol(SCIPcreateVarBasic);
  load_symbol(SCIPaddVar);
  load_symbol(SCIPreleaseVar);
#ifndef NDEBUG
  load_symbol(SCIPinfinity);
#endif
  load_symbol(SCIPcreateConsBasicLinear);
  load_symbol(SCIPcreateConsBasicQuadratic);
  load_symbol(SCIPaddCons);
  load_symbol(SCIPreleaseCons);
  load_symbol(SCIPchgVarLbGlobal);
  load_symbol(SCIPchgVarUbGlobal);
  load_symbol(SCIPgetNegatedVar);
  load_symbol(SCIPcreateConsBasicIndicator);
  load_symbol(SCIPcreateConsBasicBounddisjunction);
  load_symbol(SCIPcreateConsBasicCumulative);
  load_symbol(SCIPgetNSolsFound);
  load_symbol(SCIPgetNSols);
  load_symbol(SCIPsetIntParam);
  load_symbol(SCIPsetRealParam);
  load_symbol(SCIPwriteOrigProblem);
  load_symbol(SCIPsetMessagehdlrQuiet);
  load_symbol(SCIPmessagehdlrCreate);
  load_symbol(SCIPsetMessagehdlr);
  load_symbol(SCIPreadParams);
  load_symbol(SCIPwriteParams);
  load_symbol(SCIPsolve);
  load_symbol(SCIPgetStatus);
  load_symbol(SCIPgetPrimalbound);
  load_symbol(SCIPgetDualbound);
  load_symbol(SCIPgetSolVals);
  load_symbol(SCIPgetBestSol);
  load_symbol(SCIPgetNNodes);
  load_symbol(SCIPgetNNodesLeft);
  load_symbol(SCIPfreeTransform);
  load_symbol(SCIPsetObjsense);
  load_symbol(SCIPeventhdlrGetName);
  load_symbol(SCIPcatchEvent);
  load_symbol(SCIPdropEvent);
  load_symbol(SCIPeventGetType);
  load_symbol(SCIPgetSolOrigObj);
  load_symbol(SCIPincludeEventhdlrBasic);
  load_symbol(SCIPsetEventhdlrInit);
  load_symbol(SCIPsetEventhdlrExit);
  load_symbol(SCIPmessagePrintErrorHeader);
  load_symbol(SCIPmessagePrintError);
  load_symbol(SCIPgetNVars);
  load_symbol(SCIPgetNConss);
}

#define SCIP_PLUGIN_CALL_R(plugin, x)                                         \
  {                                                                           \
    SCIP_RETCODE _ret = (x);                                                  \
    if (_ret != SCIP_OKAY) {                                                  \
      (plugin)->SCIPmessagePrintErrorHeader(__FILE__, __LINE__);              \
      (plugin)->SCIPmessagePrintError("Error <%d> in function call\n", _ret); \
      return _ret;                                                            \
    }                                                                         \
  }

string MIPScipWrapper::getDescription(MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "MIP wrapper for SCIP " << getVersion(opt) << ". Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}
string MIPScipWrapper::getVersion(MiniZinc::SolverInstanceBase::Options* opt) {
  try {
    ScipPlugin p;
    ostringstream oss;
    oss << p.SCIPmajorVersion() << '.' << p.SCIPminorVersion() << '.' << p.SCIPtechVersion() << '.'
        << p.SCIPsubversion();
    return oss.str();
  } catch (MiniZinc::Plugin::PluginError&) {
    return "<unknown version>";
  }
}
vector<string> MIPScipWrapper::getRequiredFlags() {
  try {
    ScipPlugin p;
    return {};
  } catch (MiniZinc::Plugin::PluginError&) {
    return {"--scip-dll"};
  }
}

string MIPScipWrapper::getId() { return "scip"; }

string MIPScipWrapper::getName() { return "SCIP"; }

vector<string> MIPScipWrapper::getTags() { return {"mip", "float", "api"}; }

vector<string> MIPScipWrapper::getStdFlags() { return {"-i", "-p", "-s"}; }

void MIPScipWrapper::Options::printHelp(ostream& os) {
  os << "SCIP  MIP wrapper options:"
     << std::endl
     // -s                  print statistics
     //            << "  --readParam <file>  read SCIP parameters from file
     //               << "--writeParam <file> write SCIP parameters to file
     //               << "--tuneParam         instruct SCIP to tune parameters instead of solving
     << "--writeModel <file> write model to <file> (.lp, .mps, ...?)" << std::endl
     << "-i                  print intermediate solutions for optimization problems" << std::endl
     << "-p <N>              use N threads, default: 1"
     << std::endl
     //   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
     << "--solver-time-limit <N>       stop search after N milliseconds" << std::endl
     << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
     << "--readParam <file>  read SCIP parameters from file" << std::endl
     << "--writeParam <file> write SCIP parameters to file"
     << std::endl
     //   << "--tuneParam         instruct SCIP to tune parameters instead of solving   NOT IMPL"

     << "--absGap <n>        absolute gap |primal-dual| to stop" << std::endl
     << "--relGap <n>        relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 "
        "to use backend's default"
     << std::endl
     << "--intTol <n>        integrality tolerance for a variable. Default 1e-8"
     << std::endl
     //   << "--objDiff <n>       objective function discretization. Default 1.0" << std::endl
     << "--scip-dll <file>   load the SCIP library from the given file (absolute path or file "
        "basename), default 'scip'"
     << std::endl
     << std::endl;
}

static inline bool beginswith(const string& s, const string& t) {
  return s.compare(0, t.length(), t) == 0;
}

bool MIPScipWrapper::Options::processOption(int& i, vector<string>& argv) {
  MiniZinc::CLOParser cop(i, argv);
  if (cop.get("-i")) {
    flagIntermediate = true;
  } else if (string(argv[i]) == "-f") {  // NOLINT: Allow repeated empty if
    //     std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (cop.get("--writeModel", &sExportModel)) {     // NOLINT: Allow repeated empty if
  } else if (cop.get("-p", &nThreads)) {                   // NOLINT: Allow repeated empty if
  } else if (cop.get("--solver-time-limit", &nTimeout)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--workmem", &nWorkMemLimit)) {       // NOLINT: Allow repeated empty if
  } else if (cop.get("--readParam", &sReadParams)) {       // NOLINT: Allow repeated empty if
  } else if (cop.get("--writeParam", &sWriteParams)) {     // NOLINT: Allow repeated empty if
  } else if (cop.get("--absGap", &absGap)) {               // NOLINT: Allow repeated empty if
  } else if (cop.get("--relGap", &relGap)) {               // NOLINT: Allow repeated empty if
  } else if (cop.get("--intTol", &intTol)) {               // NOLINT: Allow repeated empty if
    //   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else if (cop.get("--scip-dll", &scipDll)) {  // NOLINT: Allow repeated empty if
  } else {
    return false;
  }
  return true;
error:
  return false;
}

// NOLINTNEXTLINE(readability-identifier-naming)
void MIPScipWrapper::SCIP_PLUGIN_CALL(SCIP_RETCODE retcode, const string& msg, bool fTerm) {
  /* evaluate return code of the SCIP process */
  if (retcode != SCIP_OKAY) {
    /* write error back trace */
    _plugin->SCIPprintError(retcode);
    string msgAll = ("  MIPScipWrapper runtime error, see output:  " + msg);
    cerr << msgAll << endl;
    if (fTerm) {
      cerr << "TERMINATING." << endl;
      throw runtime_error(msgAll);
    }
  }
}

SCIP_RETCODE MIPScipWrapper::openSCIP() {
  if (_options->scipDll.empty()) {
    _plugin = new ScipPlugin();
  } else {
    _plugin = new ScipPlugin(_options->scipDll);
  }

  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPcreate(&_scip));
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPincludeDefaultPlugins(_scip));

  /* create empty problem */
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPcreateProbBasic(_scip, "mzn_scip"));
  return SCIP_OKAY;
}

SCIP_RETCODE MIPScipWrapper::closeSCIP() {
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPfree(&_scip));

  delete _plugin;
  /// and at last:
  //   MIPWrapper::cleanup();
  return SCIP_OKAY;
}

SCIP_RETCODE MIPScipWrapper::doAddVarsSCIP(size_t n, double* obj, double* lb, double* ub,
                                           MIPWrapper::VarType* vt, string* names) {
  /// Convert var types:
  //   vector<char> ctype(n);
  //   vector<char*> pcNames(n);
  for (size_t j = 0; j < n; ++j) {
    //     pcNames[i] = (char*)names[i].c_str();
    SCIP_VARTYPE ctype;
    switch (vt[j]) {
      case REAL:
        ctype = SCIP_VARTYPE_CONTINUOUS;
        break;
      case INT:
        ctype = SCIP_VARTYPE_INTEGER;
        break;
      case BINARY:
        ctype = SCIP_VARTYPE_BINARY;
        break;
      default:
        throw runtime_error("  MIPWrapper: unknown variable type");
    }
    _scipVars.resize(_scipVars.size() + 1);
    if (fPhase1Over) {
      assert(_scipVars.size() == colObj.size());
    }
    SCIP_PLUGIN_CALL_R(
        _plugin, _plugin->SCIPcreateVarBasic(_scip, &_scipVars.back(), names[j].c_str(), lb[j],
                                             ub[j], obj[j], ctype));
    SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPaddVar(_scip, _scipVars.back()));
  }
  //   retcode = SCIP_newcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
  //   wrap_assert( !retcode,  "Failed to declare variables." );
  return SCIP_OKAY;
}

SCIP_RETCODE MIPScipWrapper::delSCIPVars() {
  for (auto& v : _scipVars) {
    _plugin->SCIPreleaseVar(_scip, &v);
  }
  return SCIP_OKAY;
}

SCIP_RETCODE MIPScipWrapper::addRowSCIP(int nnz, int* rmatind, double* rmatval,
                                        MIPWrapper::LinConType sense, double rhs, int mask,
                                        const string& rowName) {
  /// Convert var types:
  double lh = -SCIPinfinityPlugin(_plugin, _scip);
  double rh = SCIPinfinityPlugin(_plugin, _scip);
  switch (sense) {
    case LQ:
      rh = rhs;
      break;
    case EQ:
      lh = rh = rhs;
      break;
    case GQ:
      lh = rhs;
      break;
    default:
      throw runtime_error("  MIPWrapper: unknown constraint type");
  }
  const int ccnt = 0;
  const int rcnt = 1;
  const int rmatbeg[] = {0};
  char* pRName = (char*)rowName.c_str();
  // ignoring mask for now.  TODO
  SCIP_CONS* cons;
  vector<SCIP_VAR*> ab(nnz);

  for (int j = 0; j < nnz; ++j) {
    ab[j] = _scipVars[rmatind[j]];
  }

  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPcreateConsBasicLinear(_scip, &cons, rowName.c_str(), nnz,
                                                                 &ab[0], rmatval, lh, rh));
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPaddCons(_scip, cons));
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPreleaseCons(_scip, &cons));
  return SCIP_OKAY;
  //   retcode = SCIP_addrows (env, lp, ccnt, rcnt, nnz, &rhs,
  //         &ssense, rmatbeg, rmatind, rmatval,
  //         nullptr, &pRName);
  //   wrap_assert( !retcode,  "Failed to add constraint." );
}

void MIPScipWrapper::setVarBounds(int iVar, double lb, double ub) {
  SCIP_PLUGIN_CALL(lb <= ub ? SCIP_OKAY : SCIP_ERROR, "scip interface: setVarBounds: lb>ub");
  setVarLB(iVar, lb);
  setVarUB(iVar, ub);
}

void MIPScipWrapper::setVarLB(int iVar, double lb) {
  auto res = _plugin->SCIPchgVarLbGlobal(_scip, _scipVars[iVar], lb);
  SCIP_PLUGIN_CALL(res, "scip interface: failed to set var lb.");
}

void MIPScipWrapper::setVarUB(int iVar, double ub) {
  auto res = _plugin->SCIPchgVarUbGlobal(_scip, _scipVars[iVar], ub);
  SCIP_PLUGIN_CALL(res, "scip interface: failed to set var ub.");
}

void MIPScipWrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind,
                                            double* rmatval, MIPWrapper::LinConType sense,
                                            double rhs, const string& rowName) {
  MZN_ASSERT_HARD_MSG(0 <= bVal && 1 >= bVal, "SCIP: addIndicatorConstraint: bVal not 0/1");
  //// Make sure in order to notice the indices of lazy constr: also here?   TODO
  //  ++ nRows;

  SCIP_CONS* cons;
  vector<SCIP_VAR*> ab(nnz);
  SCIP_VAR*
      indicator_var;  // SCIP 6.0.1 requires that the implication is active for indicator_x == 1

  for (int j = 0; j < nnz; ++j) {
    ab[j] = _scipVars[rmatind[j]];
  }

  indicator_var = _scipVars[iBVar];
  if (0 == bVal) {
    SCIP_PLUGIN_CALL(_plugin->SCIPgetNegatedVar(_scip, indicator_var, &indicator_var));
  }

  if (LQ == sense || EQ == sense) {
    SCIP_PLUGIN_CALL(_plugin->SCIPcreateConsBasicIndicator(
        _scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatval, rhs));
    SCIP_PLUGIN_CALL(_plugin->SCIPaddCons(_scip, cons));
    SCIP_PLUGIN_CALL(_plugin->SCIPreleaseCons(_scip, &cons));
  }
  if (GQ == sense || EQ == sense) {
    std::vector<double> rmatvalNEG(nnz);
    for (int i = nnz; (i--) != 0;) {
      rmatvalNEG[i] = -rmatval[i];
    }
    SCIP_PLUGIN_CALL(_plugin->SCIPcreateConsBasicIndicator(
        _scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatvalNEG.data(), -rhs));
    SCIP_PLUGIN_CALL(_plugin->SCIPaddCons(_scip, cons));
    SCIP_PLUGIN_CALL(_plugin->SCIPreleaseCons(_scip, &cons));
  }
}

void MIPScipWrapper::addBoundsDisj(int n, double* fUB, double* bnd, int* vars, int nF, double* fUBF,
                                   double* bndF, int* varsF, const string& rowName) {
  SCIP_CONS* cons;
  std::vector<SCIP_VAR*> v(n + nF);
  std::vector<SCIP_BOUNDTYPE> bt(n + nF);
  std::vector<SCIP_Real> bs(n + nF);

  for (int j = 0; j < n; ++j) {
    v[j] = _scipVars[vars[j]];
    bt[j] = (fUB[j] != 0.0) ? SCIP_BOUNDTYPE_UPPER : SCIP_BOUNDTYPE_LOWER;
    bs[j] = bnd[j];
  }
  for (int j = 0; j < nF; ++j) {
    v[n + j] = _scipVars[varsF[j]];
    bt[n + j] = (fUBF[j] != 0.0) ? SCIP_BOUNDTYPE_UPPER : SCIP_BOUNDTYPE_LOWER;
    bs[n + j] = bndF[j];
  }

  SCIP_PLUGIN_CALL(_plugin->SCIPcreateConsBasicBounddisjunction(
      _scip, &cons, rowName.c_str(), v.size(), v.data(), bt.data(), bs.data()));
  SCIP_PLUGIN_CALL(_plugin->SCIPaddCons(_scip, cons));
  SCIP_PLUGIN_CALL(_plugin->SCIPreleaseCons(_scip, &cons));
}

void MIPScipWrapper::addCumulative(int nnz, int* rmatind, double* d, double* r, double b,
                                   const string& rowName) {
  SCIP_CONS* cons;
  vector<SCIP_VAR*> ab(nnz);
  vector<int> nd(nnz);
  vector<int> nr(nnz);

  for (int j = 0; j < nnz; ++j) {
    ab[j] = _scipVars[rmatind[j]];
    nd[j] = (int)round(d[j]);
    nr[j] = (int)round(r[j]);
  }

  SCIP_PLUGIN_CALL(_plugin->SCIPcreateConsBasicCumulative(
      _scip, &cons, rowName.c_str(), nnz, ab.data(), nd.data(), nr.data(), (int)round(b)));

  SCIP_PLUGIN_CALL(_plugin->SCIPaddCons(_scip, cons));
  SCIP_PLUGIN_CALL(_plugin->SCIPreleaseCons(_scip, &cons));
}

void MIPScipWrapper::addTimes(int x, int y, int z, const string& rowName) {
  /// As x*y - z == 0
  double zCoef = -1.0;
  double xyCoef = 1.0;
  SCIP_CONS* cons;
  std::array<SCIP_VAR*, 3> zxy = {_scipVars[z], _scipVars[x], _scipVars[y]};

  SCIP_PLUGIN_CALL(_plugin->SCIPcreateConsBasicQuadratic(
      _scip, &cons, rowName.c_str(), 1, &zxy[0], &zCoef, 1, &zxy[1], &zxy[2], &xyCoef, 0.0, 0.0));
  SCIP_PLUGIN_CALL(_plugin->SCIPaddCons(_scip, cons));
  SCIP_PLUGIN_CALL(_plugin->SCIPreleaseCons(_scip, &cons));
}

/// SolutionCallback ------------------------------------------------------------------------

/// From event_bestsol.c:
#define EVENTHDLR_NAME "bestsol"
#define EVENTHDLR_DESC "event handler for best solutions found"

// Dirty way of accessing SCIP functions inside C callbacks
static ScipPlugin* _cb_plugin;

/** initialization method of event handler (called after problem was transformed) */
static SCIP_DECL_EVENTINIT(eventInitBestsol) { /*lint --e{715}*/
  assert(scip != nullptr);
  assert(eventhdlr != nullptr);
  assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);

  /* notify SCIP that your event handler wants to react on the event type best solution found */
  SCIP_PLUGIN_CALL_R(_cb_plugin, _cb_plugin->SCIPcatchEvent(scip, SCIP_EVENTTYPE_BESTSOLFOUND,
                                                            eventhdlr, nullptr, nullptr));

  return SCIP_OKAY;
}

/** deinitialization method of event handler (called before transformed problem is freed) */
static SCIP_DECL_EVENTEXIT(eventExitBestsol) { /*lint --e{715}*/
  assert(scip != nullptr);
  assert(eventhdlr != nullptr);
  assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);

  /* notify SCIP that your event handler wants to drop the event type best solution found */
  SCIP_PLUGIN_CALL_R(_cb_plugin, _cb_plugin->SCIPdropEvent(scip, SCIP_EVENTTYPE_BESTSOLFOUND,
                                                           eventhdlr, nullptr, -1));

  return SCIP_OKAY;
}

static MIPWrapper::CBUserInfo* cbuiPtr = nullptr;
static SCIP_VAR** _scipVarsPtr = nullptr;

/** execution method of event handler */
static SCIP_DECL_EVENTEXEC(eventExecBestsol) { /*lint --e{715}*/
  SCIP_SOL* bestsol;
  SCIP_Real objVal;
  int newincumbent = 0;

  assert(eventhdlr != nullptr);
  assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);
  assert(event != nullptr);
  assert(scip != nullptr);
  assert(_cb_plugin->SCIPeventGetType(event) == SCIP_EVENTTYPE_BESTSOLFOUND);

  SCIPdebugMessage("exec method of event handler for best solution found\n");

  bestsol = _cb_plugin->SCIPgetBestSol(scip);
  assert(bestsol != nullptr);
  objVal = _cb_plugin->SCIPgetSolOrigObj(scip, bestsol);

  if (cbuiPtr == nullptr) {
    return SCIP_OKAY;
  }

  if (fabs(cbuiPtr->pOutput->objVal - objVal) > 1e-12 * (1.0 + fabs(objVal))) {
    newincumbent = 1;
    cbuiPtr->pOutput->objVal = objVal;
    cbuiPtr->pOutput->status = MIPWrapper::SAT;
    cbuiPtr->pOutput->statusName = "feasible from a callback";
  }

  if (newincumbent != 0 && _scipVarsPtr != nullptr) {
    assert(cbuiPtr->pOutput->x);
    SCIP_PLUGIN_CALL_R(
        _cb_plugin, _cb_plugin->SCIPgetSolVals(scip, bestsol, cbuiPtr->pOutput->nCols, _scipVarsPtr,
                                               (double*)cbuiPtr->pOutput->x));
    //       wrap_assert(!retcode, "Failed to get variable values.");
    cbuiPtr->pOutput->nNodes = static_cast<int>(_cb_plugin->SCIPgetNNodes(scip));
    cbuiPtr->pOutput->nOpenNodes = _cb_plugin->SCIPgetNNodesLeft(scip);
    cbuiPtr->pOutput->bestBound = _cb_plugin->SCIPgetDualbound(scip);

    cbuiPtr->pOutput->dCPUTime = -1;

    /// Call the user function:
    if (cbuiPtr->solcbfn != nullptr) {
      (*cbuiPtr->solcbfn)(*cbuiPtr->pOutput, cbuiPtr->psi);
    }
  }

  return SCIP_OKAY;
}

/** includes event handler for best solution found */
SCIP_RETCODE MIPScipWrapper::includeEventHdlrBestsol() {
  SCIP_EVENTHDLRDATA* eventhdlrdata;
  SCIP_EVENTHDLR* eventhdlr;
  eventhdlrdata = nullptr;

  eventhdlr = nullptr;

  _cb_plugin = _plugin;  // So that callbacks can access plugin functions

  /* create event handler for events on watched variables */
  SCIP_PLUGIN_CALL_R(
      _plugin, _plugin->SCIPincludeEventhdlrBasic(_scip, &eventhdlr, EVENTHDLR_NAME, EVENTHDLR_DESC,
                                                  eventExecBestsol, eventhdlrdata));
  assert(eventhdlr != nullptr);

  /// Not for sub-SCIPs
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetEventhdlrInit(_scip, eventhdlr, eventInitBestsol));
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetEventhdlrExit(_scip, eventhdlr, eventExitBestsol));

  return SCIP_OKAY;
}

MIPScipWrapper::Status MIPScipWrapper::convertStatus(SCIP_STATUS scipStatus) {
  Status s = Status::UNKNOWN;
  /* Converting the status. */
  switch (scipStatus) {
    case SCIP_STATUS_OPTIMAL:
      s = Status::OPT;
      output.statusName = "Optimal";
      assert(_plugin->SCIPgetNSolsFound(_scip));
      break;
    case SCIP_STATUS_INFEASIBLE:
      s = Status::UNSAT;
      output.statusName = "Infeasible";
      break;
      //      case SCIP_MIP_OPTIMAL_INFEAS:
    case SCIP_STATUS_INFORUNBD:
      s = Status::UNSATorUNBND;
      output.statusName = "Infeasible or unbounded";
      break;
      //      case SCIP_MIP_SOL_LIM:
      //        s = Status::SAT;
      //        wrap_assert(SCIP_getsolnpoolnumsolns(env, lp), "Feasibility reported but pool
      //        empty?", false); break;
    case SCIP_STATUS_UNBOUNDED:
      s = Status::UNBND;
      output.statusName = "Unbounded";
      break;
      //      case SCIP_STATUSMIP_ABORT_INFEAS:
      //      case SCIP_MIP_FAIL_INFEAS:
      //        s = Status::ERROR;
      //        break;
    default:
      //      case SCIP_MIP_OPTIMAL_TOL:
      //      case SCIP_MIP_ABORT_RELAXATION_UNBOUNDED:
      if (_plugin->SCIPgetNSols(_scip) != 0) {
        s = Status::SAT;
        output.statusName = "Feasible";
      } else {
        s = Status::UNKNOWN;
        output.statusName = "Unknown";
      }
  }
  return s;
}

SCIP_DECL_MESSAGEWARNING(printMsg) { cerr << msg << flush; }

SCIP_RETCODE MIPScipWrapper::solveSCIP() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  if (_options->nThreads > 0)
    SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetIntParam(_scip, "lp/threads", _options->nThreads));

  if (_options->nTimeout > 0)
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPsetRealParam(_scip, "limits/time",
                                                 static_cast<double>(_options->nTimeout) / 1000.0));

  if (_options->nWorkMemLimit > 0)
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPsetRealParam(_scip, "limits/memory", _options->nWorkMemLimit));

  if (_options->absGap >= 0.0)
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPsetRealParam(_scip, "limits/absgap", _options->absGap));
  if (_options->relGap >= 0.0)
    SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetRealParam(_scip, "limits/gap", _options->relGap));
  if (_options->intTol >= 0.0)
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPsetRealParam(_scip, "numerics/feastol", _options->intTol));

  //    retcode =  SCIP_setintparam (env, SCIP_PARAM_ClockType, 1);            // CPU time
  //    wrap_assert(!retcode, "  SCIP Warning: Failure to measure CPU time.", false);

  if (!_options->sExportModel.empty()) {
    //       std::cerr <<"  Exporting LP model to "  << sExportModel << " ..." << std::endl;
    SCIP_PLUGIN_CALL_R(
        _plugin, _plugin->SCIPwriteOrigProblem(_scip, _options->sExportModel.c_str(), nullptr, 0));
  }

  /* Turn on output to the screen  - after model export */
  if (!fVerbose) {
    //       SCIP_PLUGIN_CALL(SCIPsetMessagehdlr(_scip, nullptr));  No LP export then
    _plugin->SCIPsetMessagehdlrQuiet(_scip, TRUE);
  } else {
    SCIP_MESSAGEHDLR* pHndl = nullptr;
    SCIP_PLUGIN_CALL_R(
        _plugin, _plugin->SCIPmessagehdlrCreate(&pHndl, FALSE, nullptr, FALSE, printMsg, printMsg,
                                                printMsg, nullptr, nullptr));
    SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetMessagehdlr(_scip, pHndl));
  }

  //     assert(_scipVars.size() == colObj.size());
  int cur_numcols = _scipVars.size();  // No, we create negated indicators: getNCols();
  assert(cur_numcols == colObj.size());
  assert(cur_numcols == _scipVars.size());

  /// Solution callback
  output.nCols = colObj.size();
  _x.resize(output.nCols);
  output.x = &_x[0];
  if (_options->flagIntermediate && cbui.solcbfn != nullptr && cbuiPtr == nullptr) {
    /* include event handler for best solution found */
    SCIP_PLUGIN_CALL_R(_plugin, includeEventHdlrBestsol());
    cbuiPtr = &cbui;  // not thread-safe...         TODO
    _scipVarsPtr = &_scipVars[0];
    //       retcode = SCIP_setinfocallbackfunc (env, solcallback, &cbui);
    //       wrap_assert(!retcode, "Failed to set solution callback", false);
  }

  if (!_options->sReadParams.empty()) {
    SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPreadParams(_scip, _options->sReadParams.c_str()));
  }

  if (!_options->sWriteParams.empty()) {
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPwriteParams(_scip, _options->sReadParams.c_str(), TRUE, FALSE));
  }

  cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
  output.dCPUTime = clock();

  /* Optimize the problem and obtain solution. */
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsolve(_scip));
  //    wrap_assert( !retcode,  "Failed to optimize MIP." );

  output.dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output.dWallTime0).count();
  output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;

  cbuiPtr = nullptr;  /// cleanup
  _scipVarsPtr = nullptr;

  SCIP_STATUS solstat = _plugin->SCIPgetStatus(_scip);
  output.status = convertStatus(solstat);
  //    output.statusName = SCIP_getstatstring (env, solstat, scip_status_buffer);

  /// Continuing to fill the output object:
  output.objVal = _plugin->SCIPgetPrimalbound(_scip);
  output.bestBound = _plugin->SCIPgetDualbound(_scip);
  //    wrap_assert(!retcode, "Failed to get the best bound.", false);
  if (Status::OPT == output.status || Status::SAT == output.status) {
    //       wrap_assert( !retcode, "No MIP objective value available." );

    _x.resize(cur_numcols);
    output.x = &_x[0];
    SCIP_PLUGIN_CALL_R(_plugin,
                       _plugin->SCIPgetSolVals(_scip, _plugin->SCIPgetBestSol(_scip), cur_numcols,
                                               &_scipVars[0], (double*)output.x));
    if (cbui.solcbfn != nullptr && (!_options->flagIntermediate || !cbui.printed)) {
      cbui.solcbfn(output, cbui.psi);
    }
  }
  output.nNodes = static_cast<int>(_plugin->SCIPgetNNodes(_scip));
  output.nOpenNodes = _plugin->SCIPgetNNodesLeft(_scip);  // SCIP_getnodeleftcnt (env, lp);

  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPfreeTransform(_scip));

  return SCIP_OKAY;
}

SCIP_RETCODE MIPScipWrapper::setObjSenseSCIP(int s) {
  SCIP_PLUGIN_CALL_R(_plugin, _plugin->SCIPsetObjsense(
                                  _scip, s > 0 ? SCIP_OBJSENSE_MAXIMIZE : SCIP_OBJSENSE_MINIMIZE));
  return SCIP_OKAY;
}
