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

#include <minizinc/solvers/MIP/MIP_osicbc_wrap.h>
#include <CbcConfig.h>
#include <ClpConfig.h>

/// Linking this module provides these functions:
MIP_wrapper* MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_osicbc_wrapper;
}

string MIP_WrapperFactory::getVersion( ) {
//   # Note that you should get Cbc by e.g. 'svn co https://projects.coin-or.org/svn/Cbc/releases/2.9.6 coin-Cbc-2.9.6'
//   # as opposed to their docu: the /stable/2.9 directory is different.  TODO
  string v = "  MIP wrapper for OSICBC ";
  v += CBC_VERSION;                     // Cbc_config.h says 2.8              TODO
  v += ",  using CLP ";
  v += CLP_VERSION;
  v += "  Compiled  " __DATE__ "  " __TIME__;
  return v;
}

void MIP_WrapperFactory::printHelp(ostream& os) {
  os
  << "OSICBC  MIP wrapper options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read OSICBC parameters from file
  //               << "--writeParam <file> write OSICBC parameters to file
  //               << "--tuneParam         instruct OSICBC to tune parameters instead of solving
  << "--writeModel <file> write model to <file> (.mps)" << std::endl
//   << "-a                  print intermediate solutions (use for optimization problems only TODO)" << std::endl
//   << "-p <N>              use N threads, default: 1" << std::endl
//   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
//   << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
//   << "--readParam <file>  read OSICBC parameters from file" << std::endl
//   << "--writeParam <file> write OSICBC parameters to file" << std::endl
//   << "--tuneParam         instruct OSICBC to tune parameters instead of solving   NOT IMPL"
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
//   if (string(argv[i])=="-a") {
//     flag_all_solutions = true;
//   } else
    if (string(argv[i])=="-f") {
    std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (string(argv[i])=="--writeModel") {
    i++;
    if (i==argc) {
      goto error;
    }
    sExportModel = argv[i];
  }
//   else if (beginswith(string(argv[i]),"-p")) {
//     string nP(argv[i]);
//     if (nP.length() > 2) {
//       nP.erase(0, 2);
//     } else {
//       i++;
//       if (i==argc) {
//         goto error;
//       }
//       nP = argv[i];
//     }
//     istringstream iss(nP);
//     iss >> nThreads;
//     if (!iss && !iss.eof()) {
//       cerr << "\nBad value for -p: " << nP << endl;
//       goto error;
//     }
//   }
  else if (beginswith(string(argv[i]),"--timeout")) {
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
  }
//   else if (beginswith(string(argv[i]),"--workmem")) {
//     string nP(argv[i]);
//     if (nP.length() > 9) {
//       nP.erase(0, 9);
//     } else {
//       i++;
//       if (i==argc) {
//         goto error;
//       }
//       nP = argv[i];
//     }
//     istringstream iss(nP);
//     iss >> nWorkMemLimit;
//     if (!iss && !iss.eof()) {
//       cerr << "\nBad value for --workmem: " << nP << endl;
//       goto error;
//     }
//   } else if (string(argv[i])=="--readParam") {
//     i++;
//     if (i==argc) {
//       goto error;
//     }
//     sReadParams = argv[i];
//   } else if (string(argv[i])=="--writeParam") {
//     i++;
//     if (i==argc) {
//       goto error;
//     }
//     sWriteParams = argv[i];
//   }
  else
    return false;
  return true;
error:
  return false;
}

void MIP_osicbc_wrapper::wrap_assert(bool cond, string msg, bool fTerm)
{
   if ( !cond ) {
//       strcpy(osicbc_buffer, "[NO ERROR STRING GIVEN]");
//       CBCgeterrorstring (env, status, osicbc_buffer);
      string msgAll = ("  MIP_osicbc_wrapper runtime error:  " + msg + "  " + osicbc_buffer);
      cerr << msgAll << endl;
      if (fTerm) {
        cerr << "TERMINATING." << endl;
        throw runtime_error(msgAll);
      }
   }
}

void MIP_osicbc_wrapper::doAddVars
  (size_t n, double* obj, double* lb, double* ub, MIP_wrapper::VarType* vt, string *names)
{
  /// Convert var types:
//   vector<char> ctype(n);
//   vector<char*> pcNames(n);
  CoinPackedVector cpv;
  vector<CoinPackedVectorBase*> pCpv(n, &cpv);
  osi.addCols(n, pCpv.data(), lb, ub, obj);   // setting integer & names later
//   status = CBCnewcols (env, lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
//   wrap_assert( !status,  "Failed to declare variables." );
}

void MIP_osicbc_wrapper::addRow
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  CoinPackedVector cpv(nnz, rmatind, rmatval);
  /// Convert var types:
  double rlb=rhs, rub=rhs;
  char ssense=0;
    switch (sense) {
      case LQ:
        rlb = -osi.getInfinity();
        break;
      case EQ:
        break;
      case GQ:
        rub = osi.getInfinity();
        break;
      default:
        throw runtime_error("  MIP_wrapper: unknown constraint type");
    }
  // ignoring mask for now.  TODO
  osi.addRow(cpv, rlb, rub);
}


/// SolutionCallback ------------------------------------------------------------------------
/// OSICBC ensures thread-safety?? TODO
// static int CBCPUBLIC
// solcallback (CBCCENVptr env, void *cbdata, int wherefrom, void *cbhandle)
// {
//    int status = 0;
// 
//    MIP_wrapper::CBUserInfo *info = (MIP_wrapper::CBUserInfo*) cbhandle;
//    int        hasincumbent = 0;
//    int        newincumbent = 0;
//    double objVal;
// 
//    status = CBCgetcallbackinfo (env, cbdata, wherefrom,
//                                 CBC_CALLBACK_INFO_NODE_COUNT, &info->pOutput->nNodes);
//    if ( status )  goto TERMINATE;
// 
//    status = CBCgetcallbackinfo (env, cbdata, wherefrom,
//                                 CBC_CALLBACK_INFO_NODES_LEFT, &info->pOutput->nOpenNodes);
//    if ( status )  goto TERMINATE;
// 
//    status = CBCgetcallbackinfo (env, cbdata, wherefrom,
//                                 CBC_CALLBACK_INFO_MIP_FEAS, &hasincumbent);
//    if ( status )  goto TERMINATE;
// 
//    if ( hasincumbent ) {
//       status = CBCgetcallbackinfo (env, cbdata, wherefrom,
//                                    CBC_CALLBACK_INFO_BEST_INTEGER, &objVal);
//       if ( status )  goto TERMINATE;
//       
//       if ( fabs(info->pOutput->objVal - objVal) > 1e-12*(1.0 + fabs(objVal)) ) {
//          newincumbent = 1;
//          info->pOutput->objVal = objVal;
//         info->pOutput->status = MIP_wrapper::SAT;
//         info->pOutput->statusName = "feasible from a callback";
// 
//       }
//    }
// 
// //    if ( nodecnt >= info->lastlog + 100  ||  newincumbent ) {
// //       double walltime;
// //       double dettime;
// 
//       status = CBCgetcallbackinfo (env, cbdata, wherefrom,
//                                    CBC_CALLBACK_INFO_BEST_REMAINING, &info->pOutput->bestBound);
// //       if ( status )  goto TERMINATE;
// 
// //       status = CBCgettime (env, &walltime);
// //       if ( status )  goto TERMINATE;
// // 
// //       status = CBCgetdettime (env, &dettime);
// //       if ( status )  goto TERMINATE;
// // 
// //    }
// 
//    if ( newincumbent ) {
//       assert(info->pOutput->x);
//       status = CBCgetcallbackincumbent (env, cbdata, wherefrom,
//                                         info->pOutput->x,
//                                         0, info->pOutput->nCols-1);
//       if ( status )  goto TERMINATE;
// 
//       info->pOutput->dCPUTime = -1;
// 
//       /// Call the user function:
//       if (info->solcbfn)
//           (*info->solcbfn)(*info->pOutput, info->ppp);
//    }
//    
// 
// TERMINATE:
//    return (status);
// 
// } /* END logcallback */
// end SolutionCallback ---------------------------------------------------------------------

MIP_osicbc_wrapper::Status MIP_osicbc_wrapper::convertStatus()
{
  Status s = Status::UNKNOWN;
   /* Converting the status. */
   if (osi.isProvenOptimal()) {
       s = Status::OPT;
       output.statusName = "Optimal";
//        wrap_assert(osi., "Optimality reported but pool empty?", false);
   } else if (osi.isProvenPrimalInfeasible()) {
       s = Status::UNSAT;
       output.statusName = "Infeasible";
   } else if (osi.isProvenDualInfeasible()) {
       s = Status::UNBND;
       output.statusName = "Dual infeasible";
//        s = Status::UNSATorUNBND;
   } else if (osi.isAbandoned()) {
       s = Status::ERROR;
       output.statusName = "Abandoned";
   } else if (osi.getColSolution()) {
       s = Status::SAT;
       output.statusName = "Feasible";
   }
   else {
     s = Status::UNKNOWN;
     output.statusName = "Unknown";
   }
   return s;
}


void MIP_osicbc_wrapper::solve() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  /* Turn on output to the screen */
     class NullCoinMessageHandler : public CoinMessageHandler {
      int print() {
        return 0;
      }
      void checkSeverity() {

      }
    } nullHandler;

    if(fVerbose) {
       osi.getModelPtr()->messageHandler()->setLogLevel(1);
//        osi.getRealSolverPtr()->messageHandler()->setLogLevel(1);
    } else {
      osi.passInMessageHandler(&nullHandler);
      osi.messageHandler()->setLogLevel(0);
      osi.setHintParam(OsiDoReducePrint, true, OsiHintTry);
    }

    if(nTimeout > 0.0) {
      osi.setMaximumSeconds(nTimeout);
    }

    if(sExportModel.size()) {
      osi.setColNames(colNames, 0, colObj.size(), 0);
      osi.writeMps(sExportModel.c_str());
    }

   /// TODO
//     if(all_solutions && obj.getImpl()) {
//       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
//       _iloosicbc->use(SolutionCallback(_iloenv, lastObjVal, *this));
      // Turn off OSICBC logging

    
   /// Solution callback
//    output.nCols = colObj.size();
//    x.resize(output.nCols);
//    output.x = &x[0];

//    if (flag_all_solutions && cbui.solcbfn) {
//       status = CBCsetinfocallbackfunc (env, solcallback, &cbui);
//       wrap_assert(!status, "Failed to set solution callback", false);
//    }

   output.dCPUTime = clock();

   /* Optimize the problem and obtain solution. */
    try {
//       osi->loadProblem(*matrix,
      std::vector<VarId> integer_vars;
      for(unsigned int i=0; i<colObj.size(); i++) {
        if(REAL != colTypes[i]
//           && is_used[i]
        ) {
          integer_vars.push_back(i);
        }
      }
      osi.setInteger(integer_vars.data(), integer_vars.size());
      osi.branchAndBound();
    } catch (CoinError& err) {
      err.print(true);
    }

   output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;

   output.status = convertStatus();

   /// Continuing to fill the output object:
   if (Status::OPT == output.status || Status::SAT ==output.status) {
     output.objVal = osi.getObjValue();

      /* The size of the problem should be obtained by asking OSICBC what
          the actual size is, rather than using what was passed to CBCcopylp.
          cur_numrows and cur_numcols store the current number of rows and
          columns, respectively.  */   // ?????????????? TODO

    //    int cur_numrows = CBCgetnumrows (env, lp);
      int cur_numcols = osi.getNumCols ();
      assert(cur_numcols == colObj.size());
      
      output.x = osi.getColSolution();
      wrap_assert(output.x, "Failed to get variable values.");
   }
   output.bestBound = osi.getModelPtr()->getBestPossibleObjValue();
   output.nNodes = osi.getNodeCount();
   output.nOpenNodes = -1;
}

void MIP_osicbc_wrapper::setObjSense(int s)
{
  osi.setObjSense(-s);
}

