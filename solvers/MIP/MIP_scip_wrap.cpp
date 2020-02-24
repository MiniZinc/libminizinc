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

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <string>
#include <cstring>
#include <cmath>
#include <stdexcept>

using namespace std;

#include <minizinc/solvers/MIP/MIP_scip_wrap.hh>
#include <minizinc/utils.hh>

/// Load SCIP DLL with the given path
ScipPlugin::ScipPlugin(std::string dll) : Plugin(dll) {
  load();
}

/// Load SCIP DLL with default search paths on Windows
ScipPlugin::ScipPlugin() : 
  Plugin(
#ifdef _WIN32
    {
      "scip",
      "C:\\Program Files\\SCIPOptSuite 6.0.2\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 6.0.1\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 6.0.0\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 5.0.1\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 5.0.0\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 4.0.1\\bin\\scip.dll",
      "C:\\Program Files\\SCIPOptSuite 4.0.0\\bin\\scip.dll",
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

#define SCIP_PLUGIN_CALL(plugin, x) { \
  SCIP_RETCODE _ret = (x); \
  if (_ret != SCIP_OKAY) { \
    (plugin)->SCIPmessagePrintErrorHeader(__FILE__, __LINE__); \
    (plugin)->SCIPmessagePrintError("Error <%d> in function call\n", _ret); \
    return _ret; \
  }\
}

std::string MIP_scip_wrapper::getMznLib() { return "-Glinear_scip"; }

string MIP_scip_wrapper::getDescription(MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "MIP wrapper for SCIP "
      << getVersion(opt)
      << ". Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}
string MIP_scip_wrapper::getVersion(MiniZinc::SolverInstanceBase::Options* opt) {
  try {
    ScipPlugin p;
    ostringstream oss;
    oss << p.SCIPmajorVersion() << '.'
      << p.SCIPminorVersion() << '.'
      << p.SCIPtechVersion() << '.'
      << p.SCIPsubversion();
    return oss.str();
  } catch (MiniZinc::Plugin::PluginError&) {
    return "<unknown version>";
  }
}
string MIP_scip_wrapper::needDllFlag( ) {
  try {
    ScipPlugin p;
    return "";
  } catch (MiniZinc::Plugin::PluginError&) {
    return "--scip-dll";
  }
}

string MIP_scip_wrapper::getId() {
  return "scip";
}

string MIP_scip_wrapper::getName() {
  return "SCIP";
}

vector<string> MIP_scip_wrapper::getTags() {
  return {"mip","float","api"};
}

vector<string> MIP_scip_wrapper::getStdFlags() {
  return {"-a", "-p", "-s"};
}

void MIP_scip_wrapper::Options::printHelp(ostream& os) {
  os
  << "SCIP  MIP wrapper options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read SCIP parameters from file
  //               << "--writeParam <file> write SCIP parameters to file
  //               << "--tuneParam         instruct SCIP to tune parameters instead of solving
  << "--writeModel <file> write model to <file> (.lp, .mps, ...?)" << std::endl
  << "-a                  print intermediate solutions (use for optimization problems only TODO)" << std::endl
  << "-p <N>              use N threads, default: 1" << std::endl
//   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--solver-time-limit <N>       stop search after N milliseconds" << std::endl
  << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read SCIP parameters from file" << std::endl
  << "--writeParam <file> write SCIP parameters to file" << std::endl
//   << "--tuneParam         instruct SCIP to tune parameters instead of solving   NOT IMPL"

  << "--absGap <n>        absolute gap |primal-dual| to stop" << std::endl
  << "--relGap <n>        relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 to use backend's default" << std::endl
  << "--intTol <n>        integrality tolerance for a variable. Default 1e-8" << std::endl
//   << "--objDiff <n>       objective function discretization. Default 1.0" << std::endl
  << "--scip-dll <file>   load the SCIP library from the given file (absolute path or file basename), default 'scip'" << std::endl
  << std::endl;
}

  static inline bool beginswith(string s, string t) {
    return s.compare(0, t.length(), t)==0;
  }

bool MIP_scip_wrapper::Options::processOption(int& i, vector<string>& argv) {
  MiniZinc::CLOParser cop( i, argv );
  if ( string(argv[i])=="-a"
      || string(argv[i])=="--all"
      || string(argv[i])=="--all-solutions" ) {
    flag_all_solutions = true;
  } else if (string(argv[i])=="-f") {
//     std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if ( cop.get( "--writeModel", &sExportModel ) ) {
  } else if ( cop.get( "-p", &nThreads ) ) {
  } else if ( cop.get( "--solver-time-limit", &nTimeout ) ) {
  } else if ( cop.get( "--workmem", &nWorkMemLimit ) ) {
  } else if ( cop.get( "--readParam", &sReadParams ) ) {
  } else if ( cop.get( "--writeParam", &sWriteParams ) ) {
  } else if ( cop.get( "--absGap", &absGap) ) {
  } else if ( cop.get( "--relGap", &relGap) ) {
  } else if ( cop.get( "--intTol", &intTol) ) {
//   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else if (cop.get("--scip-dll", &scip_dll)) {
  } else
    return false;
  return true;
error:
  return false;
}

void MIP_scip_wrapper::wrap_assert(SCIP_RETCODE retcode, string msg, bool fTerm)
{
      /* evaluate return code of the SCIP process */
      if( retcode != SCIP_OKAY )
      {
          /* write error back trace */
          plugin->SCIPprintError(retcode);
        string msgAll = ("  MIP_scip_wrapper runtime error, see output:  " + msg);
        cerr << msgAll << endl;
        if (fTerm) {
          cerr << "TERMINATING." << endl;
          throw runtime_error(msgAll);
        }
      }
}

SCIP_RETCODE MIP_scip_wrapper::openSCIP()
{
  if (options->scip_dll.size())
    plugin = new ScipPlugin(options->scip_dll);
  else
    plugin = new ScipPlugin();

  SCIP_PLUGIN_CALL( plugin, plugin->SCIPcreate(&scip) );
  SCIP_PLUGIN_CALL( plugin, plugin->SCIPincludeDefaultPlugins(scip) );

  /* create empty problem */
  SCIP_PLUGIN_CALL( plugin, plugin->SCIPcreateProbBasic(scip, "mzn_scip") );
  return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::closeSCIP()
{
  SCIP_PLUGIN_CALL( plugin, plugin->SCIPfree(&scip) );

   delete plugin;
  /// and at last:
//   MIP_wrapper::cleanup();
   return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::doAddVars_SCIP
  (size_t n, double* obj, double* lb, double* ub, MIP_wrapper::VarType* vt, string *names)
{
  /// Convert var types:
//   vector<char> ctype(n);
//   vector<char*> pcNames(n);
  for (size_t j=0; j<n; ++j) {
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
        throw runtime_error("  MIP_wrapper: unknown variable type");
    }
    scipVars.resize(scipVars.size()+1);
    if (fPhase1Over)
      assert(scipVars.size() == colObj.size());
    SCIP_PLUGIN_CALL( plugin, plugin->SCIPcreateVarBasic(scip, &scipVars.back(), names[j].c_str(), lb[j], ub[j], obj[j], ctype) );
    SCIP_PLUGIN_CALL( plugin, plugin->SCIPaddVar(scip, scipVars.back()) );
  }
//   retcode = SCIP_newcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
//   wrap_assert( !retcode,  "Failed to declare variables." );
  return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::delSCIPVars()
{
  for (size_t j=0; j<scipVars.size(); ++j)
    plugin->SCIPreleaseVar(scip, &scipVars[j]);
  return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::addRow_SCIP
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  /// Convert var types:
  double lh = -SCIPinfinityPlugin(plugin, scip), rh = SCIPinfinityPlugin(plugin, scip);
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
        throw runtime_error("  MIP_wrapper: unknown constraint type");
    }
  const int ccnt=0;
  const int rcnt=1;
  const int rmatbeg[] = { 0 };
  char * pRName = (char*)rowName.c_str();
  // ignoring mask for now.  TODO
      SCIP_CONS* cons;
      vector<SCIP_VAR*> ab(nnz);
      
      for (int j=0; j<nnz; ++j)
        ab[j] = scipVars[rmatind[j]];

      SCIP_PLUGIN_CALL( plugin, plugin->SCIPcreateConsBasicLinear(scip, &cons, rowName.c_str(), nnz, &ab[0], rmatval, lh, rh) );
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPaddCons(scip, cons) );
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPreleaseCons(scip, &cons) );
      return SCIP_OKAY;
//   retcode = SCIP_addrows (env, lp, ccnt, rcnt, nnz, &rhs,
//         &ssense, rmatbeg, rmatind, rmatval,
//         NULL, &pRName);
//   wrap_assert( !retcode,  "Failed to add constraint." );
}

void MIP_scip_wrapper::setVarBounds(int iVar, double lb, double ub)
{
  wrap_assert( lb<=ub ? SCIP_OKAY : SCIP_ERROR,
               "scip interface: setVarBounds: lb>ub" );
  setVarLB(iVar, lb);
  setVarUB(iVar, ub);
}

void MIP_scip_wrapper::setVarLB(int iVar, double lb)
{
  auto res = plugin->SCIPchgVarLbGlobal(scip,scipVars[iVar],lb);
  wrap_assert( res,  "scip interface: failed to set var lb." );
}

void MIP_scip_wrapper::setVarUB(int iVar, double ub)
{
  auto res = plugin->SCIPchgVarUbGlobal(scip,scipVars[iVar],ub);
  wrap_assert( res,  "scip interface: failed to set var ub." );
}



void MIP_scip_wrapper::addIndicatorConstraint(
    int iBVar, int bVal, int nnz, int* rmatind, double* rmatval,
    MIP_wrapper::LinConType sense, double rhs, string rowName) {
  MZN_ASSERT_HARD_MSG( 0<=bVal && 1>=bVal, "SCIP: addIndicatorConstraint: bVal not 0/1" );
  //// Make sure in order to notice the indices of lazy constr: also here?   TODO
//  ++ nRows;

  SCIP_CONS* cons;
  vector<SCIP_VAR*> ab(nnz);
  SCIP_VAR* indicator_var; // SCIP 6.0.1 requires that the implication is active for indicator_x == 1

  for (int j=0; j<nnz; ++j)
    ab[j] = scipVars[rmatind[j]];

  indicator_var = scipVars[iBVar];
  if (0==bVal)
  {
    wrap_assert( plugin->SCIPgetNegatedVar(scip, indicator_var, &indicator_var) );
  }

  if (LQ==sense || EQ==sense) {
    wrap_assert( plugin->SCIPcreateConsBasicIndicator(scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatval, rhs ) );
    wrap_assert( plugin->SCIPaddCons(scip, cons) );
    wrap_assert( plugin->SCIPreleaseCons(scip, &cons) );
  }
  if (GQ==sense || EQ==sense) {
    std::vector<double> rmatvalNEG(nnz);
    for (int i=nnz; i--;)
      rmatvalNEG[i] = -rmatval[i];
    wrap_assert( plugin->SCIPcreateConsBasicIndicator(scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatvalNEG.data(), -rhs ) );
    wrap_assert( plugin->SCIPaddCons(scip, cons) );
    wrap_assert( plugin->SCIPreleaseCons(scip, &cons) );
  }
}

void MIP_scip_wrapper::addBoundsDisj(int n, double *fUB, double *bnd, int *vars,
                                     int nF, double *fUBF, double *bndF, int *varsF, string rowName) {
  SCIP_CONS* cons;
  std::vector<SCIP_VAR*> v(n+nF);
  std::vector<SCIP_BOUNDTYPE> bt(n+nF);
  std::vector<SCIP_Real> bs(n+nF);

  for (int j=0; j<n; ++j) {
    v[j] = scipVars[vars[j]];
    bt[j] = fUB[j] ? SCIP_BOUNDTYPE_UPPER : SCIP_BOUNDTYPE_LOWER;
    bs[j] = bnd[j];
  }
  for (int j=0; j<nF; ++j) {
    v[n+j] = scipVars[varsF[j]];
    bt[n+j] = fUBF[j] ? SCIP_BOUNDTYPE_UPPER : SCIP_BOUNDTYPE_LOWER;
    bs[n+j] = bndF[j];
  }

  wrap_assert( plugin->SCIPcreateConsBasicBounddisjunction(scip, &cons, rowName.c_str(),
                                                   v.size(), v.data(), bt.data(), bs.data() ) );
  wrap_assert( plugin->SCIPaddCons(scip, cons) );
  wrap_assert( plugin->SCIPreleaseCons(scip, &cons) );

}

void MIP_scip_wrapper::addCumulative(int nnz, int *rmatind, double *d, double *r, double b, string rowName)
{

  SCIP_CONS* cons;
  vector<SCIP_VAR*> ab(nnz);
  vector<int> nd(nnz), nr(nnz);

  for (int j=0; j<nnz; ++j) {
    ab[j] = scipVars[rmatind[j]];
    nd[j] = (int)round(d[j]);
    nr[j] = (int)round(r[j]);
  }

  wrap_assert( plugin->SCIPcreateConsBasicCumulative(scip, &cons, rowName.c_str(),
                                              nnz, ab.data(), nd.data(), nr.data(), (int)round(b)) );

  wrap_assert( plugin->SCIPaddCons(scip, cons) );
  wrap_assert( plugin->SCIPreleaseCons(scip, &cons) );
}


/// SolutionCallback ------------------------------------------------------------------------

/// From event_bestsol.c:
#define EVENTHDLR_NAME         "bestsol"
#define EVENTHDLR_DESC         "event handler for best solutions found"

// Dirty way of accessing SCIP functions inside C callbacks
static ScipPlugin* _cb_plugin;

/** initialization method of event handler (called after problem was transformed) */
static
SCIP_DECL_EVENTINIT(eventInitBestsol)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(eventhdlr != NULL);
   assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);

   /* notify SCIP that your event handler wants to react on the event type best solution found */
   SCIP_PLUGIN_CALL( _cb_plugin, _cb_plugin->SCIPcatchEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, NULL) );

   return SCIP_OKAY;
}

/** deinitialization method of event handler (called before transformed problem is freed) */
static
SCIP_DECL_EVENTEXIT(eventExitBestsol)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(eventhdlr != NULL);
   assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);
   
   /* notify SCIP that your event handler wants to drop the event type best solution found */
   SCIP_PLUGIN_CALL( _cb_plugin, _cb_plugin->SCIPdropEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, -1) );

   return SCIP_OKAY;
}

static MIP_wrapper::CBUserInfo *cbuiPtr=0;
static SCIP_VAR ** scipVarsPtr=0;

/** execution method of event handler */
static
SCIP_DECL_EVENTEXEC(eventExecBestsol)
{  /*lint --e{715}*/
   SCIP_SOL* bestsol;
   SCIP_Real objVal;
   int newincumbent=0;

   assert(eventhdlr != NULL);
   assert(strcmp(_cb_plugin->SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);
   assert(event != NULL);
   assert(scip != NULL);
   assert(_cb_plugin->SCIPeventGetType(event) == SCIP_EVENTTYPE_BESTSOLFOUND);

   SCIPdebugMessage("exec method of event handler for best solution found\n");
   
   bestsol = _cb_plugin->SCIPgetBestSol(scip);
   assert(bestsol != NULL);
   objVal = _cb_plugin->SCIPgetSolOrigObj(scip, bestsol);
 
   if (!cbuiPtr)
     return SCIP_OKAY;
      
    if ( fabs(cbuiPtr->pOutput->objVal - objVal) > 1e-12*(1.0 + fabs(objVal)) ) {
        newincumbent = 1;
        cbuiPtr->pOutput->objVal = objVal;
      cbuiPtr->pOutput->status = MIP_wrapper::SAT;
      cbuiPtr->pOutput->statusName = "feasible from a callback";
    }

   if ( newincumbent && scipVarsPtr ) {
      assert(cbuiPtr->pOutput->x);
      SCIP_PLUGIN_CALL( _cb_plugin, _cb_plugin->SCIPgetSolVals(scip, bestsol, cbuiPtr->pOutput->nCols,
                                scipVarsPtr, (double*)cbuiPtr->pOutput->x) );
//       wrap_assert(!retcode, "Failed to get variable values.");
      cbuiPtr->pOutput->nNodes = _cb_plugin->SCIPgetNNodes (scip);
      cbuiPtr->pOutput->nOpenNodes = _cb_plugin->SCIPgetNNodesLeft(scip);
      cbuiPtr->pOutput->bestBound = _cb_plugin->SCIPgetDualbound (scip);

      cbuiPtr->pOutput->dCPUTime = -1;

      /// Call the user function:
      if (cbuiPtr->solcbfn)
          (*cbuiPtr->solcbfn)(*cbuiPtr->pOutput, cbuiPtr->psi);
   }
   
   return SCIP_OKAY;
}

/** includes event handler for best solution found */
SCIP_RETCODE MIP_scip_wrapper::includeEventHdlrBestsol() {
   SCIP_EVENTHDLRDATA* eventhdlrdata;
   SCIP_EVENTHDLR* eventhdlr;
   eventhdlrdata = NULL;
   
   eventhdlr = NULL;

   _cb_plugin = plugin; // So that callbacks can access plugin functions

   /* create event handler for events on watched variables */
   SCIP_PLUGIN_CALL( plugin, plugin->SCIPincludeEventhdlrBasic(scip, &eventhdlr, EVENTHDLR_NAME, EVENTHDLR_DESC, eventExecBestsol, eventhdlrdata) );
   assert(eventhdlr != NULL);

   /// Not for sub-SCIPs
   SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetEventhdlrInit(scip, eventhdlr, eventInitBestsol) );
   SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetEventhdlrExit(scip, eventhdlr, eventExitBestsol) );
   
   return SCIP_OKAY;
}


MIP_scip_wrapper::Status MIP_scip_wrapper::convertStatus(SCIP_STATUS scipStatus)
{
  Status s = Status::UNKNOWN;
   /* Converting the status. */
   switch(scipStatus) {
     case SCIP_STATUS_OPTIMAL:
       s = Status::OPT;
       output.statusName = "Optimal";
       assert(plugin->SCIPgetNSolsFound(scip));
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
//        wrap_assert(SCIP_getsolnpoolnumsolns(env, lp), "Feasibility reported but pool empty?", false);
//        break;
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
       if (plugin->SCIPgetNSols (scip)) {
         s = Status::SAT;
         output.statusName = "Feasible";
       } else {
         s = Status::UNKNOWN;
         output.statusName = "Unknown";
       }
   }
   return s;
}

SCIP_DECL_MESSAGEWARNING(printMsg) {
  cerr << msg << flush;
}

SCIP_RETCODE MIP_scip_wrapper::solve_SCIP() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  if ( options->flag_all_solutions && 0==nProbType )
    cerr << "WARNING. --all-solutions for SAT problems not implemented." << endl;

    if (options->nThreads>0)
      SCIP_PLUGIN_CALL(plugin, plugin->SCIPsetIntParam(scip, "lp/threads", options->nThreads) );

    if (options->nTimeout>0)
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetRealParam(scip, "limits/time", static_cast<double>(options->nTimeout)/1000.0) );

    if (options->nWorkMemLimit>0)
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetRealParam(scip, "limits/memory", options->nWorkMemLimit) );

    if ( options->absGap>=0.0 )
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetRealParam( scip, "limits/absgap", options->absGap ) );
    if ( options->relGap>=0.0 )
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetRealParam( scip, "limits/gap", options->relGap ) );
    if ( options->intTol>=0.0 )
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetRealParam( scip, "numerics/feastol", options->intTol ) );

//    retcode =  SCIP_setintparam (env, SCIP_PARAM_ClockType, 1);            // CPU time
//    wrap_assert(!retcode, "  SCIP Warning: Failure to measure CPU time.", false);

    if (!options->sExportModel.empty()) {
//       std::cerr <<"  Exporting LP model to "  << sExportModel << " ..." << std::endl;
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPwriteOrigProblem(scip, options->sExportModel.c_str(), 0, 0) );
    }

  /* Turn on output to the screen  - after model export */
    if(!fVerbose) {
//       SCIP_PLUGIN_CALL(SCIPsetMessagehdlr(scip, NULL));  No LP export then
      plugin->SCIPsetMessagehdlrQuiet(scip, true);
    } else {
      SCIP_MESSAGEHDLR* pHndl=0;
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPmessagehdlrCreate ( &pHndl, FALSE, NULL, FALSE, printMsg, printMsg, printMsg, NULL, NULL) );
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetMessagehdlr(scip, pHndl) );
    }
    
//     assert(scipVars.size() == colObj.size());
    int cur_numcols = scipVars.size();    // No, we create negated indicators: getNCols();
    assert(cur_numcols == colObj.size());
    assert(cur_numcols == scipVars.size());

   /// Solution callback
   output.nCols = colObj.size();
   x.resize(output.nCols);
   output.x = &x[0];
   if (options->flag_all_solutions && cbui.solcbfn && !cbuiPtr) {
   /* include event handler for best solution found */
     SCIP_PLUGIN_CALL( plugin, includeEventHdlrBestsol() );
     cbuiPtr = &cbui;   // not thread-safe...         TODO
     scipVarsPtr = &scipVars[0];
//       retcode = SCIP_setinfocallbackfunc (env, solcallback, &cbui);
//       wrap_assert(!retcode, "Failed to set solution callback", false);
   }

    if (options->sReadParams.size()) {
     SCIP_PLUGIN_CALL( plugin, plugin->SCIPreadParams (scip, options->sReadParams.c_str()) );
    }
    
    if (options->sWriteParams.size()) {
     SCIP_PLUGIN_CALL( plugin, plugin->SCIPwriteParams (scip, options->sReadParams.c_str(), TRUE, FALSE) );
    }

    cbui.pOutput->dWallTime0 = output.dWallTime0 =
      std::chrono::steady_clock::now();
   output.dCPUTime = clock();

   /* Optimize the problem and obtain solution. */
   SCIP_PLUGIN_CALL( plugin, plugin->SCIPsolve (scip) );
//    wrap_assert( !retcode,  "Failed to optimize MIP." );

   output.dWallTime = std::chrono::duration<double>(
     std::chrono::steady_clock::now() - output.dWallTime0).count();
   output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;
   
   cbuiPtr = 0;                             /// cleanup
   scipVarsPtr = 0;

   SCIP_STATUS solstat = plugin->SCIPgetStatus (scip);
   output.status = convertStatus(solstat);
//    output.statusName = SCIP_getstatstring (env, solstat, scip_status_buffer);

   /// Continuing to fill the output object:
   output.objVal = plugin->SCIPgetPrimalbound (scip);
   output.bestBound = plugin->SCIPgetDualbound (scip);
//    wrap_assert(!retcode, "Failed to get the best bound.", false);
   if (Status::OPT == output.status || Status::SAT ==output.status) {
//       wrap_assert( !retcode, "No MIP objective value available." );
      
      x.resize(cur_numcols);
      output.x = &x[0];
      SCIP_PLUGIN_CALL( plugin, plugin->SCIPgetSolVals(scip, plugin->SCIPgetBestSol(scip), cur_numcols, &scipVars[0], (double*)output.x) );
      if (cbui.solcbfn && (!options->flag_all_solutions || !cbui.printed)) {
        cbui.solcbfn(output, cbui.psi);
      }
   }
   output.nNodes = plugin->SCIPgetNNodes (scip);
   output.nOpenNodes = plugin->SCIPgetNNodesLeft(scip);  // SCIP_getnodeleftcnt (env, lp);

   SCIP_PLUGIN_CALL( plugin, plugin->SCIPfreeTransform(scip) );

   return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::setObjSense_SCIP(int s)
{
  SCIP_PLUGIN_CALL( plugin, plugin->SCIPsetObjsense(scip, s>0 ? SCIP_OBJSENSE_MAXIMIZE : SCIP_OBJSENSE_MINIMIZE) );
  return SCIP_OKAY;
}

