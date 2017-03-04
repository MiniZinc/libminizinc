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

#include <minizinc/solvers/MIP/MIP_cplex_wrap.hh>
#include <minizinc/utils.hh>

using namespace std;

/// Linking this module provides these functions:
MIP_wrapper* MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_cplex_wrapper;
}

string MIP_WrapperFactory::getVersion( ) {
  string v = "  MIP wrapper for IBM ILOG CPLEX  ";
  int status;
  CPXENVptr env = CPXopenCPLEX (&status);
  if (env) {
    v += CPXversion (env);
    status = CPXcloseCPLEX (&env);
  } else
    v += "[?? ...cannot open CPLEX env to query version]";
  v += "  Compiled  " __DATE__ "  " __TIME__;
  return v;
}

void MIP_WrapperFactory::printHelp(ostream& os) {
  os
  << "IBM ILOG CPLEX  MIP wrapper options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read CPLEX parameters from file
  //               << "--writeParam <file> write CPLEX parameters to file
  //               << "--tuneParam         instruct CPLEX to tune parameters instead of solving
  << "--writeModel <file> write model to <file> (.lp, .mps, .sav, ...)" << std::endl
  << "-a                  print intermediate solutions (use for optimization problems only TODO)" << std::endl
  << "-p <N>              use N threads, default: 1" << std::endl
//   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
  << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read CPLEX parameters from file" << std::endl
  << "--writeParam <file> write CPLEX parameters to file" << std::endl
//   << "--tuneParam         instruct CPLEX to tune parameters instead of solving   NOT IMPL"

  << "--absGap <n>        absolute gap |primal-dual| to stop" << std::endl
  << "--relGap <n>        relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 to use backend's default" << std::endl
  << "--intTol <n>        integrality tolerance for a variable. Default 1e-6" << std::endl
//   << "--objDiff <n>       objective function discretization. Default 1.0" << std::endl

  << std::endl;
}

  static inline bool beginswith(string s, string t) {
    return s.compare(0, t.length(), t)==0;
  }

            /// SOLVER PARAMS ????
 static   int nThreads=1;
 static   string sExportModel;
 static   double nTimeout=-1;
 static   double nWorkMemLimit=-1;
 static   string sReadParams;
 static   string sWriteParams;
 static   bool flag_all_solutions = false;

 static   double absGap=-1;
 static   double relGap=1e-8;
 static   double intTol=1e-6;
 static   double objDiff=1.0;

bool MIP_WrapperFactory::processOption(int& i, int argc, const char** argv) {
  MiniZinc::CLOParser cop( i, argc, argv );
  if ( string(argv[i])=="-a"
      || string(argv[i])=="--all"
      || string(argv[i])=="--all-solutions" ) {
    flag_all_solutions = true;
  } else if (string(argv[i])=="-f") {
//     std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if ( cop.get( "--writeModel", &sExportModel ) ) {
  } else if ( cop.get( "-p", &nThreads ) ) {
  } else if ( cop.get( "--timeout", &nTimeout ) ) {
  } else if ( cop.get( "--workmem", &nWorkMemLimit ) ) {
  } else if ( cop.get( "--readParam", &sReadParams ) ) {
  } else if ( cop.get( "--writeParam", &sWriteParams ) ) {
  } else if ( cop.get( "--absGap", &absGap ) ) {
  } else if ( cop.get( "--relGap", &relGap ) ) {
  } else if ( cop.get( "--intTol", &intTol ) ) {
//   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else
    return false;
  return true;
error:
  return false;
}

void MIP_cplex_wrapper::wrap_assert(bool cond, string msg, bool fTerm)
{
   if ( !cond ) {
      strcpy(cplex_buffer, "[NO ERROR STRING GIVEN]");
      CPXgeterrorstring (env, status, cplex_buffer);
      string msgAll = ("  MIP_cplex_wrapper runtime error:  " + msg + "  " + cplex_buffer);
      cerr << msgAll << endl;
      if (fTerm) {
        cerr << "TERMINATING." << endl;
        throw runtime_error(msgAll);
      }
   }
}

void MIP_cplex_wrapper::openCPLEX()
{
  /// Cleanup first.
//   cleanup();
   /* Initialize the CPLEX environment */
   env = CPXopenCPLEX (&status);
   /* If an error occurs, the status value indicates the reason for
      failure.  A call to CPXgeterrorstring will produce the text of
      the error message.  Note that CPXopenCPLEX produces no output,
      so the only way to see the cause of the error is to use
      CPXgeterrorstring.  For other CPLEX routines, the errors will
      be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */
   wrap_assert ( env, "Could not open CPLEX environment." );
   /* Create the problem. */
   lp = CPXcreateprob (env, &status, "MIP_cplex_wrapper");
   /* A returned pointer of NULL may mean that not enough memory
      was available or there was some other problem.  In the case of
      failure, an error message will have been written to the error
      channel from inside CPLEX.  In this example, the setting of
      the parameter CPXPARAM_ScreenOutput causes the error message to
      appear on stdout.  */
   wrap_assert ( lp, "Failed to create LP." );
}

void MIP_cplex_wrapper::closeCPLEX()
{
  /// Freeing the problem can be slow both in C and C++, see IBM forums. Skipping.
     /* Free up the problem as allocated by CPXcreateprob, if necessary */
//    if ( lp != NULL ) {
//       status = CPXfreeprob (env, &lp);
//       cplex_wrap_assert ( !status, "CPXfreeprob failed." );
//    }
  lp = 0;
   /* Free up the CPLEX environment, if necessary */
   if ( env != NULL ) {
      status = CPXcloseCPLEX (&env);
      wrap_assert ( !status, "Could not close CPLEX environment." );
   }
  /// and at last:
//   MIP_wrapper::cleanup();
}

void MIP_cplex_wrapper::doAddVars
  (size_t n, double* obj, double* lb, double* ub, MIP_wrapper::VarType* vt, string *names)
{
  /// Convert var types:
  vector<char> ctype(n);
  vector<char*> pcNames(n);
  for (size_t i=0; i<n; ++i) {
    pcNames[i] = (char*)names[i].c_str();
    switch (vt[i]) {
      case REAL:
        ctype[i] = CPX_CONTINUOUS;
        break;
      case INT:
        ctype[i] = CPX_INTEGER;
        break;
      case BINARY:
        ctype[i] = CPX_BINARY;
        break;
      default:
        throw runtime_error("  MIP_wrapper: unknown variable type");
    }
  }
  status = CPXnewcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
  wrap_assert( !status,  "Failed to declare variables." );
}

static char getCPLEXConstrSense(MIP_wrapper::LinConType sense) {
    switch (sense) {
      case MIP_wrapper::LQ:
        return 'L';
      case MIP_wrapper::EQ:
        return 'E';
      case MIP_wrapper::GQ:
        return 'G';
      default:
        throw runtime_error("  MIP_cplex_wrapper: unknown constraint type");
    }
}

void MIP_cplex_wrapper::addRow
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  /// Convert var types:
  char ssense=getCPLEXConstrSense(sense);
  const int ccnt=0;
  const int rcnt=1;
  const int rmatbeg[] = { 0 };
  char * pRName = (char*)rowName.c_str();
  if (MaskConsType_Normal & mask) {
    status = CPXaddrows (env, lp, ccnt, rcnt, nnz, &rhs,
        &ssense, rmatbeg, rmatind, rmatval,
        NULL, &pRName);
    wrap_assert( !status,  "Failed to add constraint." );
  }
  if (MaskConsType_Usercut & mask) {
    status = CPXaddusercuts (env, lp, rcnt, nnz, &rhs,
        &ssense, rmatbeg, rmatind, rmatval,
        &pRName);
    wrap_assert( !status,  "Failed to add usercut." );
  }
  if (MaskConsType_Lazy & mask) {
    status = CPXaddlazyconstraints (env, lp, rcnt, nnz, &rhs,
        &ssense, rmatbeg, rmatind, rmatval,
        &pRName);
    wrap_assert( !status,  "Failed to add lazy constraint." );
  }
}


/// SolutionCallback ------------------------------------------------------------------------
/// CPLEX ensures thread-safety
static int CPXPUBLIC
solcallback (CPXCENVptr env, void *cbdata, int wherefrom, void *cbhandle)
{
   int status = 0;

   MIP_wrapper::CBUserInfo *info = (MIP_wrapper::CBUserInfo*) cbhandle;
   int        hasincumbent = 0;
   int        newincumbent = 0;
   double objVal;

   status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODE_COUNT, &info->pOutput->nNodes);
   if ( status )  goto TERMINATE;

   status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_NODES_LEFT, &info->pOutput->nOpenNodes);
   if ( status )  goto TERMINATE;

   status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                CPX_CALLBACK_INFO_MIP_FEAS, &hasincumbent);
   if ( status )  goto TERMINATE;

   if ( hasincumbent ) {
      status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                   CPX_CALLBACK_INFO_BEST_INTEGER, &objVal);
      if ( status )  goto TERMINATE;
      
      if ( fabs(info->pOutput->objVal - objVal) > 1e-12*(1.0 + fabs(objVal)) ) {
         newincumbent = 1;
         info->pOutput->objVal = objVal;
        info->pOutput->status = MIP_wrapper::SAT;
        info->pOutput->statusName = "feasible from a callback";

      }
   }

//    if ( nodecnt >= info->lastlog + 100  ||  newincumbent ) {
//       double walltime;
//       double dettime;

      status = CPXgetcallbackinfo (env, cbdata, wherefrom,
                                   CPX_CALLBACK_INFO_BEST_REMAINING, &info->pOutput->bestBound);
//       if ( status )  goto TERMINATE;

//       status = CPXgettime (env, &walltime);
//       if ( status )  goto TERMINATE;
// 
//       status = CPXgetdettime (env, &dettime);
//       if ( status )  goto TERMINATE;
// 
//    }

   if ( newincumbent ) {
      assert(info->pOutput->x);
      status = CPXgetcallbackincumbent (env, cbdata, wherefrom,
                                        (double*)info->pOutput->x,
                                        0, info->pOutput->nCols-1);
      if ( status )  goto TERMINATE;

      info->pOutput->dCPUTime = double(std::clock() - info->pOutput->cCPUTime0) / CLOCKS_PER_SEC;

      /// Call the user function:
      if (info->solcbfn)
          (*info->solcbfn)(*info->pOutput, info->ppp);
   }
   

TERMINATE:
   return (status);

} /* END logcallback */
// end SolutionCallback ---------------------------------------------------------------------

/// Cut callbacks, mostly copied from admipex5.c, CPLEX 12.6.3
/* The following macro defines the smallest improvement 
   on the value of the objective function that is required
   when adding user cuts from within a callback.
   If the improvement on the value of the ojective function
   is not large enough, the callback will abort the cut loop. */

#define EPSOBJ 0.1

/* The following structure will hold the information we need to 
   pass to the cut callback function */

struct cutinfo {
   CPXLPptr lp;
   int      numcols;
   int      num;
   double   *x;
   int      *beg;
   int      *ind; 
   double   *val;
   double   *rhs;
   int      nodeid;
   double   nodeobjval;
   int      objsen;
   MIP_wrapper::CBUserInfo *info=0;
};
typedef struct cutinfo CUTINFO, *CUTINFOptr;

/* Init information on the node objval for the user cut callback */

static void 
initnodeobjvalinfo (CPXENVptr env, CPXLPptr lp, CUTINFOptr cutinfo)
{
   cutinfo->nodeid = -1;
   cutinfo->nodeobjval = 0.0;
   cutinfo->objsen = CPXgetobjsen (env, lp);
   if ( cutinfo->objsen == CPX_MIN )
      cutinfo->objsen = 1;
   else
      cutinfo->objsen = -1;
} /* END initnodeobjvalinfo */



static int CPXPUBLIC 
myusercutcallback (CPXCENVptr env,
               void       *cbdata,
               int        wherefrom,
               void       *cbhandle,
               int        *useraction_p)
{
   int status = 0;

   CUTINFOptr cutinfo = (CUTINFOptr) cbhandle;

   int      numcols  = cutinfo->numcols;
   int      numcuts  = cutinfo->num;
//    double   *x       = cutinfo->x;
//    int      *beg     = cutinfo->beg;
//    int      *ind     = cutinfo->ind;
//    double   *val     = cutinfo->val;
//    double   *rhs     = cutinfo->rhs;
//    int      *cutind  = NULL;
//    double   *cutval  = NULL;
   double   cutvio;
   int      addedcuts = 0;
   int      i, j, k, cutnz;
   MIP_wrapper::CBUserInfo *info = cutinfo->info;
//    double   *x       = info->pCutOutput->x;

   *useraction_p = CPX_CALLBACK_DEFAULT; 

   /* If we are called as a user cut callback, decide
      first if we want to add cuts or abort the cut loop.
      When adding user cuts with purgeable flag set to 
      CPX_USECUT_PURGE or CPX_USECUT_FILTER, it is important 
      to avoid the possibility of an infinite cut loop, where 
      the same cuts are added to the LP and then immediately 
      purged at every cut pass. Such a situation can be avoided,
      for instance, by applying a tailing off criterion and aborting 
      the cut loop where no progress in the objval is observed.
      Note, however, that the same approach must not be applied
      with lazy constraints. In this case, if lazy constraints are
      added with purgeable flag set to CPX_USECUT_PURGE, adding
      the same lazy constraint more than once could be required
      to ensure the correctness of the final result. */

   bool fMIPSol=true;
   if ( wherefrom == CPX_CALLBACK_MIP_CUT_LOOP ||
        wherefrom == CPX_CALLBACK_MIP_CUT_LAST   ) {
      int    oldnodeid     = cutinfo->nodeid;
      double oldnodeobjval = cutinfo->nodeobjval;

      fMIPSol = false;
   
      /* Retrieve nodeid and node objval of the current node */

      status = CPXgetcallbacknodeinfo (env, cbdata, wherefrom, 0,
                                       CPX_CALLBACK_INFO_NODE_SEQNUM,
                                       &cutinfo->nodeid);
      if ( status ) {
         fprintf(stderr, "Failed to get node id.\n");
         goto TERMINATE;
      }

      status = CPXgetcallbacknodeinfo (env, cbdata, wherefrom, 0,
                                       CPX_CALLBACK_INFO_NODE_OBJVAL,
                                       &cutinfo->nodeobjval);
      if ( status ) {
         fprintf(stderr, "Failed to get node objval.\n");
         goto TERMINATE;
      }

      /* Abort the cut loop if we are stuck at the same node
         as before and there is no progress in the node objval */

      if ( oldnodeid == cutinfo->nodeid ) {
         double objchg = (cutinfo->nodeobjval - oldnodeobjval);
         /* Multiply objchg by objsen to normalize 
            the change in the objective function to 
            the case of a minimization problem */
         objchg *= cutinfo->objsen;
         if ( objchg <= EPSOBJ ) {
            *useraction_p = CPX_CALLBACK_ABORT_CUT_LOOP;
            goto TERMINATE;
         }
      }
   }

   /* If we reached this point, we are 
      .. in a lazyconstraint callback, or 
      .. in a user cut callback, and cuts seem to help 
         improving the node objval. 
      In both cases, we retrieve the x solution and 
      look for violated cuts. */

    if ( info->cutcbfn ) {    // if cut handler given
      MIP_wrapper::Output outpRlx;
      outpRlx.x = info->pOutput->x;  // using the sol output storage  TODO?
      outpRlx.nCols = info->pOutput->nCols;
      assert( outpRlx.x && outpRlx.nCols );
      status = CPXgetcallbacknodex (env, cbdata, wherefrom, (double*)outpRlx.x,
                                    0, outpRlx.nCols-1); 
      if ( status ) {
          fprintf(stderr, "Cut callback: failed to get node solution.\n");
          goto TERMINATE;
      }
      MIP_wrapper::CutInput cutInput;
      info->cutcbfn( outpRlx, cutInput, info->ppp, fMIPSol );
      static int nCuts=0;
      nCuts += cutInput.size();
      if ( cutInput.size() )
        cerr << "\n   N CUTS:  " << nCuts << endl;
      for ( auto& cd : cutInput ) {
        assert( cd.mask &
          (MIP_wrapper::MaskConsType_Usercut|MIP_wrapper::MaskConsType_Lazy) );
        /* Use a cut violation tolerance of 0.01 */
        if ( true ) {  //cutvio > 0.01 ) { 
          status = CPXcutcallbackadd (env, cbdata, wherefrom,
                       cd.rmatind.size(), cd.rhs, getCPLEXConstrSense(cd.sense),
                                      cd.rmatind.data(), cd.rmatval.data(),
                                      CPX_USECUT_PURGE);
          if ( status ) {
              fprintf (stderr, "CPLEX callback: failed to add cut.\n");
              goto TERMINATE;
          }
          addedcuts++;
        }
      }
   }

   /* Tell CPLEX that cuts have been created */ 
   if ( addedcuts > 0 ) {
      *useraction_p = CPX_CALLBACK_SET; 
   }

TERMINATE:

   return (status);

} /* END myusercutcallback */

// ----------------- END Cut callbacks ------------------


MIP_cplex_wrapper::Status MIP_cplex_wrapper::convertStatus(int cplexStatus)
{
  Status s = Status::UNKNOWN;
   /* Converting the status. */
   switch(cplexStatus) {
     case CPXMIP_OPTIMAL:
       s = Status::OPT;
       wrap_assert(CPXgetsolnpoolnumsolns(env, lp), "Optimality reported but pool empty?", false);
       break;
     case CPXMIP_INFEASIBLE:
       s = Status::UNSAT;
       break;
//      case CPXMIP_OPTIMAL_INFEAS:
     case CPXMIP_INForUNBD:
       s = Status::UNSATorUNBND;
       break;
     case CPXMIP_SOL_LIM:
     case CPXMIP_NODE_LIM_FEAS:
     case CPXMIP_TIME_LIM_FEAS:
     case CPXMIP_FAIL_FEAS:
     case CPXMIP_MEM_LIM_FEAS:
     case CPXMIP_ABORT_FEAS:
     case CPXMIP_FAIL_FEAS_NO_TREE:
       s = Status::SAT;
       wrap_assert(CPXgetsolnpoolnumsolns(env, lp), "Feasibility reported but pool empty?", false);
       break;
     case CPXMIP_UNBOUNDED:
       s = Status::UNBND;
       break;
//      case CPXMIP_ABORT_INFEAS:
     case CPXMIP_FAIL_INFEAS:
       s = Status::__ERROR;
       break;
     default:
//      case CPXMIP_OPTIMAL_TOL:
//      case CPXMIP_ABORT_RELAXATION_UNBOUNDED:
       if (CPXgetsolnpoolnumsolns (env, lp))
         s = Status::SAT;
       else
        s = Status::UNKNOWN;
   }
   return s;
}

void msgfunction(void *handle, const char *msg_string)
{
  cerr << msg_string << flush;
}

void MIP_cplex_wrapper::solve() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  if ( flag_all_solutions && 0==nProbType )
    cerr << "WARNING. --all-solutions for SAT problems not implemented." << endl;
  // Before all manual params ???
  if (sReadParams.size()) {
    status = CPXreadcopyparam (env, sReadParams.c_str());
    wrap_assert(!status, "Failed to read CPLEX parameters.", false);
  }
    
  /* Turn on output to the screen */
   if (fVerbose) {
     CPXCHANNELptr chnl[4];
     CPXgetchannels(env, &chnl[0], &chnl[1], &chnl[2], &chnl[3]);
     for (int i = 0; i < 3; ++i) {
       status = CPXaddfuncdest(env, chnl[i], nullptr, msgfunction);
     }
//     status = CPXsetintparam(env, CPXPARAM_ScreenOutput,
//       fVerbose ? CPX_ON : CPX_OFF);  // also when flag_all_solutions?  TODO
//     wrap_assert(!status, "  CPLEX Warning: Failure to switch screen indicator.", false);
   }
   status = CPXsetintparam (env, CPXPARAM_MIP_Display,
                            fVerbose ? 2 : 0);  // also when flag_all_solutions?  TODO
   wrap_assert(!status, "  CPLEX Warning: Failure to switch logging.", false);
   status =  CPXsetintparam (env, CPXPARAM_ClockType, 1);            // CPU time
   wrap_assert(!status, "  CPLEX Warning: Failure to measure CPU time.", false);
   status =  CPXsetintparam (env, CPX_PARAM_MIPCBREDLP, CPX_OFF);    // Access original model
   wrap_assert(!status, "  CPLEX Warning: Failure to set access original model in callbacks.", false);
   if (sExportModel.size()) {
     status = CPXwriteprob (env, lp, sExportModel.c_str(), NULL);
     wrap_assert(!status, "Failed to write LP to disk.", false);
   }

   /// TODO
//     if(all_solutions && obj.getImpl()) {
//       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
//       _ilocplex->use(SolutionCallback(_iloenv, lastObjVal, *this));
      // Turn off CPLEX logging

   if (nThreads>0) {
     status =  CPXsetintparam (env, CPXPARAM_Threads, nThreads);
     wrap_assert(!status, "Failed to set CPXPARAM_Threads.", false);
   }

    if (nTimeout>0) {
     status =  CPXsetdblparam (env, CPXPARAM_TimeLimit, nTimeout);
     wrap_assert(!status, "Failed to set CPXPARAM_TimeLimit.", false);
    }

    if (nWorkMemLimit>0) {
     status =  CPXsetdblparam (env, CPXPARAM_MIP_Limits_TreeMemory, nWorkMemLimit);
     wrap_assert(!status, "Failed to set CPXPARAM_MIP_Limits_TreeMemory.", false);
    }

   if ( absGap>=0.0 ) {
    status =  CPXsetdblparam (env, CPXPARAM_MIP_Tolerances_AbsMIPGap, absGap);
    wrap_assert(!status, "Failed to set CPXPARAM_MIP_Tolerances_AbsMIPGap.", false);
   }
   if (relGap>=0.0) {
    status =  CPXsetdblparam (env, CPXPARAM_MIP_Tolerances_MIPGap, relGap);
    wrap_assert(!status, "Failed to set CPXPARAM_MIP_Tolerances_MIPGap.", false);
   }
   if (intTol>=0.0) {
    status =  CPXsetdblparam (env, CPXPARAM_MIP_Tolerances_Integrality, intTol);
    wrap_assert(!status, "Failed to set CPXPARAM_MIP_Tolerances_Integrality.", false);
   }

//    status =  CPXsetdblparam (env, CPXPARAM_MIP_Tolerances_ObjDifference, objDiff);
//    wrap_assert(!status, "Failed to set CPXPARAM_MIP_Tolerances_ObjDifference.", false);

    
   /// Solution callback
   output.nCols = colObj.size();
   x.resize(output.nCols);
   output.x = &x[0];
   if (flag_all_solutions && cbui.solcbfn) {
      status = CPXsetinfocallbackfunc (env, solcallback, &cbui);
      wrap_assert(!status, "Failed to set solution callback", false);
   }
  if ( cbui.cutcbfn ) {
    assert( cbui.cutMask & (MaskConsType_Usercut|MaskConsType_Lazy) );
    if ( cbui.cutMask & MaskConsType_Usercut ) {
      // For user cuts, needs to keep some info after presolve
      if ( fVerbose )
        cerr << "  MIP_cplex_wrapper: user cut callback enabled, setting params" << endl;
      CUTINFO usercutinfo;  // THREADS?  TODO
      usercutinfo.info = &cbui;
      /* Init information on the node objval for the user cut callback */
      initnodeobjvalinfo (env, lp, &usercutinfo);
      /* Assure linear mappings between the presolved and original
          models */
      status = CPXsetintparam (env, CPXPARAM_Preprocessing_Linear, 0);
      wrap_assert ( !status, "CPLEX: setting prepro_linear" );
      /* Turn on traditional search for use with control callbacks */
      status = CPXsetintparam (env, CPXPARAM_MIP_Strategy_Search,
                                CPX_MIPSEARCH_TRADITIONAL);
      wrap_assert ( !status, "CPLEX: setting traditional search" );
      /* Let MIP callbacks work on the original model */
      status = CPXsetintparam (env, CPXPARAM_MIP_Strategy_CallbackReducedLP,
                                CPX_OFF);
      wrap_assert ( !status, "CPLEX: setting callbacks to work on orig model" );
      /// And
      /* Set up to use MIP usercut callback */

      status = CPXsetusercutcallbackfunc (env, myusercutcallback, &usercutinfo);
      wrap_assert ( !status, "CPLEX: setting user cut callback" );
    }
    if ( cbui.cutMask & MaskConsType_Lazy ) {
      CUTINFO lazyconinfo;
      lazyconinfo.info = &cbui;
      /* Init information on the node objval for the user cut callback.
          No need to initialize the information on the node objval,
          for the lazy constraint callback, because those information are
          used only in the user cut callback. */
      initnodeobjvalinfo (env, lp, &lazyconinfo);
      /* Assure linear mappings between the presolved and original
          models */
      status = CPXsetintparam (env, CPXPARAM_Preprocessing_Linear, 0);
      wrap_assert ( !status, "CPLEX: setting prepro_linear" );
      /* Turn on traditional search for use with control callbacks */
//       status = CPXsetintparam (env, CPXPARAM_MIP_Strategy_Search,
//                                 CPX_MIPSEARCH_TRADITIONAL);
//       wrap_assert ( !status, "CPLEX: setting traditional search" );
      /* Let MIP callbacks work on the original model */
      status = CPXsetintparam (env, CPXPARAM_MIP_Strategy_CallbackReducedLP,
                                CPX_OFF);
      wrap_assert ( !status, "CPLEX: setting callbacks to work on orig model" );
      /* Set up to use MIP lazyconstraint callback. The callback funtion
        * registered is the same, but the data will be different. */

      status = CPXsetlazyconstraintcallbackfunc (env, myusercutcallback, &lazyconinfo);
      wrap_assert ( !status, "CPLEX: setting lazy cut callback" );
    }
  }

  /// after all modifs
    if (sWriteParams.size()) {
     status = CPXwriteparam (env, sWriteParams.c_str());
     wrap_assert(!status, "Failed to write CPLEX parameters.", false);
    }
    
   status = CPXgettime (env, &output.dCPUTime);
   wrap_assert(!status, "Failed to get time stamp.", false);
   cbui.pOutput->cCPUTime0 = std::clock();

   /* Optimize the problem and obtain solution. */
   status = CPXmipopt (env, lp);
   wrap_assert( !status,  "Failed to optimize MIP." );

   double tmNow;
   status = CPXgettime (env, &tmNow);
   wrap_assert(!status, "Failed to get time stamp.", false);
   output.dCPUTime = tmNow - output.dCPUTime;

   int solstat = CPXgetstat (env, lp);
   output.status = convertStatus(solstat);
   output.statusName = CPXgetstatstring (env, solstat, cplex_status_buffer);

   /// Continuing to fill the output object:
   if (Status::OPT == output.status || Status::SAT ==output.status) {
      status = CPXgetobjval (env, lp, &output.objVal);
      wrap_assert( !status, "No MIP objective value available." );

      /* The size of the problem should be obtained by asking CPLEX what
          the actual size is, rather than using what was passed to CPXcopylp.
          cur_numrows and cur_numcols store the current number of rows and
          columns, respectively.  */   // ?????????????? TODO

    //    int cur_numrows = CPXgetnumrows (env, lp);
      int cur_numcols = CPXgetnumcols (env, lp);
      assert(cur_numcols == colObj.size());
      
      x.resize(cur_numcols);
      output.x = &x[0];
      status = CPXgetx (env, lp, &x[0], 0, cur_numcols-1);
      wrap_assert(!status, "Failed to get variable values.");
   }
   output.bestBound = 1e308;
   status = CPXgetbestobjval (env, lp, &output.bestBound);
   wrap_assert(!status, "Failed to get the best bound.", false);
   output.nNodes = CPXgetnodecnt (env, lp);
   output.nOpenNodes = CPXgetnodeleftcnt (env, lp);
}

void MIP_cplex_wrapper::setObjSense(int s)
{
  status = CPXchgobjsen (env, lp, -s);  // +1 for min in CPLEX
  wrap_assert(!status, "Failed to set obj sense.");
}

