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

#include "scip/scipshell.h"

string MIP_scip_wrapper::getDescription(MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "MIP wrapper for SCIP "
      << getVersion(opt)
      << ". Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}
string MIP_scip_wrapper::getVersion(MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << SCIPmajorVersion() << '.'
      << SCIPminorVersion() << '.'
      << SCIPtechVersion() << '.'
      << SCIPsubversion();
  return oss.str();
}
string MIP_scip_wrapper::needDllFlag( ) {
  return "";
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
  return {"-a", "-p"};
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
          SCIPprintError(retcode);
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
   SCIP_CALL( SCIPcreate(&scip) );
   SCIP_CALL( SCIPincludeDefaultPlugins(scip) );

   /* create empty problem */
   SCIP_CALL( SCIPcreateProbBasic(scip, "mzn_scip") );
   return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::closeSCIP()
{
   SCIP_CALL( SCIPfree(&scip) );
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
    SCIP_CALL( SCIPcreateVarBasic(scip, &scipVars.back(), names[j].c_str(), lb[j], ub[j], obj[j], ctype) );
    SCIP_CALL( SCIPaddVar(scip, scipVars.back()) );
  }
//   retcode = SCIP_newcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
//   wrap_assert( !retcode,  "Failed to declare variables." );
  return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::delSCIPVars()
{
  for (size_t j=0; j<scipVars.size(); ++j)
    SCIPreleaseVar(scip, &scipVars[j]);
  return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::addRow_SCIP
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  /// Convert var types:
  double lh = -SCIPinfinity(scip), rh = SCIPinfinity(scip);
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

      SCIP_CALL( SCIPcreateConsBasicLinear(scip, &cons, rowName.c_str(), nnz, &ab[0], rmatval, lh, rh) );
      SCIP_CALL( SCIPaddCons(scip, cons) );
      SCIP_CALL( SCIPreleaseCons(scip, &cons) );
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
  auto res = SCIPchgVarLbGlobal(scip,scipVars[iVar],lb);
  wrap_assert( res,  "scip interface: failed to set var lb." );
}

void MIP_scip_wrapper::setVarUB(int iVar, double ub)
{
  auto res = SCIPchgVarUbGlobal(scip,scipVars[iVar],ub);
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
    wrap_assert(SCIPgetNegatedVar(scip, indicator_var, &indicator_var));
  }

  if (LQ==sense || EQ==sense) {
    wrap_assert( SCIPcreateConsBasicIndicator(scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatval, rhs ) );
    wrap_assert( SCIPaddCons(scip, cons) );
    wrap_assert( SCIPreleaseCons(scip, &cons) );
  }
  if (GQ==sense || EQ==sense) {
    std::vector<double> rmatvalNEG(nnz);
    for (int i=nnz; i--;)
      rmatvalNEG[i] = -rmatval[i];
    wrap_assert( SCIPcreateConsBasicIndicator(scip, &cons, rowName.c_str(), indicator_var, nnz, ab.data(), rmatvalNEG.data(), -rhs ) );
    wrap_assert( SCIPaddCons(scip, cons) );
    wrap_assert( SCIPreleaseCons(scip, &cons) );
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

  wrap_assert( SCIPcreateConsBasicBounddisjunction(scip, &cons, rowName.c_str(),
                                                   v.size(), v.data(), bt.data(), bs.data() ) );
  wrap_assert( SCIPaddCons(scip, cons) );
  wrap_assert( SCIPreleaseCons(scip, &cons) );

}

/// SolutionCallback ------------------------------------------------------------------------

/// From event_bestsol.c:
#define EVENTHDLR_NAME         "bestsol"
#define EVENTHDLR_DESC         "event handler for best solutions found"

/** includes event handler for best solution found */
extern
SCIP_RETCODE SCIPincludeEventHdlrBestsol(
   SCIP*                 scip                /**< SCIP data structure */
   );

/** copy method for event handler plugins (called when SCIP copies plugins) */
static
SCIP_DECL_EVENTCOPY(eventCopyBestsol) 
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(eventhdlr != NULL);
   assert(strcmp(SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);

   /* call inclusion method of event handler */
   SCIP_CALL( SCIPincludeEventHdlrBestsol(scip) );

   return SCIP_OKAY;
}

/** initialization method of event handler (called after problem was transformed) */
static
SCIP_DECL_EVENTINIT(eventInitBestsol)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(eventhdlr != NULL);
   assert(strcmp(SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);

   /* notify SCIP that your event handler wants to react on the event type best solution found */
   SCIP_CALL( SCIPcatchEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, NULL) );

   return SCIP_OKAY;
}

/** deinitialization method of event handler (called before transformed problem is freed) */
static
SCIP_DECL_EVENTEXIT(eventExitBestsol)
{  /*lint --e{715}*/
   assert(scip != NULL);
   assert(eventhdlr != NULL);
   assert(strcmp(SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);
   
   /* notify SCIP that your event handler wants to drop the event type best solution found */
   SCIP_CALL( SCIPdropEvent( scip, SCIP_EVENTTYPE_BESTSOLFOUND, eventhdlr, NULL, -1) );

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
   assert(strcmp(SCIPeventhdlrGetName(eventhdlr), EVENTHDLR_NAME) == 0);
   assert(event != NULL);
   assert(scip != NULL);
   assert(SCIPeventGetType(event) == SCIP_EVENTTYPE_BESTSOLFOUND);

   SCIPdebugMessage("exec method of event handler for best solution found\n");
   
   bestsol = SCIPgetBestSol(scip);
   assert(bestsol != NULL);
   objVal = SCIPgetSolOrigObj(scip, bestsol);
 
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
      SCIP_CALL( SCIPgetSolVals(scip, bestsol, cbuiPtr->pOutput->nCols,
                                scipVarsPtr, (double*)cbuiPtr->pOutput->x) );
//       wrap_assert(!retcode, "Failed to get variable values.");
      cbuiPtr->pOutput->nNodes = SCIPgetNNodes (scip);
      cbuiPtr->pOutput->nOpenNodes = SCIPgetNNodesLeft(scip);
      cbuiPtr->pOutput->bestBound = SCIPgetDualbound (scip);

      cbuiPtr->pOutput->dCPUTime = -1;

      /// Call the user function:
      if (cbuiPtr->solcbfn)
          (*cbuiPtr->solcbfn)(*cbuiPtr->pOutput, cbuiPtr->ppp);
   }
   
   return SCIP_OKAY;
}

/** includes event handler for best solution found */
SCIP_RETCODE SCIPincludeEventHdlrBestsol(
   SCIP*                 scip                /**< SCIP data structure */
   )
{
   SCIP_EVENTHDLRDATA* eventhdlrdata;
   SCIP_EVENTHDLR* eventhdlr;
   eventhdlrdata = NULL;
   
   eventhdlr = NULL;
   /* create event handler for events on watched variables */
   SCIP_CALL( SCIPincludeEventhdlrBasic(scip, &eventhdlr, EVENTHDLR_NAME, EVENTHDLR_DESC, eventExecBestsol, eventhdlrdata) );
   assert(eventhdlr != NULL);

   /// Not for sub-SCIPs
   // SCIP_CALL( SCIPsetEventhdlrCopy(scip, eventhdlr, eventCopyBestsol) );
   SCIP_CALL( SCIPsetEventhdlrInit(scip, eventhdlr, eventInitBestsol) );
   SCIP_CALL( SCIPsetEventhdlrExit(scip, eventhdlr, eventExitBestsol) );
   
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
       assert(SCIPgetNSolsFound(scip));
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
       if (SCIPgetNSols (scip)) {
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
      SCIP_CALL( SCIPsetIntParam(scip, "lp/threads", options->nThreads) );

    if (options->nTimeout>0)
      SCIP_CALL( SCIPsetRealParam(scip, "limits/time", static_cast<double>(options->nTimeout)/1000.0) );

    if (options->nWorkMemLimit>0)
      SCIP_CALL( SCIPsetRealParam(scip, "limits/memory", options->nWorkMemLimit) );

    if ( options->absGap>=0.0 )
      SCIP_CALL( SCIPsetRealParam( scip, "limits/absgap", options->absGap ) );
    if ( options->relGap>=0.0 )
      SCIP_CALL( SCIPsetRealParam( scip, "limits/gap", options->relGap ) );
    if ( options->intTol>=0.0 )
      SCIP_CALL( SCIPsetRealParam( scip, "numerics/feastol", options->intTol ) );

//    retcode =  SCIP_setintparam (env, SCIP_PARAM_ClockType, 1);            // CPU time
//    wrap_assert(!retcode, "  SCIP Warning: Failure to measure CPU time.", false);

    if (!options->sExportModel.empty()) {
//       std::cerr <<"  Exporting LP model to "  << sExportModel << " ..." << std::endl;
      SCIP_CALL( SCIPwriteOrigProblem(scip, options->sExportModel.c_str(), 0, 0) );
    }

  /* Turn on output to the screen  - after model export */
    if(!fVerbose) {
//       SCIP_CALL(SCIPsetMessagehdlr(scip, NULL));  No LP export then
      SCIPsetMessagehdlrQuiet(scip, true);
    } else {
      SCIP_MESSAGEHDLR* pHndl=0;
      SCIP_CALL ( SCIPmessagehdlrCreate ( &pHndl, FALSE, NULL, FALSE, printMsg, printMsg, printMsg, NULL, NULL) );
      SCIP_CALL ( SCIPsetMessagehdlr(scip, pHndl) );      
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
     SCIP_CALL( SCIPincludeEventHdlrBestsol(scip) );
     cbuiPtr = &cbui;   // not thread-safe...         TODO
     scipVarsPtr = &scipVars[0];
//       retcode = SCIP_setinfocallbackfunc (env, solcallback, &cbui);
//       wrap_assert(!retcode, "Failed to set solution callback", false);
   }

    if (options->sReadParams.size()) {
     SCIP_CALL( SCIPreadParams (scip, options->sReadParams.c_str()) );
    }
    
    if (options->sWriteParams.size()) {
     SCIP_CALL( SCIPwriteParams (scip, options->sReadParams.c_str(), TRUE, FALSE) );
    }

   output.dCPUTime = clock();

   /* Optimize the problem and obtain solution. */
   SCIP_CALL( SCIPsolve (scip) );
//    wrap_assert( !retcode,  "Failed to optimize MIP." );

   output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;
   
   cbuiPtr = 0;                             /// cleanup
   scipVarsPtr = 0;

   SCIP_STATUS solstat = SCIPgetStatus (scip);
   output.status = convertStatus(solstat);
//    output.statusName = SCIP_getstatstring (env, solstat, scip_status_buffer);

   /// Continuing to fill the output object:
   output.objVal = SCIPgetPrimalbound (scip);
   output.bestBound = SCIPgetDualbound (scip);
//    wrap_assert(!retcode, "Failed to get the best bound.", false);
   if (Status::OPT == output.status || Status::SAT ==output.status) {
//       wrap_assert( !retcode, "No MIP objective value available." );
      
      x.resize(cur_numcols);
      output.x = &x[0];
      SCIP_CALL( SCIPgetSolVals(scip, SCIPgetBestSol(scip), cur_numcols, &scipVars[0], (double*)output.x) );
      if (cbui.solcbfn && (!options->flag_all_solutions || !cbui.printed)) {
        cbui.solcbfn(output, cbui.ppp);
      }
   }
   output.nNodes = SCIPgetNNodes (scip);
   output.nOpenNodes = SCIPgetNNodesLeft(scip);  // SCIP_getnodeleftcnt (env, lp);

   SCIP_CALL( SCIPfreeTransform(scip) );

   return SCIP_OKAY;
}

SCIP_RETCODE MIP_scip_wrapper::setObjSense_SCIP(int s)
{
        SCIP_CALL(SCIPsetObjsense(scip, s>0 ? SCIP_OBJSENSE_MAXIMIZE : SCIP_OBJSENSE_MINIMIZE));
        return SCIP_OKAY;
}

