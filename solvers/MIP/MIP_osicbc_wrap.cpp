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
#include <CbcSolver.hpp>
#include <CbcConfig.h>
#include <ClpConfig.h>


/// Linking this module provides these functions:
MIP_wrapper* MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_osicbc_wrapper;
}

string MIP_WrapperFactory::getVersion( ) {
  string v = "  MIP wrapper for OSICBC ";
  v += CBC_VERSION;                     // E.g., 2.9 stable or 2.9.7 latest release
  v += ",  using CLP ";
  v += CLP_VERSION;
  v += "  Compiled  " __DATE__ "  " __TIME__;
  return v;
}

void MIP_WrapperFactory::printHelp(ostream& os) {
  os
  << "OSICBC MIP wrapper options:" << std::endl
  // -s                  print statistics
  //            << "  --readParam <file>  read OSICBC parameters from file
  //               << "--writeParam <file> write OSICBC parameters to file
  //               << "--tuneParam         instruct OSICBC to tune parameters instead of solving
  << "--cbcArgs \"args\"      command-line args passed to callCbc, e.g., \"-cuts off -preprocess off -passc 1\". \"-preprocess off\" recommended in 2.9.6" << std::endl
  << "--writeModel <file>   write model to <file> (.mps)" << std::endl
//   << "-a                  print intermediate solutions (use for optimization problems only TODO)" << std::endl
//   << "-p <N>              use N threads, default: 1" << std::endl
//   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>         stop search after N seconds" << std::endl
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
 
 static   string cbc_cmdOptions;


bool MIP_WrapperFactory::processOption(int& i, int argc, const char** argv) {
   if (string(argv[i])=="-a") {
     cerr << "\n  WARNING: -a: No solution callbacks implemented for coin-cbc.\n"
       "However, kill -SIGINT <pid> should work like Ctrl-C and produce final output" << endl;
//     flag_all_solutions = true;
   } else
    if (string(argv[i])=="-f") {
    std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (string(argv[i])=="--writeModel") {
    i++;
    if (i==argc) {
      goto error;
    }
    sExportModel = argv[i];
  } else if (string(argv[i])=="--cbcArgs") {
    i++;
    if (i==argc) {
      goto error;
    }
    cbc_cmdOptions += argv[i];
  }
  else if (beginswith(string(argv[i]),"-p")) {
    cerr << "\n  WARNING: -p: No multi-threading in coin-cbc." << endl;
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
  }
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
   else if (beginswith(string(argv[i]),"--workmem")) {
      cerr << "  WARNING: --workmem: not supported in coin-cbc" << endl;
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
   }
//   else if (string(argv[i])=="--readParam") {
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
  try {
    osi.addRow(cpv, rlb, rub);
  } catch (const CoinError& err) {
    cerr << "  COIN-OR Error: " << err.message() << endl;
    throw runtime_error(err.message());
  }
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

MIP_osicbc_wrapper::Status MIP_osicbc_wrapper::convertStatus(CbcModel *pModel)
{
  Status s = Status::UNKNOWN;
   /* Converting the status. */
   if (pModel->isProvenOptimal()) {
       s = Status::OPT;
       output.statusName = "Optimal";
//        wrap_assert(osi., "Optimality reported but pool empty?", false);
   } else if (pModel->isProvenInfeasible()) {
       s = Status::UNSAT;
       output.statusName = "Infeasible";
   } else if (pModel->isProvenDualInfeasible()) {
       s = Status::UNBND;
       output.statusName = "Dual infeasible";
//        s = Status::UNSATorUNBND;
   } else if   // wrong: (pModel->getColSolution())
     (fabs(pModel->getObjValue()) < 1e50)
   {
       s = Status::SAT;
       output.statusName = "Feasible";
   } else if (pModel->isAbandoned()) {     // AFTER feas-ty
       s = Status::ERROR;
       output.statusName = "Abandoned";
   } else {
     s = Status::UNKNOWN;
     output.statusName = "Unknown";
   }
   return s;
}

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
   } else if   // wrong: (pModel->getColSolution())
     (fabs(osi.getObjValue()) < osi.getInfinity())
   {
       s = Status::SAT;
       output.statusName = "Feasible";
       cout << " getSolverObjValue(as minim) == " << osi.getObjValue() << endl;
   }
   else {
     s = Status::UNKNOWN;
     output.statusName = "Unknown";
   }
   return s;
}


void MIP_osicbc_wrapper::solve() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
//       osi->loadProblem(*matrix, 
  {
      std::vector<VarId> integer_vars;
      for(unsigned int i=0; i<colObj.size(); i++) {
        if(REAL != colTypes[i]
//           && is_used[i]
        ) {
          integer_vars.push_back(i);
        }
      }
      osi.setInteger(integer_vars.data(), integer_vars.size());
  }
    if(sExportModel.size()) {
      // Not implemented for OsiClp:
//       osi.setColNames(colNames, 0, colObj.size(), 0);
      vector<const char*> colN(colObj.size());
      for (int j=0; j<colNames.size(); ++j)
        colN[j] = colNames[j].c_str();
      osi.writeMpsNative(sExportModel.c_str(), 0, colN.data());
    }
  /* Turn on output to the screen */
     class NullCoinMessageHandler : public CoinMessageHandler {
      int print() {
        return 0;
      }
      void checkSeverity() {

      }
    } nullHandler;

//     CbcSolver control(osi);
//     // initialize   ???????
//     control.fillValuesInSolver();  
//     CbcModel * pModel = control.model();
    
    CbcModel model(osi);
    
    if(fVerbose) {
//        osi.messageHandler()->setLogLevel(1);
//        osi.getModelPtr()->setLogLevel(1);
//        osi.getRealSolverPtr()->messageHandler()->setLogLevel(0);
       model.setLogLevel(1);
       model.solver()->messageHandler()->setLogLevel(0);
    } else {
      model.passInMessageHandler(&nullHandler);
      model.messageHandler()->setLogLevel(0);
      model.solver()->setHintParam(OsiDoReducePrint, true, OsiHintTry);
//       osi.passInMessageHandler(&nullHandler);
//       osi.messageHandler()->setLogLevel(0);
//       osi.setHintParam(OsiDoReducePrint, true, OsiHintTry);
    }

    if(nTimeout > 0.0) {
//       osi.setMaximumSeconds(nTimeout);
      model.setMaximumSeconds(nTimeout);
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
//       model.branchAndBound();
//       osi.branchAndBound();

      /// TAKEN FORM DRIVER3.CPP, seems to use most features:
//      CbcMain0(model);
//      CbcCbcParamUtils::setCbcModelDefaults(model) ;
//       const char * argv2[]={"mzn-cbc","-solve","-quit"};
//        CbcMain1(3,argv2,model);
      cbc_cmdOptions += " -solve";
      cbc_cmdOptions += " -quit";
//       if (fVerbose)
//         cerr << "  Calling callCbc with options '" << cbc_cmdOptions << "'..." << endl;
      callCbc(cbc_cmdOptions, model);
    } catch (CoinError& err) {
      err.print(true);
    }
    

   output.dCPUTime = (clock() - output.dCPUTime) / CLOCKS_PER_SEC;

   output.status = convertStatus(&model);
//    output.status = convertStatus();

   /// Continuing to fill the output object:
   if (Status::OPT == output.status || Status::SAT ==output.status) {
     output.objVal = model.getObjValue();
//      output.objVal = osi.getObjValue();

      /* The size of the problem should be obtained by asking OSICBC what
          the actual size is, rather than using what was passed to CBCcopylp.
          cur_numrows and cur_numcols store the current number of rows and
          columns, respectively.  */   // ?????????????? TODO

      int cur_numcols = model.getNumCols ();
//       int cur_numcols = osi.getNumCols ();
      assert(cur_numcols == colObj.size());
      
      wrap_assert(model.getColSolution(), "Failed to get variable values.");
      x.assign( model.getColSolution(), model.getColSolution() + cur_numcols ); // ColSolution();
      output.x = x.data();
//       output.x = osi.getColSolution();
   }
   output.bestBound = model.getBestPossibleObjValue();
//    output.bestBound = -1;
   output.nNodes = model.getNodeCount();
//    output.nNodes = osi.getNodeCount();
   output.nOpenNodes = -1;
}

void MIP_osicbc_wrapper::setObjSense(int s)
{
  osi.setObjSense(-s);
}

/*

try the following for example:

CbcMain0(model); 
const char * argv2[]={"driver4","-cuts","off" ,"-preprocess","off","-passc","1","-solve","-quit"};
CbcMain1(9,argv2,model);

you can add any feature you want to argv2 ...

if you want to add cuts yourself, or heuristics, do the following:

  OsiSolverInterface *solver2 = osi;
  CglPreProcess *process = new CglPreProcess;
  solver2 = process->preProcess(*solver,false,2);

    CbcModel model1(*solver2);
  
    model1.initialSolve();
    
  //==============================================
 
  CglProbing generator1;
  generator1.setUsingObjective(true);
  generator1.setMaxPass(1);
  generator1.setMaxPassRoot(5);
  generator1.setMaxProbe(10);
  generator1.setMaxProbeRoot(1000);
  generator1.setMaxLook(50);
  generator1.setMaxLookRoot(500);
  generator1.setMaxElements(200);
  generator1.setRowCuts(3);

  CglGomory generator2;
  generator2.setLimit(300);

  CglKnapsackCover generator3;

  CglRedSplit generator4;
  generator4.setLimit(200);

  CglClique generator5;
  generator5.setStarCliqueReport(false);
  generator5.setRowCliqueReport(false);

  CglMixedIntegerRounding2 mixedGen;
  CglFlowCover flowGen;
  
  CglGMI cut1;
  CglMixedIntegerRounding2 cut2;
  CglOddHole cut3;
  CglSimpleRounding cut4;
  CglResidualCapacity cut5;
  CglTwomir cut6;
  CglZeroHalf cut7;
  
  model1.addCutGenerator(&generator1,-1,"Probing");
  model1.addCutGenerator(&generator2,-1,"Gomory");
  model1.addCutGenerator(&generator3,-1,"Knapsack");
  model1.addCutGenerator(&generator4,-1,"RedSplit");
  model1.addCutGenerator(&generator5,-1,"Clique");
  model1.addCutGenerator(&flowGen,-1,"FlowCover");
  model1.addCutGenerator(&mixedGen,-1,"MixedIntegerRounding");
  model1.addCutGenerator(&cut1,-1,"GMI");
  model1.addCutGenerator(&cut2,-1,"MixedIntegerRounding2");
  model1.addCutGenerator(&cut3,-1,"OddHole");
  model1.addCutGenerator(&cut4,-1,"SimpleRounding");
  model1.addCutGenerator(&cut5,-1,"ResidualCapacity");
  model1.addCutGenerator(&cut6,-1,"Twomir");
  model1.addCutGenerator(&cut7,-1,"ZeroHalf");
  
 

  CbcRounding heuristic1(model1);
  CbcHeuristicLocal heuristic2(model1);

  
 model1.addHeuristic(&heuristic1);
 model1.addHeuristic(&heuristic2);

 
  
  
    model1.setMaximumCutPassesAtRoot(50); 
    model1.setMaximumCutPasses(1000);
  
  
  
  model1.branchAndBound();
  
  
  OsiSolverInterface * solver3;
  
  process->postProcess(*model1.solver());
  
  solver3 = solver;
  
 or, use the default strategy:

CbcStrategyDefault strategy(5);
model1.setStrategy(strategy);
 

  
  
  

On Sun, Oct 11, 2015 at 8:38 PM, Gleb Belov <gleb.belov@monash.edu> wrote:

    Hi,

    I am trying to call Cbc 2.9.6 from my program. When using the tutorial-style approach

    OsiClpSolverInterface osi;
    osi.add .......
    CbcModel model(osi);
    model.branchAndBound();

    there seem to be no cuts and other stuff applied. When using the method from the examples,

    CbcMain0(model);
    const char * argv2[]={"driver4","-solve","-quit"};
    CbcMain1(3,argv2,model);

    there are cuts applied, but obviously different (less aggressive) to the standalone Cbc executable. I also tried CbcSolver class but its method solve() is not found by the linker. So what is the 'standard' way of using the 'default' add-ons?

    Moreover. The attached example crashes both in the standalone Cbc and in the CbcCmain0/1 variant after a few minutes.

    Thanks

    _______________________________________________
    Cbc mailing list
    Cbc@list.coin-or.org
    http://list.coin-or.org/mailman/listinfo/cbc


    

Hi, what is currently good way to have a solution callback in Cbc? the 
interrupt example shows 2 ways, don't know which is right.

Moreover, it says that the solution would be given for the preprocessed 
model. Is it possible to produce one for the original? Is it possible to 
call other functions from inside, such as number of nodes, dual bound?

Thanks

From john.forrest at fastercoin.com  Thu Oct  8 10:34:15 2015
From: john.forrest at fastercoin.com (John Forrest)
Date: Thu, 8 Oct 2015 15:34:15 +0100
Subject: [Cbc] Solution callbacks
In-Reply-To: <5615F778.9020601@monash.edu>
References: <5615F778.9020601@monash.edu>
Message-ID: <56167EE7.6000607@fastercoin.com>

Gleb,

On 08/10/15 05:56, Gleb Belov wrote:
> Hi, what is currently good way to have a solution callback in Cbc? the 
> interrupt example shows 2 ways, don't know which is right.
>

It is the event handling code you would be using.
> Moreover, it says that the solution would be given for the 
> preprocessed model. Is it possible to produce one for the original? 

At present no.  In principle not difficult.  First the callback function 
would have to be modified to get passed the CglPreProcess object - 
easy.  Then in event handler you could make a copy of object and 
postsolve (you need a copy as postsolve deletes data).
> Is it possible to call other functions from inside, such as number of 
> nodes, dual bound?

Yes - you have CbcModel * model_ so things like that are available (or 
could easily be made available)

>
> Thanks
>

John Forrest


 */