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
#include <ctime>
#include <cmath>
#include <stdexcept>

using namespace std;

#include <minizinc/solvers/MIP/MIP_gurobi_wrap.h>

/// Linking this module provides these functions:
MIP_wrapper* MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_gurobi_wrapper;
}

string MIP_WrapperFactory::getVersion( ) {
  ostringstream oss;
  oss << "  MIP wrapper for Gurobi library ";
  int major, minor, technical;
  GRBversion(&major, &minor, &technical);
  oss << major << '.' << minor << '.' << technical;
  oss << ".  Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}

void MIP_WrapperFactory::printHelp(ostream& os) {
  os
  << "GUROBI MIP wrapper options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read GUROBI parameters from file
  //               << "--writeParam <file> write GUROBI parameters to file
  //               << "--tuneParam         instruct GUROBI to tune parameters instead of solving
  << "--writeModel <file> write model to <file> (.lp, .mps, .sav, ...)" << std::endl
  << "-a                  print intermediate solutions (use for optimization problems only TODO)" << std::endl
  << "-p <N>              use N threads, default: 1" << std::endl
  << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
//   << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read GUROBI parameters from file" << std::endl
  << "--writeParam <file> write GUROBI parameters to file" << std::endl
  << "--tuneParam         instruct GUROBI to tune parameters instead of solving   NOT IMPL"
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

void MIP_gurobi_wrapper::wrap_assert(bool cond, string msg, bool fTerm)
{
   if ( !cond ) {
      gurobi_buffer = "[NO ERROR STRING GIVEN]";
      if (error) {
         gurobi_buffer = GRBgeterrormsg(env);
      }
      string msgAll = ("  MIP_gurobi_wrapper runtime error:  " + msg + "  " + gurobi_buffer);
      cerr << msgAll << endl;
      if (fTerm) {
        cerr << "TERMINATING." << endl;
        throw runtime_error(msgAll);
      }
   }
}

void MIP_gurobi_wrapper::openGUROBI()
{
   /* Initialize the GUROBI environment */
   error = GRBloadenv (&env, "mzn-gurobi.log");
   wrap_assert ( !error, "Could not open GUROBI environment." );
   /* Create the problem. */
   error = GRBnewmodel(env, &model, "mzn_gurobi", 0, NULL, NULL, NULL, NULL, NULL);
   wrap_assert ( model, "Failed to create LP." );
}

void MIP_gurobi_wrapper::closeGUROBI()
{
  /// Freeing the problem can be slow both in C and C++, see IBM forums. Skipping.
     /* Free up the problem as allocated by GRB_createprob, if necessary */
  /* Free model */

//   GRBfreemodel(model);
  model = 0;

  /* Free environment */

  if (env)
    GRBfreeenv(env);
  /// and at last:
//   MIP_wrapper::cleanup();
}

void MIP_gurobi_wrapper::doAddVars
  (size_t n, double* obj, double* lb, double* ub, MIP_wrapper::VarType* vt, string *names)
{
  /// Convert var types:
  vector<char> ctype(n);
  vector<char*> pcNames(n);
  for (size_t i=0; i<n; ++i) {
    pcNames[i] = (char*)names[i].c_str();
    switch (vt[i]) {
      case REAL:
        ctype[i] = GRB_CONTINUOUS;
        break;
      case INT:
        ctype[i] = GRB_INTEGER;
        break;
      case BINARY:
        ctype[i] = GRB_BINARY;
        break;
      default:
        throw runtime_error("  MIP_wrapper: unknown variable type");
    }
  }
  error = GRBaddvars(model, n, 0, NULL, NULL, NULL, obj, lb, ub, &ctype[0], &pcNames[0]);
  wrap_assert( !error,  "Failed to declare variables." );
  error = GRBupdatemodel(model);
  wrap_assert( !error,  "Failed to update model." );
}

void MIP_gurobi_wrapper::addRow
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  /// Convert var types:
  char ssense=0;
    switch (sense) {
      case LQ:
        ssense = GRB_LESS_EQUAL;
        break;
      case EQ:
        ssense = GRB_EQUAL;
        break;
      case GQ:
        ssense = GRB_GREATER_EQUAL;
        break;
      default:
        throw runtime_error("  MIP_wrapper: unknown constraint type");
    }
  const int ccnt=0;
  const int rcnt=1;
  const int rmatbeg[] = { 0 };
  const char * pRName = rowName.c_str();
  // ignoring mask for now.  TODO
  error = GRBaddconstr(model, nnz, rmatind, rmatval, ssense, rhs, pRName);
  wrap_assert( !error,  "Failed to add constraint." );
}

/// SolutionCallback ------------------------------------------------------------------------
/// CPLEX ensures thread-safety
static int __stdcall
solcallback(GRBmodel *model,
           void     *cbdata,
           int       where,
           void     *usrdata)
{
    MIP_wrapper::CBUserInfo *info = (MIP_wrapper::CBUserInfo*) usrdata;
    double nodecnt, actnodes, objVal;
    int    solcnt;
    int    newincumbent=0;

    if (where == GRB_CB_MIP) {
        /* General MIP callback */
        GRBcbget(cbdata, where, GRB_CB_MIP_OBJBND, &info->pOutput->bestBound);
          GRBcbget(cbdata, where, GRB_CB_MIP_NODLFT, &actnodes);
       info->pOutput->nOpenNodes = actnodes;
    }
    if (where != GRB_CB_MIPSOL)
      return 0;
    
    /* MIP solution callback */
 
    GRBcbget(cbdata, where, GRB_CB_MIPSOL_NODCNT, &nodecnt);
    info->pOutput->nNodes = nodecnt;
    GRBcbget(cbdata, where, GRB_CB_MIPSOL_OBJ, &objVal);
    GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOLCNT, &solcnt);

   if ( solcnt ) {
      
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

//       status = CPXgettime (env, &walltime);
//       if ( status )  goto TERMINATE;
// 
//       status = CPXgetdettime (env, &dettime);
//       if ( status )  goto TERMINATE;
// 
//    }

   if ( newincumbent ) {
      assert(info->pOutput->x);
      GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, (void*)info->pOutput->x);
      
      info->pOutput->dCPUTime = -1;

      /// Call the user function:
      if (info->solcbfn)
          (*info->solcbfn)(*info->pOutput, info->ppp);
   }
   
   return 0;
} /* END logcallback */
// end SolutionCallback ---------------------------------------------------------------------


MIP_gurobi_wrapper::Status MIP_gurobi_wrapper::convertStatus(int gurobiStatus)
{
  Status s = Status::UNKNOWN;
  ostringstream oss;
   /* Converting the status. */
  if (gurobiStatus == GRB_OPTIMAL) {
    s = Status::OPT;
    oss << "Optimal";
  } else if (gurobiStatus == GRB_INF_OR_UNBD) {
    s = Status::UNSATorUNBND;
    oss << "Infeasible or unbounded";
  } else if (gurobiStatus == GRB_INFEASIBLE) {
    s = Status::UNSAT;
    oss << "Infeasible";
  } else if (gurobiStatus == GRB_UNBOUNDED) {
    oss << "Unbounded";
    s = Status::UNBND;
  } else {
    int solcount=0;
    error = GRBgetintattr(model, "SolCount", &solcount);
    wrap_assert(!error, "  Failure to access solution count.", false);
    if (solcount)
      s = Status::SAT;
    oss << "Gurobi stopped with status " << gurobiStatus;
  }
  output.statusName = gurobi_status_buffer = oss.str();
  return s;
}


void MIP_gurobi_wrapper::solve() {  // Move into ancestor?
  error = GRBupdatemodel(model);                  // for model export
  wrap_assert( !error,  "Failed to update model." );

  /////////////// Last-minute solver options //////////////////
  /* Turn on output to the screen */
   error = GRBsetintparam(env, "OutputFlag", 
                            fVerbose ? 1 : 0);  // also when flag_all_solutions?  TODO
   wrap_assert(!error, "  GUROBI Warning: Failure to switch screen indicator.", false);
//    error =  GRB_setintparam (env, GRB_PARAM_ClockType, 1);            // CPU time
//    error =  GRB_setintparam (env, GRB_PARAM_MIP_Strategy_CallbackReducedLP, GRB__OFF);    // Access original model
   if (sExportModel.size()) {
     error = GRBwrite(model, sExportModel.c_str());
     wrap_assert(!error, "Failed to write LP to disk.", false);
   }

   /// TODO
//     if(all_solutions && obj.getImpl()) {
//       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
//       _ilogurobi->use(SolutionCallback(_iloenv, lastObjVal, *this));
      // Turn off GUROBI logging

   if (nThreads>0) {
     error = GRBsetintparam(env, GRB_INT_PAR_THREADS, nThreads);
//      int nn;    // THE SETTING FAILS TO WORK IN 6.0.5.
//      error = GRBgetintparam(env, GRB_INT_PAR_THREADS, &nn);
//      cerr << "Set " << nThreads << " threads, reported " << nn << endl;
     wrap_assert(!error, "Failed to set GRB_INT_PAR_THREADS.", false);
   }

    if (nTimeout>0) {
     error = GRBsetdblparam(env, GRB_DBL_PAR_TIMELIMIT, nTimeout);
     wrap_assert(!error, "Failed to set GRB_PARAM_TimeLimit.", false);
    }

//     if (nWorkMemLimit>0) {
//      error =  GRB_setdblparam (env, GRB_PARAM_MIP_Limits_TreeMemory, nWorkMemLimit);
//      wrap_assert(!error, "Failed to set GRB_PARAM_MIP_Limits_TreeMemory.", false);
//     }
    
    if (sReadParams.size()) {
     error = GRBreadparams (env, sReadParams.c_str());
     wrap_assert(!error, "Failed to read GUROBI parameters.", false);
    }
    
    if (sWriteParams.size()) {
     error = GRBwriteparams (env, sWriteParams.c_str());
     wrap_assert(!error, "Failed to write GUROBI parameters.", false);
    }

       /// Solution callback
   output.nCols = colObj.size();
   x.resize(output.nCols);
   output.x = &x[0];
   if (flag_all_solutions && cbui.solcbfn) {
      error = GRBsetcallbackfunc(model, solcallback, (void *) &cbui);
      wrap_assert(!error, "Failed to set solution callback", false);
   }

   output.dCPUTime = std::clock();

   /* Optimize the problem and obtain solution. */
   error = GRBoptimize(model);
   wrap_assert( !error,  "Failed to optimize MIP." );

   output.dCPUTime = (std::clock() - output.dCPUTime) / CLOCKS_PER_SEC;

   int solstat;
   error = GRBgetintattr(model, GRB_INT_ATTR_STATUS, &solstat);
   wrap_assert(!error, "Failed to get MIP status.", false);
   output.status = convertStatus(solstat);

   /// Continuing to fill the output object:
   if (Status::OPT == output.status || Status::SAT ==output.status) {
      error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &output.objVal);
      wrap_assert( !error, "No MIP objective value available." );

    //    int cur_numrows = GRB_getnumrows (env, lp);
      int cur_numcols = getNCols();
      assert(cur_numcols == colObj.size());
      
      x.resize(cur_numcols);
      output.x = &x[0];
      error = GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cur_numcols, (double*)output.x);
      wrap_assert(!error, "Failed to get variable values.");
   }
   output.bestBound = 1e308;
   error = GRBgetdblattr(model, GRB_DBL_ATTR_OBJBOUNDC, &output.bestBound);
   wrap_assert(!error, "Failed to get the best bound.", false);
   double nNodes=-1;
   error = GRBgetdblattr(model, GRB_DBL_ATTR_NODECOUNT, &nNodes);
   output.nNodes = nNodes;
   output.nOpenNodes = 0;
}

void MIP_gurobi_wrapper::setObjSense(int s)
{
  error = GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE,
                        s>0 ? GRB_MAXIMIZE : GRB_MINIMIZE);
  wrap_assert(!error, "Failed to set obj sense.");
}

