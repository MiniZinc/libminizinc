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

#include <minizinc/config.hh>
#include <minizinc/exception.hh>

#ifdef HAS_GUROBI_PLUGIN
#ifdef HAS_DLFCN_H
#include <dlfcn.h>
#elif defined HAS_WINDOWS_H
#include <Windows.h>
#endif
#endif

using namespace std;

#include <minizinc/solvers/MIP/MIP_gurobi_wrap.hh>
#include <minizinc/utils.hh>

/// Linking this module provides these functions:
MIP_wrapper* MIP_WrapperFactory::GetDefaultMIPWrapper() {
  return new MIP_gurobi_wrapper;
}

string MIP_WrapperFactory::getVersion( ) {
  ostringstream oss;
  oss << "  MIP wrapper for Gurobi library ";
  MIP_gurobi_wrapper mgw( (int) 5 );
  mgw.checkDLL();
  int major, minor, technical;
  mgw.dll_GRBversion(&major, &minor, &technical);
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
  << "-p <N>              use N threads, default: 1." << std::endl
//   << "--nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
  << "--timeout <N>       stop search after N seconds" << std::endl
//   << "--workmem <N>       maximal amount of RAM used, MB" << std::endl
  << "--readParam <file>  read GUROBI parameters from file" << std::endl
  << "--writeParam <file> write GUROBI parameters to file" << std::endl
//   << "--tuneParam         instruct GUROBI to tune parameters instead of solving   NOT IMPL"

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

void MIP_gurobi_wrapper::wrap_assert(bool cond, string msg, bool fTerm)
{
   if ( !cond ) {
      gurobi_buffer = "[NO ERROR STRING GIVEN]";
      if (error) {
         gurobi_buffer = dll_GRBgeterrormsg(env);
      }
      string msgAll = ("  MIP_gurobi_wrapper runtime error:  " + msg + "  " + gurobi_buffer);
      cerr << msgAll << endl;
      if (fTerm) {
        cerr << "TERMINATING." << endl;
        throw runtime_error(msgAll);
      }
   }
}

#ifdef HAS_GUROBI_PLUGIN

namespace {
  void* dll_open(const char* file) {
#ifdef HAS_DLFCN_H
    return dlopen( (std::string("lib")+file+".so").c_str(), RTLD_NOW);
#else
    return LoadLibrary((std::string(file)+".dll").c_str());
#endif
  }
  void* dll_sym(void* dll, const char* sym) {
#ifdef HAS_DLFCN_H
    return dlsym(dll, sym);
#else
    return GetProcAddress((HMODULE)dll, sym);
#endif
  }
  void dll_close(void* dll) {
#ifdef HAS_DLFCN_H
    dlclose(dll);
#else
    FreeLibrary((HMODULE)dll);
#endif
  }
}

#endif

void MIP_gurobi_wrapper::checkDLL()
{
#ifdef HAS_GUROBI_PLUGIN
  
  gurobi_dll = dll_open("gurobi70");
  if (gurobi_dll==NULL) {
    gurobi_dll = dll_open("gurobi65");
  }

  if (gurobi_dll==NULL) {
    throw MiniZinc::InternalError("cannot load gurobi dll");
  }
  
  *(void**)(&dll_GRBversion) = dll_sym(gurobi_dll, "GRBversion");
  *(void**)(&dll_GRBaddconstr) = dll_sym(gurobi_dll, "GRBaddconstr");
  *(void**)(&dll_GRBaddvars) = dll_sym(gurobi_dll, "GRBaddvars");
  *(void**)(&dll_GRBcbcut) = dll_sym(gurobi_dll, "GRBcbcut");
  *(void**)(&dll_GRBcbget) = dll_sym(gurobi_dll, "GRBcbget");
  *(void**)(&dll_GRBcblazy) = dll_sym(gurobi_dll, "GRBcblazy");
  *(void**)(&dll_GRBfreeenv) = dll_sym(gurobi_dll, "GRBfreeenv");
  *(void**)(&dll_GRBfreemodel) = dll_sym(gurobi_dll, "GRBfreemodel");
  *(void**)(&dll_GRBgetdblattr) = dll_sym(gurobi_dll, "GRBgetdblattr");
  *(void**)(&dll_GRBgetdblattrarray) = dll_sym(gurobi_dll, "GRBgetdblattrarray");
  *(void**)(&dll_GRBgetenv) = dll_sym(gurobi_dll, "GRBgetenv");
  *(void**)(&dll_GRBgeterrormsg) = dll_sym(gurobi_dll, "GRBgeterrormsg");
  *(void**)(&dll_GRBgetintattr) = dll_sym(gurobi_dll, "GRBgetintattr");
  *(void**)(&dll_GRBloadenv) = dll_sym(gurobi_dll, "GRBloadenv");
  *(void**)(&dll_GRBnewmodel) = dll_sym(gurobi_dll, "GRBnewmodel");
  *(void**)(&dll_GRBoptimize) = dll_sym(gurobi_dll, "GRBoptimize");
  *(void**)(&dll_GRBreadparams) = dll_sym(gurobi_dll, "GRBreadparams");
  *(void**)(&dll_GRBsetcallbackfunc) = dll_sym(gurobi_dll, "GRBsetcallbackfunc");
  *(void**)(&dll_GRBsetdblparam) = dll_sym(gurobi_dll, "GRBsetdblparam");
  *(void**)(&dll_GRBsetintattr) = dll_sym(gurobi_dll, "GRBsetintattr");
  *(void**)(&dll_GRBsetintattrlist) = dll_sym(gurobi_dll, "GRBsetintattrlist");
  *(void**)(&dll_GRBsetintparam) = dll_sym(gurobi_dll, "GRBsetintparam");
  *(void**)(&dll_GRBsetstrparam) = dll_sym(gurobi_dll, "GRBsetstrparam");
  *(void**)(&dll_GRBupdatemodel) = dll_sym(gurobi_dll, "GRBupdatemodel");
  *(void**)(&dll_GRBwrite) = dll_sym(gurobi_dll, "GRBwrite");
  *(void**)(&dll_GRBwriteparams) = dll_sym(gurobi_dll, "GRBwriteparams");

#else

  dll_GRBversion = GRBversion;
  dll_GRBaddconstr = GRBaddconstr;
  dll_GRBaddvars = GRBaddvars;
  dll_GRBcbcut = GRBcbcut;
  dll_GRBcbget = GRBcbget;
  dll_GRBcblazy = GRBcblazy;
  dll_GRBfreeenv = GRBfreeenv;
  dll_GRBfreemodel = GRBfreemodel;
  dll_GRBgetdblattr = GRBgetdblattr;
  dll_GRBgetdblattrarray = GRBgetdblattrarray;
  dll_GRBgetenv = GRBgetenv;
  dll_GRBgeterrormsg = GRBgeterrormsg;
  dll_GRBgetintattr = GRBgetintattr;
  dll_GRBloadenv = GRBloadenv;
  dll_GRBnewmodel = GRBnewmodel;
  dll_GRBoptimize = GRBoptimize;
  dll_GRBreadparams = GRBreadparams;
  dll_GRBsetcallbackfunc = GRBsetcallbackfunc;
  dll_GRBsetdblparam = GRBsetdblparam;
  dll_GRBsetintattr = GRBsetintattr;
  dll_GRBsetintattrlist = GRBsetintattrlist;
  dll_GRBsetintparam = GRBsetintparam;
  dll_GRBsetstrparam = GRBsetstrparam;
  dll_GRBupdatemodel = GRBupdatemodel;
  dll_GRBwrite = GRBwrite;
  dll_GRBwriteparams = GRBwriteparams;
  
#endif
}


void MIP_gurobi_wrapper::openGUROBI()
{
  checkDLL();
  
  cbui.wrapper = this;
  
   /* Initialize the GUROBI environment */
   error = dll_GRBloadenv (&env, "mzn-gurobi.log");
   wrap_assert ( !error, "Could not open GUROBI environment." );
   error = dll_GRBsetintparam(env, "OutputFlag", 0);  // Switch off output
//    error = dll_GRBsetintparam(env, "LogToConsole",
//                             fVerbose ? 1 : 0);  // also when flag_all_solutions?  TODO
  /* Create the problem. */
   error = dll_GRBnewmodel(env, &model, "mzn_gurobi", 0, NULL, NULL, NULL, NULL, NULL);
   wrap_assert ( model, "Failed to create LP." );
}

void MIP_gurobi_wrapper::closeGUROBI()
{
  /// Freeing the problem can be slow both in C and C++, see IBM forums. Skipping.
     /* Free up the problem as allocated by GRB_createprob, if necessary */
  /* Free model */

  dll_GRBfreemodel(model);
  model = 0;

  /* Free environment */

  if (env)
    dll_GRBfreeenv(env);
  /// and at last:
//   MIP_wrapper::cleanup();
#ifdef HAS_GUROBI_PLUGIN
  dll_close(gurobi_dll);
#endif
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
  error = dll_GRBaddvars(model, n, 0, NULL, NULL, NULL, obj, lb, ub, &ctype[0], &pcNames[0]);
  wrap_assert( !error,  "Failed to declare variables." );
  error = dll_GRBupdatemodel(model);
  wrap_assert( !error,  "Failed to update model." );
}

static char getGRBSense( MIP_wrapper::LinConType s ) {
    switch (s) {
      case MIP_wrapper::LQ:
        return GRB_LESS_EQUAL;
      case MIP_wrapper::EQ:
        return GRB_EQUAL;
      case MIP_wrapper::GQ:
        return GRB_GREATER_EQUAL;
      default:
        throw runtime_error("  MIP_gurobi_wrapper: unknown constraint sense");
    }
}

void MIP_gurobi_wrapper::addRow
  (int nnz, int* rmatind, double* rmatval, MIP_wrapper::LinConType sense,
   double rhs, int mask, string rowName)
{
  //// Make sure:
  ++ nRows;
  /// Convert var types:
  char ssense=getGRBSense(sense);
  const int ccnt=0;
  const int rcnt=1;
  const int rmatbeg[] = { 0 };
  const char * pRName = rowName.c_str();
  error = dll_GRBaddconstr(model, nnz, rmatind, rmatval, ssense, rhs, pRName);
  wrap_assert( !error,  "Failed to add constraint." );
  int nLazyAttr=0;
  const bool fUser = (MaskConsType_Usercut & mask);
  const bool fLazy = (MaskConsType_Lazy & mask);
  /// Gurobi 6.5.2 has lazyness 1-3.
  if (fUser) {
    if (fLazy)
      nLazyAttr = 2;  // just active lazy
    else
      nLazyAttr = 3;  // even LP-active
  } else
    if (fLazy)
      nLazyAttr = 1;  // very lazy
  if (nLazyAttr) {
    nLazyIdx.push_back( nRows-1 );
    nLazyValue.push_back( nLazyAttr );
  }
}

/// SolutionCallback ------------------------------------------------------------------------
/// Gurobi ensures thread-safety
static int __stdcall
solcallback(GRBmodel *model,
           void     *cbdata,
           int       where,
           void     *usrdata)
{
  MIP_wrapper::CBUserInfo *info = (MIP_wrapper::CBUserInfo*) usrdata;
  MIP_gurobi_wrapper* gw = static_cast<MIP_gurobi_wrapper*>(info->wrapper);
  
  double nodecnt=0.0, actnodes=0.0, objVal=0.0;
  int    solcnt=0;
  int    newincumbent=0;

  if ( GRB_CB_MIP==where ) {
      /* General MIP callback */
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIP_OBJBND, &info->pOutput->bestBound);
        gw->dll_GRBcbget(cbdata, where, GRB_CB_MIP_NODLFT, &actnodes);
      info->pOutput->nOpenNodes = actnodes;
  } else if ( GRB_CB_MESSAGE==where ) {
    /* Message callback */
    if ( info->fVerb ) {
      char *msg;
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MSG_STRING, &msg);
      cerr << msg << flush;
    }
  } else if ( GRB_CB_MIPSOL==where ) {
      /* MIP solution callback */
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_NODCNT, &nodecnt);
      info->pOutput->nNodes = nodecnt;
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_OBJ, &objVal);
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOLCNT, &solcnt);

      if ( solcnt ) {
        
        if ( fabs(info->pOutput->objVal - objVal) > 1e-12*(1.0 + fabs(objVal)) ) {
          newincumbent = 1;
          info->pOutput->objVal = objVal;
          info->pOutput->status = MIP_wrapper::SAT;
          info->pOutput->statusName = "feasible from a callback";
        }
      }
    if ( newincumbent ) {
        assert(info->pOutput->x);
        gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, (void*)info->pOutput->x);
        
        info->pOutput->dCPUTime = double(std::clock() - info->pOutput->cCPUTime0) / CLOCKS_PER_SEC;

        /// Call the user function:
        if (info->solcbfn)
            (*info->solcbfn)(*info->pOutput, info->ppp);
    }
    /// Callback for lazy cuts
    if ( info->cutcbfn && info->cutMask&MIP_wrapper::MaskConsType_Lazy ) {
      MIP_wrapper::CutInput cutInput;
      info->cutcbfn( *info->pOutput, cutInput, info->ppp, true );
      for ( auto& cd : cutInput ) {
//         assert( cd.mask & MIP_wrapper::MaskConsType_Lazy );
        if ( cd.mask & MIP_wrapper::MaskConsType_Lazy ) {
          int error = gw->dll_GRBcblazy(cbdata, cd.rmatind.size(),
                  cd.rmatind.data(), cd.rmatval.data(), 
                  getGRBSense(cd.sense), cd.rhs);
          if (error)
            cerr << "  GRB_wrapper: failed to add lazy cut. " << endl;
        }
      }
    }
  } else if ( GRB_CB_MIPNODE==where  ) {
    int status;
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_STATUS, &status);
    if ( status == GRB_OPTIMAL && info->cutcbfn ) {    // if cut handler given
      MIP_wrapper::Output outpRlx;
      outpRlx.x = info->pOutput->x;  // using the sol output storage  TODO?
      outpRlx.nCols = info->pOutput->nCols;
      assert( outpRlx.x && outpRlx.nCols );
//       dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_RELOBJ, outpRlx.objVal);
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_REL, (void*)outpRlx.x);
      MIP_wrapper::CutInput cutInput;
      info->cutcbfn( outpRlx, cutInput, info->ppp, false );
//       static int nCuts=0;
//       nCuts += cutInput.size();
//       if ( cutInput.size() )
//         cerr << "\n   N CUTS:  " << nCuts << endl;
      for ( auto& cd : cutInput ) {
        assert( cd.mask &
          (MIP_wrapper::MaskConsType_Usercut|MIP_wrapper::MaskConsType_Lazy) );
        if ( cd.mask & MIP_wrapper::MaskConsType_Usercut ) {
          int error = gw->dll_GRBcbcut(cbdata, cd.rmatind.size(),
                  cd.rmatind.data(), cd.rmatval.data(), 
                  getGRBSense(cd.sense), cd.rhs);
          if (error)
            cerr << "  GRB_wrapper: failed to add user cut. " << endl;
        }
        if ( cd.mask & MIP_wrapper::MaskConsType_Lazy ) {
          int error = gw->dll_GRBcblazy(cbdata, cd.rmatind.size(),
                  cd.rmatind.data(), cd.rmatval.data(), 
                  getGRBSense(cd.sense), cd.rhs);
          if (error)
            cerr << "  GRB_wrapper: failed to add lazy cut. " << endl;
        }
      }
    }
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
    error = dll_GRBgetintattr(model, "SolCount", &solcount);
    wrap_assert(!error, "  Failure to access solution count.", false);
    if (solcount)
      s = Status::SAT;
    oss << "Gurobi stopped with status " << gurobiStatus;
  }
  output.statusName = gurobi_status_buffer = oss.str();
  return s;
}


void MIP_gurobi_wrapper::solve() {  // Move into ancestor?
  if ( flag_all_solutions && 0==nProbType )
    cerr << "WARNING. --all-solutions for SAT problems not implemented." << endl;
  
   error = dll_GRBupdatemodel(model);                  // for model export
   wrap_assert( !error,  "Failed to update model." );
   
   /// ADDING LAZY CONSTRAINTS IF ANY
   if ( nLazyIdx.size() ) {
     assert( nLazyIdx.size()==nLazyValue.size() );
      if ( fVerbose )
         cerr << "  MIP_gurobi_wrapper: marking "<<nLazyIdx.size()
           <<" lazy cuts." << endl;
      error = dll_GRBsetintattrlist(model, "Lazy", nLazyIdx.size(), nLazyIdx.data(), nLazyValue.data());
      wrap_assert( !error,  "Failed to set constraint attribute." );
      nLazyIdx.clear();
      nLazyValue.clear();
      error = dll_GRBupdatemodel(model);                  // for model export
      wrap_assert( !error,  "Failed to update model after modifying some constraint attr." );
   }

  /////////////// Last-minute solver options //////////////////
  /* Turn on output to file */
   error = dll_GRBsetstrparam(dll_GRBgetenv(model), "LogFile", "");  // FAILS to switch off in Ubuntu 15.04
  /* Turn on output to the screen */
   error = dll_GRBsetintparam(dll_GRBgetenv(model), "OutputFlag",
                             /*fVerbose ? 1 :*/ 0);  // switch off, redirect in callback
//    error = dll_GRBsetintparam(dll_GRBgetenv(model), "LogToConsole",
//                             fVerbose ? 1 : 0);  // also when flag_all_solutions?  TODO
   wrap_assert(!error, "  GUROBI Warning: Failure to switch screen indicator.", false);
//    error =  dll_GRB_setintparam (env, GRB_PARAM_ClockType, 1);            // CPU time
//    error =  dll_GRB_setintparam (env, GRB_PARAM_MIP_Strategy_CallbackReducedLP, GRB__OFF);    // Access original model
   if (sExportModel.size()) {
     error = dll_GRBwrite(model, sExportModel.c_str());
     wrap_assert(!error, "Failed to write LP to disk.", false);
   }

   /// TODO
//     if(all_solutions && obj.getImpl()) {
//       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
//       _ilogurobi->use(SolutionCallback(_iloenv, lastObjVal, *this));
      // Turn off GUROBI logging

   if (nThreads>0) {
     error = dll_GRBsetintparam(dll_GRBgetenv(model), GRB_INT_PAR_THREADS, nThreads);
//      int nn;    // THE SETTING FAILS TO WORK IN 6.0.5.
//      error = dll_getintparam(env, GRB_INT_PAR_THREADS, &nn);
//      cerr << "Set " << nThreads << " threads, reported " << nn << endl;
     wrap_assert(!error, "Failed to set GRB_INT_PAR_THREADS.", false);
   }

    if (nTimeout>0) {
     error = dll_GRBsetdblparam(dll_GRBgetenv(model), GRB_DBL_PAR_TIMELIMIT, nTimeout);
     wrap_assert(!error, "Failed to set GRB_PARAM_TimeLimit.", false);
    }

//     if (nWorkMemLimit>0) {
//      error =  dll_GRB_setdblparam (env, GRB_PARAM_MIP_Limits_TreeMemory, nWorkMemLimit);
//      wrap_assert(!error, "Failed to set GRB_PARAM_MIP_Limits_TreeMemory.", false);
//     }

   if ( absGap>=0.0 ) {
     error = dll_GRBsetdblparam( dll_GRBgetenv(model),  "MIPGapAbs", absGap );
     wrap_assert(!error, "Failed to set  MIPGapAbs.", false);
   }
   if ( relGap>=0.0 ) {
     error = dll_GRBsetdblparam( dll_GRBgetenv(model),  "MIPGap", relGap );
     wrap_assert(!error, "Failed to set  MIPGap.", false);
   }
   if ( intTol>=0.0 ) {
     error = dll_GRBsetdblparam( dll_GRBgetenv(model),  "IntFeasTol", intTol );
     wrap_assert(!error, "Failed to set   IntFeasTol.", false);
   }

    
       /// Solution callback
   output.nCols = colObj.size();
   x.resize(output.nCols);
   output.x = &x[0];
   if (true) {                 // Need for logging
      cbui.fVerb = fVerbose;
      if ( !flag_all_solutions )
        cbui.solcbfn = 0;
      if ( cbui.cutcbfn ) {
        assert( cbui.cutMask & (MaskConsType_Usercut|MaskConsType_Lazy) );
        if ( cbui.cutMask & MaskConsType_Usercut ) {
          // For user cuts, needs to keep some info after presolve
          if ( fVerbose )
            cerr << "  MIP_gurobi_wrapper: user cut callback enabled, setting PreCrush=1" << endl;
          error = dll_GRBsetintparam(dll_GRBgetenv(model), GRB_INT_PAR_PRECRUSH, 1);
          wrap_assert(!error, "Failed to set GRB_INT_PAR_PRECRUSH.", false);
        }
        if ( cbui.cutMask & MaskConsType_Lazy ) {
          // For lazy cuts, Gurobi disables some presolves
          if ( fVerbose )
            cerr << "  MIP_gurobi_wrapper: lazy cut callback enabled, setting LazyConstraints=1" << endl;
          error = dll_GRBsetintparam(dll_GRBgetenv(model), GRB_INT_PAR_LAZYCONSTRAINTS, 1);
          wrap_assert(!error, "Failed to set GRB_INT_PAR_LAZYCONSTRAINTS.", false);
        }
      }
      error = dll_GRBsetcallbackfunc(model, solcallback, (void *) &cbui);
      wrap_assert(!error, "Failed to set callback", false);
   }

   /// after all modifs
    if (sReadParams.size()) {
     error = dll_GRBreadparams (dll_GRBgetenv(model), sReadParams.c_str());
     wrap_assert(!error, "Failed to read GUROBI parameters.", false);
    }
    
    if (sWriteParams.size()) {
     error = dll_GRBwriteparams (dll_GRBgetenv(model), sWriteParams.c_str());
     wrap_assert(!error, "Failed to write GUROBI parameters.", false);
    }

   cbui.pOutput->cCPUTime0 = output.dCPUTime = std::clock();

   /* Optimize the problem and obtain solution. */
   error = dll_GRBoptimize(model);
   wrap_assert( !error,  "Failed to optimize MIP." );

   output.dCPUTime = (std::clock() - output.dCPUTime) / CLOCKS_PER_SEC;

   int solstat;
   error = dll_GRBgetintattr(model, GRB_INT_ATTR_STATUS, &solstat);
   wrap_assert(!error, "Failed to get MIP status.", false);
   output.status = convertStatus(solstat);

   /// Continuing to fill the output object:
   if (Status::OPT == output.status || Status::SAT ==output.status) {
      error = dll_GRBgetdblattr(model, GRB_DBL_ATTR_OBJVAL, &output.objVal);
      wrap_assert( !error, "No MIP objective value available." );

    //    int cur_numrows = dll_GRB_getnumrows (env, lp);
      int cur_numcols = getNCols();
      assert(cur_numcols == colObj.size());
      
      x.resize(cur_numcols);
      output.x = &x[0];
      error = dll_GRBgetdblattrarray(model, GRB_DBL_ATTR_X, 0, cur_numcols, (double*)output.x);
      wrap_assert(!error, "Failed to get variable values.");
   }
   output.bestBound = 1e308;
   error = dll_GRBgetdblattr(model, GRB_DBL_ATTR_OBJBOUNDC, &output.bestBound);
   wrap_assert(!error, "Failed to get the best bound.", false);
   double nNodes=-1;
   error = dll_GRBgetdblattr(model, GRB_DBL_ATTR_NODECOUNT, &nNodes);
   output.nNodes = nNodes;
   output.nOpenNodes = 0;
}

void MIP_gurobi_wrapper::setObjSense(int s)
{
  error = dll_GRBsetintattr(model, GRB_INT_ATTR_MODELSENSE,
                        s>0 ? GRB_MAXIMIZE : GRB_MINIMIZE);
  wrap_assert(!error, "Failed to set obj sense.");
}

