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

#include <minizinc/solvers/MIP/MIP_cplex_wrap.h>

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
  << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
  << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read CPLEX parameters from file" << std::endl
  << "--writeParam <file> write CPLEX parameters to file" << std::endl
  << "--tuneParam         instruct CPLEX to tune parameters instead of solving   NOT IMPL"
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


bool MIP_WrapperFactory::processOption(int& i, int argc, const char** argv) {
  if (string(argv[i])=="-a") {
    flag_all_solutions = true;
  } else if (string(argv[i])=="-f") {
    std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (string(argv[i])=="--writeModel") {
    i++;
    if (i==argc) {
      goto error;
    }
    sExportModel = argv[i];
  } else if (beginswith(string(argv[i]),"-p")) {
    string nP(argv[i]);
    if (nP.length() > 2) {
      nP.erase(0, 2);
    } else {
      i++;
      if (i==argc) {
        goto error;
      }
      nP = argv[i];
    }
    istringstream iss(nP);
    iss >> nThreads;
    if (!iss && !iss.eof()) {
      cerr << "\nBad value for -p: " << nP << endl;
      goto error;
    }
  } else if (beginswith(string(argv[i]),"--timeout")) {
    string nP(argv[i]);
    if (nP.length() > 9) {
      nP.erase(0, 9);
    } else {
      i++;
      if (i==argc) {
        goto error;
      }
      nP = argv[i];
    }
    istringstream iss(nP);
    iss >> nTimeout;
    if (!iss && !iss.eof()) {
      cerr << "\nBad value for --timeout: " << nP << endl;
      goto error;
    }
  } else if (beginswith(string(argv[i]),"--workmem")) {
    string nP(argv[i]);
    if (nP.length() > 9) {
      nP.erase(0, 9);
    } else {
      i++;
      if (i==argc) {
        goto error;
      }
      nP = argv[i];
    }
    istringstream iss(nP);
    iss >> nWorkMemLimit;
    if (!iss && !iss.eof()) {
      cerr << "\nBad value for --workmem: " << nP << endl;
      goto error;
    }
  } else if (string(argv[i])=="--readParam") {
    i++;
    if (i==argc) {
      goto error;
    }
    sReadParams = argv[i];
  } else if (string(argv[i])=="--writeParam") {
    i++;
    if (i==argc) {
      goto error;
    }
    sWriteParams = argv[i];
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

void MIP_cplex_wrapper::addRow
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  /// Convert var types:
  char ssense=0;
    switch (sense) {
      case LQ:
        ssense = 'L';
        break;
      case EQ:
        ssense = 'E';
        break;
      case GQ:
        ssense = 'G';
        break;
      default:
        throw runtime_error("  MIP_wrapper: unknown constraint type");
    }
  const int ccnt=0;
  const int rcnt=1;
  const int rmatbeg[] = { 0 };
  char * pRName = (char*)rowName.c_str();
  // ignoring mask for now.  TODO
  status = CPXaddrows (env, lp, ccnt, rcnt, nnz, &rhs,
        &ssense, rmatbeg, rmatind, rmatval,
        NULL, &pRName);
  wrap_assert( !status,  "Failed to add constraint." );
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
                                        info->pOutput->x,
                                        0, info->pOutput->nCols-1);
      if ( status )  goto TERMINATE;

      info->pOutput->dCPUTime = -1;

      /// Call the user function:
      if (info->solcbfn)
          (*info->solcbfn)(*info->pOutput, info->ppp);
   }
   

TERMINATE:
   return (status);

} /* END logcallback */
// end SolutionCallback ---------------------------------------------------------------------

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
     case CPXMIP_ABORT_INFEAS:
     case CPXMIP_FAIL_INFEAS:
       s = Status::ERROR;
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


void MIP_cplex_wrapper::solve() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  /* Turn on output to the screen */
   status = CPXsetintparam (env, CPXPARAM_ScreenOutput,
                            fVerbose ? CPX_ON : CPX_OFF);  // also when flag_all_solutions?  TODO
   wrap_assert(!status, "  CPLEX Warning: Failure to switch screen indicator.", false);
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
    
    if (sReadParams.size()) {
     status = CPXreadcopyparam (env, sReadParams.c_str());
     wrap_assert(!status, "Failed to read CPLEX parameters.", false);
    }
    
    if (sWriteParams.size()) {
     status = CPXwriteparam (env, sWriteParams.c_str());
     wrap_assert(!status, "Failed to write CPLEX parameters.", false);
    }
    
   /// Solution callback
   output.nCols = colObj.size();
   x.resize(output.nCols);
   output.x = &x[0];
   if (flag_all_solutions && cbui.solcbfn) {
      status = CPXsetinfocallbackfunc (env, solcallback, &cbui);
      wrap_assert(!status, "Failed to set solution callback", false);
   }

   status = CPXgettime (env, &output.dCPUTime);
   wrap_assert(!status, "Failed to get time stamp.", false);

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

