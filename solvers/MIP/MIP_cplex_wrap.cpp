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

#include <minizinc/config.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/utils.hh>

#include <cmath>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef CPLEX_PLUGIN
#ifdef HAS_DLFCN_H
#include <dlfcn.h>
#elif defined HAS_WINDOWS_H
#define NOMINMAX  // Ensure the words min/max remain available
#include <Windows.h>
#undef ERROR
#endif
#endif

#include <minizinc/solvers/MIP/MIP_cplex_wrap.hh>

using namespace std;

#ifdef CPLEX_PLUGIN

namespace {
void* dll_open(const std::string& file) {
#ifdef HAS_DLFCN_H
  if (MiniZinc::FileUtils::is_absolute(file)) {
    return dlopen(file.c_str(), RTLD_NOW);
  }
  if (void* so = dlopen(("lib" + file + ".so").c_str(), RTLD_NOW)) {
    return so;
  }
  return dlopen(("lib" + file + ".jnilib").c_str(), RTLD_NOW);
#else
  if (MiniZinc::FileUtils::is_absolute(file)) {
    return LoadLibrary(file.c_str());
  }
  return LoadLibrary((file + ".dll").c_str());
#endif
}
void* dll_sym(void* dll, const char* sym) {
#ifdef HAS_DLFCN_H
  void* ret = dlsym(dll, sym);
#else
  void* ret = GetProcAddress((HMODULE)dll, sym);
#endif
  if (ret == nullptr) {
    throw MiniZinc::InternalError("cannot load symbol " + string(sym) + " from CPLEX dll");
  }
  return ret;
}
void dll_close(void* dll) {
#ifdef HAS_DLFCN_H
  dlclose(dll);
#else
  FreeLibrary((HMODULE)dll);
#endif
}
}  // namespace

#endif

const vector<string>& cplex_dlls() {
  static const vector<string> sCPLEXDLLs = {"cplex12100", "cplex1290", "cplex1280", "cplex1270"};
  return sCPLEXDLLs;
}

void MIPCplexWrapper::checkDLL() {
#ifdef CPLEX_PLUGIN
  _cplexDll = nullptr;
  if (!_factoryOptions.cplexDll.empty()) {
    _cplexDll = dll_open(_factoryOptions.cplexDll);
  } else {
    for (const auto& s : cplex_dlls()) {
      _cplexDll = dll_open(s);
      if (nullptr != _cplexDll) {
        break;
      }
    }
  }

  if (_cplexDll == nullptr) {
    if (_factoryOptions.cplexDll.empty()) {
      throw MiniZinc::InternalError("cannot load cplex dll, specify --cplex-dll");
    }
    throw MiniZinc::InternalError("cannot load cplex dll `" + _factoryOptions.cplexDll + "'");
  }

  *(void**)(&dll_CPXaddfuncdest) = dll_sym(_cplexDll, "CPXaddfuncdest");
  *(void**)(&dll_CPXaddindconstr) = dll_sym(_cplexDll, "CPXaddindconstr");
  *(void**)(&dll_CPXaddlazyconstraints) = dll_sym(_cplexDll, "CPXaddlazyconstraints");
  *(void**)(&dll_CPXaddmipstarts) = dll_sym(_cplexDll, "CPXaddmipstarts");
  *(void**)(&dll_CPXaddrows) = dll_sym(_cplexDll, "CPXaddrows");
  *(void**)(&dll_CPXaddusercuts) = dll_sym(_cplexDll, "CPXaddusercuts");
  *(void**)(&dll_CPXchgbds) = dll_sym(_cplexDll, "CPXchgbds");
  *(void**)(&dll_CPXchgmipstarts) = dll_sym(_cplexDll, "CPXchgmipstarts");
  *(void**)(&dll_CPXchgobjsen) = dll_sym(_cplexDll, "CPXchgobjsen");
  *(void**)(&dll_CPXcloseCPLEX) = dll_sym(_cplexDll, "CPXcloseCPLEX");
  *(void**)(&dll_CPXcreateprob) = dll_sym(_cplexDll, "CPXcreateprob");
  *(void**)(&dll_CPXcutcallbackadd) = dll_sym(_cplexDll, "CPXcutcallbackadd");
  *(void**)(&dll_CPXfreeprob) = dll_sym(_cplexDll, "CPXfreeprob");
  *(void**)(&dll_CPXgetbestobjval) = dll_sym(_cplexDll, "CPXgetbestobjval");
  *(void**)(&dll_CPXgetcallbackincumbent) = dll_sym(_cplexDll, "CPXgetcallbackincumbent");
  *(void**)(&dll_CPXgetcallbackinfo) = dll_sym(_cplexDll, "CPXgetcallbackinfo");
  *(void**)(&dll_CPXgetcallbacknodeinfo) = dll_sym(_cplexDll, "CPXgetcallbacknodeinfo");
  *(void**)(&dll_CPXgetcallbacknodex) = dll_sym(_cplexDll, "CPXgetcallbacknodex");
  *(void**)(&dll_CPXgetchannels) = dll_sym(_cplexDll, "CPXgetchannels");
  *(void**)(&dll_CPXgetdettime) = dll_sym(_cplexDll, "CPXgetdettime");
  *(void**)(&dll_CPXgeterrorstring) = dll_sym(_cplexDll, "CPXgeterrorstring");
  *(void**)(&dll_CPXgetmipstartindex) = dll_sym(_cplexDll, "CPXgetmipstartindex");
  *(void**)(&dll_CPXgetnodecnt) = dll_sym(_cplexDll, "CPXgetnodecnt");
  *(void**)(&dll_CPXgetnodeleftcnt) = dll_sym(_cplexDll, "CPXgetnodeleftcnt");
  *(void**)(&dll_CPXgetnumcols) = dll_sym(_cplexDll, "CPXgetnumcols");
  *(void**)(&dll_CPXgetnumrows) = dll_sym(_cplexDll, "CPXgetnumrows");
  *(void**)(&dll_CPXgetobjsen) = dll_sym(_cplexDll, "CPXgetobjsen");
  *(void**)(&dll_CPXgetobjval) = dll_sym(_cplexDll, "CPXgetobjval");
  *(void**)(&dll_CPXgetsolnpoolnumsolns) = dll_sym(_cplexDll, "CPXgetsolnpoolnumsolns");
  *(void**)(&dll_CPXgetstat) = dll_sym(_cplexDll, "CPXgetstat");
  *(void**)(&dll_CPXgetstatstring) = dll_sym(_cplexDll, "CPXgetstatstring");
  *(void**)(&dll_CPXgettime) = dll_sym(_cplexDll, "CPXgettime");
  *(void**)(&dll_CPXgetx) = dll_sym(_cplexDll, "CPXgetx");
  *(void**)(&dll_CPXmipopt) = dll_sym(_cplexDll, "CPXmipopt");
  *(void**)(&dll_CPXnewcols) = dll_sym(_cplexDll, "CPXnewcols");
  *(void**)(&dll_CPXopenCPLEX) = dll_sym(_cplexDll, "CPXopenCPLEX");
  *(void**)(&dll_CPXreadcopyparam) = dll_sym(_cplexDll, "CPXreadcopyparam");
  *(void**)(&dll_CPXsetdblparam) = dll_sym(_cplexDll, "CPXsetdblparam");
  *(void**)(&dll_CPXsetinfocallbackfunc) = dll_sym(_cplexDll, "CPXsetinfocallbackfunc");
  *(void**)(&dll_CPXsetintparam) = dll_sym(_cplexDll, "CPXsetintparam");
  *(void**)(&dll_CPXsetstrparam) = dll_sym(_cplexDll, "CPXsetstrparam");
  *(void**)(&dll_CPXsetlazyconstraintcallbackfunc) =
      dll_sym(_cplexDll, "CPXsetlazyconstraintcallbackfunc");
  *(void**)(&dll_CPXsetusercutcallbackfunc) = dll_sym(_cplexDll, "CPXsetusercutcallbackfunc");
  *(void**)(&dll_CPXversion) = dll_sym(_cplexDll, "CPXversion");
  *(void**)(&dll_CPXwriteparam) = dll_sym(_cplexDll, "CPXwriteparam");
  *(void**)(&dll_CPXwriteprob) = dll_sym(_cplexDll, "CPXwriteprob");
  *(void**)(&dll_CPXgetparamname) = dll_sym(_cplexDll, "CPXgetparamname");
  *(void**)(&dll_CPXgetparamnum) = dll_sym(_cplexDll, "CPXgetparamnum");
  *(void**)(&dll_CPXgetparamtype) = dll_sym(_cplexDll, "CPXgetparamtype");
  *(void**)(&dll_CPXinfointparam) = dll_sym(_cplexDll, "CPXinfointparam");
  *(void**)(&dll_CPXinfolongparam) = dll_sym(_cplexDll, "CPXinfolongparam");
  *(void**)(&dll_CPXinfodblparam) = dll_sym(_cplexDll, "CPXinfodblparam");
  *(void**)(&dll_CPXinfostrparam) = dll_sym(_cplexDll, "CPXinfostrparam");
  *(void**)(&dll_CPXsetlongparam) = dll_sym(_cplexDll, "CPXsetlongparam");

#else

  dll_CPXaddfuncdest = CPXaddfuncdest;
  dll_CPXaddindconstr = CPXaddindconstr;
  dll_CPXaddlazyconstraints = CPXaddlazyconstraints;
  dll_CPXaddmipstarts = CPXaddmipstarts;
  dll_CPXaddrows = CPXaddrows;
  dll_CPXaddusercuts = CPXaddusercuts;
  dll_CPXchgbds = CPXchgbds;
  dll_CPXchgmipstarts = CPXchgmipstarts;
  dll_CPXchgobjsen = CPXchgobjsen;
  dll_CPXcloseCPLEX = CPXcloseCPLEX;
  dll_CPXcreateprob = CPXcreateprob;
  dll_CPXcutcallbackadd = CPXcutcallbackadd;
  dll_CPXfreeprob = CPXfreeprob;
  dll_CPXgetbestobjval = CPXgetbestobjval;
  dll_CPXgetcallbackincumbent = CPXgetcallbackincumbent;
  dll_CPXgetcallbackinfo = CPXgetcallbackinfo;
  dll_CPXgetcallbacknodeinfo = CPXgetcallbacknodeinfo;
  dll_CPXgetcallbacknodex = CPXgetcallbacknodex;
  dll_CPXgetchannels = CPXgetchannels;
  dll_CPXgetdettime = CPXgetdettime;
  dll_CPXgeterrorstring = CPXgeterrorstring;
  dll_CPXgetmipstartindex = CPXgetmipstartindex;
  dll_CPXgetnodecnt = CPXgetnodecnt;
  dll_CPXgetnodeleftcnt = CPXgetnodeleftcnt;
  dll_CPXgetnumcols = CPXgetnumcols;
  dll_CPXgetnumrows = CPXgetnumrows;
  dll_CPXgetobjsen = CPXgetobjsen;
  dll_CPXgetobjval = CPXgetobjval;
  dll_CPXgetsolnpoolnumsolns = CPXgetsolnpoolnumsolns;
  dll_CPXgetstat = CPXgetstat;
  dll_CPXgetstatstring = CPXgetstatstring;
  dll_CPXgettime = CPXgettime;
  dll_CPXgetx = CPXgetx;
  dll_CPXmipopt = CPXmipopt;
  dll_CPXnewcols = CPXnewcols;
  dll_CPXopenCPLEX = CPXopenCPLEX;
  dll_CPXreadcopyparam = CPXreadcopyparam;
  dll_CPXsetdblparam = CPXsetdblparam;
  dll_CPXsetinfocallbackfunc = CPXsetinfocallbackfunc;
  dll_CPXsetintparam = CPXsetintparam;
  dll_CPXsetstrparam = CPXsetstrparam;
  dll_CPXsetlazyconstraintcallbackfunc = CPXsetlazyconstraintcallbackfunc;
  dll_CPXsetusercutcallbackfunc = CPXsetusercutcallbackfunc;
  dll_CPXversion = CPXversion;
  dll_CPXwriteparam = CPXwriteparam;
  dll_CPXwriteprob = CPXwriteprob;
  dll_CPXgetparamname = CPXgetparamname;
  dll_CPXgetparamnum = CPXgetparamnum;
  dll_CPXgetparamtype = CPXgetparamtype;
  dll_CPXinfointparam = CPXinfointparam;
  dll_CPXinfolongparam = CPXinfolongparam;
  dll_CPXinfodblparam = CPXinfodblparam;
  dll_CPXinfostrparam = CPXinfostrparam;
  dll_CPXsetlongparam = CPXsetlongparam;

#endif
}

string MIPCplexWrapper::getDescription(FactoryOptions& factoryOpt,
                                       MiniZinc::SolverInstanceBase::Options* opt) {
  string v = "MIP wrapper for IBM ILOG CPLEX  ";
  int status;
  Options def_options;
  Options* options = opt != nullptr ? static_cast<Options*>(opt) : &def_options;
  try {
    MIPCplexWrapper mcw(factoryOpt, options);
    CPXENVptr env = mcw.dll_CPXopenCPLEX(&status);
    if (env != nullptr) {
      v += mcw.dll_CPXversion(env);
      status = mcw.dll_CPXcloseCPLEX(&env);
    } else {
      v += "[?? ...cannot open CPLEX env to query version]";
    }
  } catch (MiniZinc::InternalError&) {
    v += "[?? ...cannot open CPLEX env to query version]";
  }
  v += "  Compiled  " __DATE__ "  " __TIME__;
  return v;
}

string MIPCplexWrapper::getVersion(FactoryOptions& factoryOpt,
                                   MiniZinc::SolverInstanceBase::Options* opt) {
  string v;
  int status;
  Options def_options;
  Options* options = opt != nullptr ? static_cast<Options*>(opt) : &def_options;
  try {
    MIPCplexWrapper mcw(factoryOpt, options);
    CPXENVptr env = mcw.dll_CPXopenCPLEX(&status);
    if (env != nullptr) {
      v += mcw.dll_CPXversion(env);
      status = mcw.dll_CPXcloseCPLEX(&env);
    } else {
      v += "<unknown version>";
    }
  } catch (MiniZinc::InternalError&) {
    v += "<unknown version>";
  }
  return v;
}

vector<string> MIPCplexWrapper::getRequiredFlags(FactoryOptions& f) {
  int status;
  FactoryOptions factoryOptions;
  Options options;
  try {
    MIPCplexWrapper mcw(factoryOptions, &options);
    CPXENVptr env = mcw.dll_CPXopenCPLEX(&status);
    if (env != nullptr) {
      return {};
    }
  } catch (MiniZinc::InternalError&) {
  }
  return {"--cplex-dll"};
}

vector<string> MIPCplexWrapper::getFactoryFlags() { return {"--cplex-dll"}; }

string MIPCplexWrapper::getId() { return "cplex"; }

string MIPCplexWrapper::getName() { return "CPLEX"; }

vector<string> MIPCplexWrapper::getTags() { return {"mip", "float", "api"}; }

vector<string> MIPCplexWrapper::getStdFlags() { return {"-i", "-p", "-s", "-v"}; }

vector<MiniZinc::SolverConfig::ExtraFlag> MIPCplexWrapper::getExtraFlags(
    FactoryOptions& factoryOpt) {
  // CPLEX doesn't seem to have a way to enumerate all parameters
  static std::vector<std::string> all_params = {
      "CPXPARAM_Advance", "CPXPARAM_Barrier_Algorithm", "CPXPARAM_Barrier_ColNonzeros",
      "CPXPARAM_Barrier_ConvergeTol", "CPXPARAM_Barrier_Crossover", "CPXPARAM_Barrier_Display",
      "CPXPARAM_Barrier_Limits_Corrections", "CPXPARAM_Barrier_Limits_Growth",
      "CPXPARAM_Barrier_Limits_Iteration", "CPXPARAM_Barrier_Limits_ObjRange",
      "CPXPARAM_Barrier_Ordering", "CPXPARAM_Barrier_QCPConvergeTol", "CPXPARAM_Barrier_StartAlg",
      "CPXPARAM_Benders_Strategy", "CPXPARAM_Benders_Tolerances_feasibilitycut",
      "CPXPARAM_Benders_Tolerances_optimalitycut", "CPXPARAM_Benders_WorkerAlgorithm",
      "CPXPARAM_ClockType", "CPXPARAM_Conflict_Algorithm", "CPXPARAM_Conflict_Display",
      "CPXPARAM_CPUmask", "CPXPARAM_DetTimeLimit", "CPXPARAM_DistMIP_Rampup_DetTimeLimit",
      "CPXPARAM_DistMIP_Rampup_Duration", "CPXPARAM_DistMIP_Rampup_TimeLimit",
      "CPXPARAM_Emphasis_Memory",
      // "CPXPARAM_Emphasis_MIP",
      "CPXPARAM_Emphasis_Numerical", "CPXPARAM_Feasopt_Mode", "CPXPARAM_Feasopt_Tolerance",
      "CPXPARAM_LPMethod", "CPXPARAM_MIP_Cuts_BQP", "CPXPARAM_MIP_Cuts_Cliques",
      "CPXPARAM_MIP_Cuts_Covers", "CPXPARAM_MIP_Cuts_Disjunctive", "CPXPARAM_MIP_Cuts_FlowCovers",
      "CPXPARAM_MIP_Cuts_Gomory", "CPXPARAM_MIP_Cuts_GUBCovers", "CPXPARAM_MIP_Cuts_Implied",
      "CPXPARAM_MIP_Cuts_LiftProj", "CPXPARAM_MIP_Cuts_LocalImplied", "CPXPARAM_MIP_Cuts_MCFCut",
      "CPXPARAM_MIP_Cuts_MIRCut", "CPXPARAM_MIP_Cuts_PathCut", "CPXPARAM_MIP_Cuts_RLT",
      "CPXPARAM_MIP_Cuts_ZeroHalfCut",
      // "CPXPARAM_MIP_Display",
      "CPXPARAM_MIP_Interval", "CPXPARAM_MIP_Limits_AggForCut",
      "CPXPARAM_MIP_Limits_AuxRootThreads", "CPXPARAM_MIP_Limits_CutPasses",
      "CPXPARAM_MIP_Limits_CutsFactor", "CPXPARAM_MIP_Limits_EachCutLimit",
      "CPXPARAM_MIP_Limits_GomoryCand", "CPXPARAM_MIP_Limits_GomoryPass",
      "CPXPARAM_MIP_Limits_Nodes", "CPXPARAM_MIP_Limits_PolishTime", "CPXPARAM_MIP_Limits_Populate",
      "CPXPARAM_MIP_Limits_ProbeDetTime", "CPXPARAM_MIP_Limits_ProbeTime",
      "CPXPARAM_MIP_Limits_RepairTries",
      // "CPXPARAM_MIP_Limits_Solutions",
      "CPXPARAM_MIP_Limits_StrongCand", "CPXPARAM_MIP_Limits_StrongIt",
      "CPXPARAM_MIP_Limits_TreeMemory", "CPXPARAM_MIP_OrderType",
      "CPXPARAM_MIP_PolishAfter_AbsMIPGap", "CPXPARAM_MIP_PolishAfter_DetTime",
      "CPXPARAM_MIP_PolishAfter_MIPGap", "CPXPARAM_MIP_PolishAfter_Nodes",
      "CPXPARAM_MIP_PolishAfter_Solutions", "CPXPARAM_MIP_PolishAfter_Time",
      "CPXPARAM_MIP_Pool_AbsGap", "CPXPARAM_MIP_Pool_Capacity", "CPXPARAM_MIP_Pool_Intensity",
      "CPXPARAM_MIP_Pool_RelGap", "CPXPARAM_MIP_Pool_Replace", "CPXPARAM_MIP_Strategy_Backtrack",
      "CPXPARAM_MIP_Strategy_BBInterval", "CPXPARAM_MIP_Strategy_Branch",
      // "CPXPARAM_MIP_Strategy_CallbackReducedLP",
      "CPXPARAM_MIP_Strategy_Dive",
      // "CPXPARAM_MIP_Strategy_File",
      "CPXPARAM_MIP_Strategy_FPHeur", "CPXPARAM_MIP_Strategy_HeuristicEffort",
      "CPXPARAM_MIP_Strategy_HeuristicFreq", "CPXPARAM_MIP_Strategy_KappaStats",
      "CPXPARAM_MIP_Strategy_LBHeur", "CPXPARAM_MIP_Strategy_MIQCPStrat",
      "CPXPARAM_MIP_Strategy_NodeSelect", "CPXPARAM_MIP_Strategy_Order",
      "CPXPARAM_MIP_Strategy_PresolveNode", "CPXPARAM_MIP_Strategy_Probe",
      "CPXPARAM_MIP_Strategy_RINSHeur",
      // "CPXPARAM_MIP_Strategy_Search",
      "CPXPARAM_MIP_Strategy_StartAlgorithm", "CPXPARAM_MIP_Strategy_SubAlgorithm",
      "CPXPARAM_MIP_Strategy_VariableSelect", "CPXPARAM_MIP_SubMIP_StartAlg",
      "CPXPARAM_MIP_SubMIP_SubAlg", "CPXPARAM_MIP_SubMIP_NodeLimit", "CPXPARAM_MIP_SubMIP_Scale",
      // "CPXPARAM_MIP_Tolerances_AbsMIPGap",
      "CPXPARAM_MIP_Tolerances_Linearization",
      // "CPXPARAM_MIP_Tolerances_Integrality",
      "CPXPARAM_MIP_Tolerances_LowerCutoff",
      // "CPXPARAM_MIP_Tolerances_MIPGap",
      "CPXPARAM_MIP_Tolerances_ObjDifference", "CPXPARAM_MIP_Tolerances_RelObjDifference",
      "CPXPARAM_MIP_Tolerances_UpperCutoff", "CPXPARAM_MultiObjective_Display",
      "CPXPARAM_Network_Display", "CPXPARAM_Network_Iterations", "CPXPARAM_Network_NetFind",
      "CPXPARAM_Network_Pricing", "CPXPARAM_Network_Tolerances_Feasibility",
      "CPXPARAM_Network_Tolerances_Optimality", "CPXPARAM_OptimalityTarget",
      "CPXPARAM_Output_CloneLog", "CPXPARAM_Output_IntSolFilePrefix", "CPXPARAM_Output_MPSLong",
      "CPXPARAM_Output_WriteLevel", "CPXPARAM_Parallel", "CPXPARAM_ParamDisplay",
      "CPXPARAM_Preprocessing_Aggregator", "CPXPARAM_Preprocessing_BoundStrength",
      "CPXPARAM_Preprocessing_CoeffReduce", "CPXPARAM_Preprocessing_Dependency",
      "CPXPARAM_Preprocessing_Dual", "CPXPARAM_Preprocessing_Fill",
      "CPXPARAM_Preprocessing_Folding",
      // "CPXPARAM_Preprocessing_Linear",
      "CPXPARAM_Preprocessing_NumPass", "CPXPARAM_Preprocessing_Presolve",
      "CPXPARAM_Preprocessing_QCPDuals", "CPXPARAM_Preprocessing_QPMakePSD",
      "CPXPARAM_Preprocessing_QToLin", "CPXPARAM_Preprocessing_Reduce",
      "CPXPARAM_Preprocessing_Relax", "CPXPARAM_Preprocessing_RepeatPresolve",
      "CPXPARAM_Preprocessing_Symmetry", "CPXPARAM_QPMethod",
      // "CPXPARAM_RandomSeed",
      "CPXPARAM_Read_APIEncoding", "CPXPARAM_Read_Constraints", "CPXPARAM_Read_DataCheck",
      "CPXPARAM_Read_FileEncoding", "CPXPARAM_Read_Nonzeros", "CPXPARAM_Read_QPNonzeros",
      "CPXPARAM_Read_Scale", "CPXPARAM_Read_Variables", "CPXPARAM_Read_WarningLimit",
      "CPXPARAM_Record", "CPXPARAM_ScreenOutput", "CPXPARAM_Sifting_Algorithm",
      "CPXPARAM_Sifting_Simplex", "CPXPARAM_Sifting_Display", "CPXPARAM_Sifting_Iterations",
      "CPXPARAM_Simplex_Crash", "CPXPARAM_Simplex_DGradient", "CPXPARAM_Simplex_Display",
      "CPXPARAM_Simplex_DynamicRows", "CPXPARAM_Simplex_Limits_Iterations",
      "CPXPARAM_Simplex_Limits_LowerObj", "CPXPARAM_Simplex_Limits_Perturbation",
      "CPXPARAM_Simplex_Limits_Singularity", "CPXPARAM_Simplex_Limits_UpperObj",
      "CPXPARAM_Simplex_Perturbation_Constant", "CPXPARAM_Simplex_Perturbation_Indicator",
      "CPXPARAM_Simplex_PGradient", "CPXPARAM_Simplex_Pricing", "CPXPARAM_Simplex_Refactor",
      "CPXPARAM_Simplex_Tolerances_Feasibility", "CPXPARAM_Simplex_Tolerances_Markowitz",
      "CPXPARAM_Simplex_Tolerances_Optimality", "CPXPARAM_SolutionType",
      // "CPXPARAM_Threads",
      // "CPXPARAM_TimeLimit",
      "CPXPARAM_Tune_DetTimeLimit", "CPXPARAM_Tune_Display", "CPXPARAM_Tune_Measure",
      "CPXPARAM_Tune_Repeat", "CPXPARAM_Tune_TimeLimit",
      // "CPXPARAM_WorkDir",
      // "CPXPARAM_WorkMem",
      "CPX_PARAM_ADVIND", "CPX_PARAM_AGGFILL", "CPX_PARAM_AGGIND", "CPX_PARAM_CLOCKTYPE",
      "CPX_PARAM_CRAIND", "CPX_PARAM_DEPIND", "CPX_PARAM_DPRIIND", "CPX_PARAM_PRICELIM",
      "CPX_PARAM_EPMRK", "CPX_PARAM_EPOPT", "CPX_PARAM_EPPER", "CPX_PARAM_EPRHS",
      "CPX_PARAM_SIMDISPLAY", "CPX_PARAM_ITLIM", "CPX_PARAM_ROWREADLIM", "CPX_PARAM_NETFIND",
      "CPX_PARAM_COLREADLIM", "CPX_PARAM_NZREADLIM", "CPX_PARAM_OBJLLIM", "CPX_PARAM_OBJULIM",
      "CPX_PARAM_PERIND", "CPX_PARAM_PERLIM", "CPX_PARAM_PPRIIND", "CPX_PARAM_PREIND",
      "CPX_PARAM_REINV", "CPX_PARAM_SCAIND", "CPX_PARAM_SCRIND", "CPX_PARAM_SINGLIM",
      "CPX_PARAM_TILIM", "CPX_PARAM_PREDUAL", "CPX_PARAM_PREPASS", "CPX_PARAM_DATACHECK",
      "CPX_PARAM_REDUCE", "CPX_PARAM_PRELINEAR", "CPX_PARAM_LPMETHOD", "CPX_PARAM_QPMETHOD",
      "CPX_PARAM_WORKDIR", "CPX_PARAM_WORKMEM", "CPX_PARAM_THREADS", "CPX_PARAM_CONFLICTALG",
      "CPX_PARAM_CONFLICTDISPLAY", "CPX_PARAM_SIFTDISPLAY", "CPX_PARAM_SIFTALG",
      "CPX_PARAM_SIFTITLIM", "CPX_PARAM_MPSLONGNUM", "CPX_PARAM_MEMORYEMPHASIS",
      "CPX_PARAM_NUMERICALEMPHASIS", "CPX_PARAM_FEASOPTMODE", "CPX_PARAM_PARALLELMODE",
      "CPX_PARAM_TUNINGMEASURE", "CPX_PARAM_TUNINGREPEAT", "CPX_PARAM_TUNINGTILIM",
      "CPX_PARAM_TUNINGDISPLAY", "CPX_PARAM_WRITELEVEL", "CPX_PARAM_RANDOMSEED",
      "CPX_PARAM_DETTILIM", "CPX_PARAM_FILEENCODING", "CPX_PARAM_APIENCODING",
      "CPX_PARAM_OPTIMALITYTARGET", "CPX_PARAM_CLONELOG", "CPX_PARAM_TUNINGDETTILIM",
      "CPX_PARAM_CPUMASK", "CPX_PARAM_SOLUTIONTYPE", "CPX_PARAM_WARNLIM", "CPX_PARAM_SIFTSIM",
      "CPX_PARAM_DYNAMICROWS", "CPX_PARAM_RECORD", "CPX_PARAM_PARAMDISPLAY", "CPX_PARAM_FOLDING",
      "CPX_PARAM_WORKERALG", "CPX_PARAM_BENDERSSTRATEGY", "CPX_PARAM_BENDERSFEASCUTTOL",
      "CPX_PARAM_BENDERSOPTCUTTOL", "CPX_PARAM_MULTIOBJDISPLAY", "CPX_PARAM_BRDIR",
      "CPX_PARAM_BTTOL", "CPX_PARAM_CLIQUES", "CPX_PARAM_COEREDIND", "CPX_PARAM_COVERS",
      "CPX_PARAM_CUTLO", "CPX_PARAM_CUTUP", "CPX_PARAM_EPAGAP", "CPX_PARAM_EPGAP",
      "CPX_PARAM_EPINT", "CPX_PARAM_MIPDISPLAY", "CPX_PARAM_MIPINTERVAL", "CPX_PARAM_INTSOLLIM",
      "CPX_PARAM_NODEFILEIND", "CPX_PARAM_NODELIM", "CPX_PARAM_NODESEL", "CPX_PARAM_OBJDIF",
      "CPX_PARAM_MIPORDIND", "CPX_PARAM_RELOBJDIF", "CPX_PARAM_STARTALG", "CPX_PARAM_SUBALG",
      "CPX_PARAM_TRELIM", "CPX_PARAM_VARSEL", "CPX_PARAM_BNDSTRENIND", "CPX_PARAM_HEURFREQ",
      "CPX_PARAM_MIPORDTYPE", "CPX_PARAM_CUTSFACTOR", "CPX_PARAM_RELAXPREIND", "CPX_PARAM_PRESLVND",
      "CPX_PARAM_BBINTERVAL", "CPX_PARAM_FLOWCOVERS", "CPX_PARAM_IMPLBD", "CPX_PARAM_PROBE",
      "CPX_PARAM_GUBCOVERS", "CPX_PARAM_STRONGCANDLIM", "CPX_PARAM_STRONGITLIM",
      "CPX_PARAM_FRACCAND", "CPX_PARAM_FRACCUTS", "CPX_PARAM_FRACPASS", "CPX_PARAM_FLOWPATHS",
      "CPX_PARAM_MIRCUTS", "CPX_PARAM_DISJCUTS", "CPX_PARAM_AGGCUTLIM",
      // "CPX_PARAM_MIPCBREDLP",
      "CPX_PARAM_CUTPASS", "CPX_PARAM_MIPEMPHASIS", "CPX_PARAM_SYMMETRY", "CPX_PARAM_DIVETYPE",
      "CPX_PARAM_RINSHEUR", "CPX_PARAM_LBHEUR", "CPX_PARAM_REPEATPRESOLVE", "CPX_PARAM_PROBETIME",
      "CPX_PARAM_POLISHTIME", "CPX_PARAM_REPAIRTRIES", "CPX_PARAM_EPLIN", "CPX_PARAM_EPRELAX",
      "CPX_PARAM_FPHEUR", "CPX_PARAM_EACHCUTLIM", "CPX_PARAM_SOLNPOOLCAPACITY",
      "CPX_PARAM_SOLNPOOLREPLACE", "CPX_PARAM_SOLNPOOLGAP", "CPX_PARAM_SOLNPOOLAGAP",
      "CPX_PARAM_SOLNPOOLINTENSITY", "CPX_PARAM_POPULATELIM", "CPX_PARAM_MIPSEARCH",
      "CPX_PARAM_MIQCPSTRAT", "CPX_PARAM_ZEROHALFCUTS", "CPX_PARAM_HEUREFFORT",
      "CPX_PARAM_POLISHAFTEREPAGAP", "CPX_PARAM_POLISHAFTEREPGAP", "CPX_PARAM_POLISHAFTERNODE",
      "CPX_PARAM_POLISHAFTERINTSOL", "CPX_PARAM_POLISHAFTERTIME", "CPX_PARAM_MCFCUTS",
      "CPX_PARAM_MIPKAPPASTATS", "CPX_PARAM_AUXROOTTHREADS", "CPX_PARAM_INTSOLFILEPREFIX",
      "CPX_PARAM_PROBEDETTIME", "CPX_PARAM_POLISHAFTERDETTIME", "CPX_PARAM_LANDPCUTS",
      "CPX_PARAM_RAMPUPDURATION", "CPX_PARAM_RAMPUPDETTILIM", "CPX_PARAM_RAMPUPTILIM",
      "CPX_PARAM_LOCALIMPLBD", "CPX_PARAM_BQPCUTS", "CPX_PARAM_RLTCUTS", "CPX_PARAM_SUBMIPSTARTALG",
      "CPX_PARAM_SUBMIPSUBALG", "CPX_PARAM_SUBMIPSCAIND", "CPX_PARAM_SUBMIPNODELIMIT",
      "CPX_PARAM_BAREPCOMP", "CPX_PARAM_BARGROWTH", "CPX_PARAM_BAROBJRNG", "CPX_PARAM_BARALG",
      "CPX_PARAM_BARCOLNZ", "CPX_PARAM_BARDISPLAY", "CPX_PARAM_BARITLIM", "CPX_PARAM_BARMAXCOR",
      "CPX_PARAM_BARORDER", "CPX_PARAM_BARSTARTALG", "CPX_PARAM_BARCROSSALG",
      "CPX_PARAM_BARQCPEPCOMP", "CPX_PARAM_QPNZREADLIM", "CPX_PARAM_CALCQCPDUALS",
      "CPX_PARAM_QPMAKEPSDIND", "CPX_PARAM_QTOLININD", "CPX_PARAM_NETITLIM", "CPX_PARAM_NETEPOPT",
      "CPX_PARAM_NETEPRHS", "CPX_PARAM_NETPPRIIND", "CPX_PARAM_NETDISPLAY"};
  int status;
  Options def_options;
  try {
    MIPCplexWrapper mcw(factoryOpt, &def_options);
    CPXENVptr env = mcw.dll_CPXopenCPLEX(&status);
    if (env == nullptr) {
      return {};
    }
    std::vector<MiniZinc::SolverConfig::ExtraFlag> res;
    for (auto& param : all_params) {
      int num;
      int type;
      if (mcw.dll_CPXgetparamnum(env, param.c_str(), &num) != 0) {
        // Param not available
        continue;
      }
      mcw.dll_CPXgetparamtype(env, num, &type);
      MiniZinc::SolverConfig::ExtraFlag::FlagType param_type;
      std::vector<std::string> param_range;
      std::string param_default;
      switch (type) {
        case CPX_PARAMTYPE_INT: {
          CPXINT def;
          CPXINT min;
          CPXINT max;
          mcw.dll_CPXinfointparam(env, num, &def, &min, &max);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
          param_range.push_back(std::to_string(min));
          param_range.push_back(std::to_string(max));
          param_default = std::to_string(def);
          break;
        }
        case CPX_PARAMTYPE_LONG: {
          CPXLONG def;
          CPXLONG min;
          CPXLONG max;
          mcw.dll_CPXinfolongparam(env, num, &def, &min, &max);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
          param_range.push_back(std::to_string(min));
          param_range.push_back(std::to_string(max));
          param_default = std::to_string(def);
          break;
        }
        case CPX_PARAMTYPE_DOUBLE: {
          double def;
          double min;
          double max;
          mcw.dll_CPXinfodblparam(env, num, &def, &min, &max);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_FLOAT;
          param_range.push_back(std::to_string(min));
          param_range.push_back(std::to_string(max));
          param_default = std::to_string(def);
          break;
        }
        case CPX_PARAMTYPE_STRING: {
          char def[CPX_STR_PARAM_MAX];
          mcw.dll_CPXinfostrparam(env, num, def);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_STRING;
          param_default = std::string(def);
          break;
        }
        default:
          continue;
      }
      res.emplace_back("--cplex-" + param, param, param_type, param_range, param_default);
    }
    return res;
  } catch (MiniZinc::InternalError&) {
    return {};
  }
  return {};
}

void MIPCplexWrapper::Options::printHelp(ostream& os) {
  os << "IBM ILOG CPLEX  MIP wrapper options:"
     << std::endl
     // -s                  print statistics
     //            << "  --readParam <file>  read CPLEX parameters from file
     //               << "--writeParam <file> write CPLEX parameters to file
     //               << "--tuneParam         instruct CPLEX to tune parameters instead of solving
     << "  --mipfocus <n>\n    1: feasibility, 2: optimality, 3: move bound (default is 0, "
        "balanced)"
     << std::endl
     << "  -i\n    print intermediate solutions for optimization problems" << std::endl
     << "  -p <N>, --parallel <N>\n    use N threads, default: 1"
     << std::endl
     //   << "  --nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
     << "  --solver-time-limit <N>\n    stop search after N milliseconds wall time" << std::endl
     << "  -n <N>, --num-solutions <N>\n"
        "    stop search after N solutions"
     << std::endl
     << "  -r <N>, --random-seed <N>\n"
        "    random seed, integer"
     << std::endl
     << "  --workmem <N>, --nodefilestart <N>\n"
        "    maximal RAM for working memory used before writing to node file, GB, default: 0.5"
     << std::endl
     << "  --nodefiledir <path>\n"
        "    nodefile directory"
     << std::endl
     << "  --writeModel <file>\n    write model to <file> (.lp, .mps, .sav, ...)" << std::endl
     << "  --readParam <file>\n    read CPLEX parameters from file" << std::endl
     << "  --writeParam <file>\n    write CPLEX parameters to file"
     << std::endl
     //   << "  --tuneParam         instruct CPLEX to tune parameters instead of solving   NOT IMPL"

     << "  --absGap <n>\n    absolute gap |primal-dual| to stop" << std::endl
     << "  --relGap <n>\n    relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 "
        "to use backend's default"
     << std::endl
     << "  --intTol <n>\n    integrality tolerance for a variable. Default 1e-8" << std::endl
     << "\n  --cplex-dll <file> or <basename>\n    CPLEX DLL, or base name, such as cplex1280, "
        "when using plugin. Default range tried: "
     << cplex_dlls().front() << " .. " << cplex_dlls().back()
     << std::endl
     //   << "  --objDiff <n>       objective function discretization. Default 1.0" << std::endl

     << std::endl;
}

bool MIPCplexWrapper::FactoryOptions::processOption(int& i, std::vector<std::string>& argv,
                                                    const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  return cop.get("--cplex-dll", &cplexDll);
}

bool MIPCplexWrapper::Options::processOption(int& i, std::vector<std::string>& argv,
                                             const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  std::string buffer;
  if (cop.get("-i")) {
    flagIntermediate = true;
  } else if (string(argv[i]) == "-f") {  // NOLINT: Allow repeated empty if
    //     std::cerr << "  Flag -f: ignoring fixed strategy anyway." << std::endl;
  } else if (cop.get("--mipfocus --mipFocus --MIPFocus --MIPfocus",
                     &nMIPFocus)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--writeModel", &buffer)) {
    sExportModel = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("-p  --parallel", &nThreads)) {       // NOLINT: Allow repeated empty if
  } else if (cop.get("--solver-time-limit", &nTimeout)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("-n --num-solutions", &nSolLimit)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("-r --random-seed", &nSeed)) {        // NOLINT: Allow repeated empty if
  } else if (cop.get("--workmem --nodefilestart",
                     &nWorkMemLimit)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--nodefiledir --NodefileDir",
                     &sNodefileDir)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--readParam", &buffer)) {
    sReadParams = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("--writeParam", &buffer)) {
    sWriteParams = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("--absGap", &absGap)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--relGap", &relGap)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--intTol", &intTol)) {  // NOLINT: Allow repeated empty if
    //   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else {
    return false;
  }
  return true;
}

void MIPCplexWrapper::wrapAssert(bool cond, const string& msg, bool fTerm) {
  if (!cond) {
    strcpy(_cplexBuffer, "[NO ERROR STRING GIVEN]");
    dll_CPXgeterrorstring(_env, _status, _cplexBuffer);
    string msgAll = ("  MIPCplexWrapper runtime error:  " + msg + "  " + _cplexBuffer);
    cerr << msgAll << endl;
    if (fTerm) {
      cerr << "TERMINATING." << endl;
      throw runtime_error(msgAll);
    }
  }
}

void MIPCplexWrapper::openCPLEX() {
  checkDLL();
  cbui.wrapper = this;
  /// Cleanup first.
  //   cleanup();
  /* Initialize the CPLEX environment */
  _env = dll_CPXopenCPLEX(&_status);
  /* If an error occurs, the status value indicates the reason for
     failure.  A call to CPXgeterrorstring will produce the text of
     the error message.  Note that CPXopenCPLEX produces no output,
     so the only way to see the cause of the error is to use
     CPXgeterrorstring.  For other CPLEX routines, the errors will
     be seen if the CPXPARAM_ScreenOutput indicator is set to CPX_ON.  */
  wrapAssert(_env != nullptr, "Could not open CPLEX environment.");
  /* Create the problem. */
  _lp = dll_CPXcreateprob(_env, &_status, "MIPCplexWrapper");
  /* A returned pointer of NULL may mean that not enough memory
     was available or there was some other problem.  In the case of
     failure, an error message will have been written to the error
     channel from inside CPLEX.  In this example, the setting of
     the parameter CPXPARAM_ScreenOutput causes the error message to
     appear on stdout.  */
  wrapAssert(_lp != nullptr, "Failed to create LP.");
}

void MIPCplexWrapper::closeCPLEX() {
  /// Freeing the problem can be slow both in C and C++, see IBM forums. Skipping.
  /* Free up the problem as allocated by CPXcreateprob, if necessary */
  //    if ( lp != NULL ) {
  //       status = CPXfreeprob (env, &lp);
  //       cplex_wrapAssert ( !status, "CPXfreeprob failed." );
  //    }
  _lp = nullptr;
  /* Free up the CPLEX environment, if necessary */
  if (_env != nullptr) {
    _status = dll_CPXcloseCPLEX(&_env);
    assert(_status == 0);  // Assume CPLEX environment is closed correctly
  }
  /// and at last:
//   MIPWrapper::cleanup();
#ifdef CPLEX_PLUGIN
//  dll_close(cplex_dll);
#endif
}

void MIPCplexWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub,
                                MIPWrapper::VarType* vt, string* names) {
  /// Convert var types:
  vector<char> ctype(n);
  vector<char*> pcNames(n);
  for (size_t i = 0; i < n; ++i) {
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
        throw runtime_error("  MIPWrapper: unknown variable type");
    }
  }
  _status = dll_CPXnewcols(_env, _lp, n, obj, lb, ub, &ctype[0], &pcNames[0]);
  wrapAssert(_status == 0, "Failed to declare variables.");
}

static char get_cplex_constr_cense(MIPWrapper::LinConType sense) {
  switch (sense) {
    case MIPWrapper::LQ:
      return 'L';
    case MIPWrapper::EQ:
      return 'E';
    case MIPWrapper::GQ:
      return 'G';
    default:
      throw runtime_error("  MIPCplexWrapper: unknown constraint type");
  }
}

void MIPCplexWrapper::addRow(int nnz, int* rmatind, double* rmatval, MIPWrapper::LinConType sense,
                             double rhs, int mask, const string& rowName) {
  /// Convert var types:
  char ssense = get_cplex_constr_cense(sense);
  const int ccnt = 0;
  const int rcnt = 1;
  const int rmatbeg[] = {0};
  char* pRName = (char*)rowName.c_str();
  if ((MaskConsType_Normal & mask) != 0) {
    _status = dll_CPXaddrows(_env, _lp, ccnt, rcnt, nnz, &rhs, &ssense, rmatbeg, rmatind, rmatval,
                             nullptr, &pRName);
    wrapAssert(_status == 0, "Failed to add constraint.");
  }
  if ((MaskConsType_Usercut & mask) != 0) {
    _status =
        dll_CPXaddusercuts(_env, _lp, rcnt, nnz, &rhs, &ssense, rmatbeg, rmatind, rmatval, &pRName);
    wrapAssert(_status == 0, "Failed to add usercut.");
  }
  if ((MaskConsType_Lazy & mask) != 0) {
    _status = dll_CPXaddlazyconstraints(_env, _lp, rcnt, nnz, &rhs, &ssense, rmatbeg, rmatind,
                                        rmatval, &pRName);
    wrapAssert(_status == 0, "Failed to add lazy constraint.");
  }
}

void MIPCplexWrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind,
                                             double* rmatval, MIPWrapper::LinConType sense,
                                             double rhs, const string& rowName) {
  wrapAssert(0 <= bVal && 1 >= bVal, "mzn-cplex: addIndicatorConstraint: bVal not 0/1");
  char ssense = get_cplex_constr_cense(sense);
  _status = dll_CPXaddindconstr(_env, _lp, iBVar, 1 - bVal, nnz, rhs, ssense, rmatind, rmatval,
                                rowName.c_str());
  wrapAssert(_status == 0, "Failed to add indicator constraint.");
}

bool MIPCplexWrapper::addWarmStart(const std::vector<VarId>& vars,
                                   const std::vector<double>& vals) {
  assert(vars.size() == vals.size());
  const char* sMSName = "MZNMS";
  int msindex = -1;
  const int beg = 0;
  /// Check if we already added a start
  _status = dll_CPXgetmipstartindex(_env, _lp, sMSName, &msindex);
  if (_status != 0) {  // not existent
    // status = dll_CPXaddmipstarts (env, lp, mcnt, nzcnt, beg, varindices,
    //                            values, effortlevel, mipstartname);
    _status = dll_CPXaddmipstarts(_env, _lp, 1, vars.size(), &beg, vars.data(), vals.data(),
                                  nullptr, (char**)&sMSName);
    wrapAssert(_status == 0, "Failed to add warm start.");
  } else {
    // status = dll_CPXchgmipstarts (env, lp, mcnt, mipstartindices, nzcnt, beg, varindices, values,
    // effortlevel);
    _status = dll_CPXchgmipstarts(_env, _lp, 1, &msindex, vars.size(), &beg, vars.data(),
                                  vals.data(), nullptr);
    wrapAssert(_status == 0, "Failed to extend warm start.");
  }
  return true;
}

void MIPCplexWrapper::setVarBounds(int iVar, double lb, double ub) {
  wrapAssert(lb <= ub, "mzn-cplex: setVarBounds: lb>ub");
  char cl = 'L';
  char cu = 'U';
  _status = dll_CPXchgbds(_env, _lp, 1, &iVar, &cl, &lb);
  wrapAssert(_status == 0, "Failed to set lower bound.");
  _status = dll_CPXchgbds(_env, _lp, 1, &iVar, &cu, &ub);
  wrapAssert(_status == 0, "Failed to set upper bound.");
}

void MIPCplexWrapper::setVarLB(int iVar, double lb) {
  char cl = 'L';
  _status = dll_CPXchgbds(_env, _lp, 1, &iVar, &cl, &lb);
  wrapAssert(_status == 0, "Failed to set lower bound.");
}

void MIPCplexWrapper::setVarUB(int iVar, double ub) {
  char cu = 'U';
  _status = dll_CPXchgbds(_env, _lp, 1, &iVar, &cu, &ub);
  wrapAssert(_status == 0, "Failed to set upper bound.");
}

/// SolutionCallback ------------------------------------------------------------------------
/// CPLEX ensures thread-safety
static int CPXPUBLIC solcallback(CPXCENVptr env, void* cbdata, int wherefrom, void* cbhandle) {
  int status = 0;

  auto* info = (MIPWrapper::CBUserInfo*)cbhandle;
  auto* cw = static_cast<MIPCplexWrapper*>(info->wrapper);
  int hasincumbent = 0;
  int newincumbent = 0;
  double objVal;

  status = cw->dll_CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_NODE_COUNT,
                                      &info->pOutput->nNodes);
  if (status != 0) {
    goto TERMINATE;
  }

  status = cw->dll_CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_NODES_LEFT,
                                      &info->pOutput->nOpenNodes);
  if (status != 0) {
    goto TERMINATE;
  }

  status =
      cw->dll_CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_MIP_FEAS, &hasincumbent);
  if (status != 0) {
    goto TERMINATE;
  }

  if (hasincumbent != 0) {
    status =
        cw->dll_CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_BEST_INTEGER, &objVal);
    if (status != 0) {
      goto TERMINATE;
    }

    if (fabs(info->pOutput->objVal - objVal) > 1e-12 * (1.0 + fabs(objVal))) {
      newincumbent = 1;
      info->pOutput->objVal = objVal;
      info->pOutput->status = MIPWrapper::SAT;
      info->pOutput->statusName = "feasible from a callback";
    }
  }

  //    if ( nodecnt >= info->lastlog + 100  ||  newincumbent ) {
  //       double walltime;
  //       double dettime;

  status = cw->dll_CPXgetcallbackinfo(env, cbdata, wherefrom, CPX_CALLBACK_INFO_BEST_REMAINING,
                                      &info->pOutput->bestBound);
  //       if ( status )  goto TERMINATE;

  //       status = dll_CPXgettime (env, &walltime);
  //       if ( status )  goto TERMINATE;
  //
  //       status = dll_CPXgetdettime (env, &dettime);
  //       if ( status )  goto TERMINATE;
  //
  //    }

  if (newincumbent != 0) {
    assert(info->pOutput->x);
    status = cw->dll_CPXgetcallbackincumbent(env, cbdata, wherefrom, (double*)info->pOutput->x, 0,
                                             info->pOutput->nCols - 1);
    if (status != 0) {
      goto TERMINATE;
    }

    info->pOutput->dWallTime =
        std::chrono::duration<double>(std::chrono::steady_clock::now() - info->pOutput->dWallTime0)
            .count();
    info->pOutput->dCPUTime = double(std::clock() - info->pOutput->cCPUTime0) / CLOCKS_PER_SEC;

    /// Call the user function:
    if (info->solcbfn != nullptr) {
      (*info->solcbfn)(*info->pOutput, info->psi);
    }
    info->printed = true;
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

struct CutInfo {
  CPXLPptr lp;
  int numcols;
  int num;
  double* x;
  int* beg;
  int* ind;
  double* val;
  double* rhs;
  int nodeid;
  double nodeobjval;
  int objsen;
  MIPWrapper::CBUserInfo* info = nullptr;
};
typedef struct CutInfo CUTINFO, *CUTINFOptr;

/* Init information on the node objval for the user cut callback */

static void initnodeobjvalinfo(MIPCplexWrapper* cw, CPXENVptr env, CPXLPptr lp,
                               CUTINFOptr cutinfo) {
  cutinfo->nodeid = -1;
  cutinfo->nodeobjval = 0.0;
  cutinfo->objsen = cw->dll_CPXgetobjsen(env, lp);
  if (cutinfo->objsen == CPX_MIN) {
    cutinfo->objsen = 1;
  } else {
    cutinfo->objsen = -1;
  }
} /* END initnodeobjvalinfo */

static int CPXPUBLIC myusercutcallback(CPXCENVptr env, void* cbdata, int wherefrom, void* cbhandle,
                                       int* useraction_p) {
  int status = 0;

  auto* cutinfo = (CUTINFOptr)cbhandle;

  //   int      numcols  = cutinfo->numcols;
  //   int      numcuts  = cutinfo->num;
  //    double   *x       = cutinfo->x;
  //    int      *beg     = cutinfo->beg;
  //    int      *ind     = cutinfo->ind;
  //    double   *val     = cutinfo->val;
  //    double   *rhs     = cutinfo->rhs;
  //    int      *cutind  = NULL;
  //    double   *cutval  = NULL;
  //   double   cutvio;
  int addedcuts = 0;
  //   int      i, j, k; //, cutnz;
  MIPWrapper::CBUserInfo* info = cutinfo->info;
  auto* cw = static_cast<MIPCplexWrapper*>(info->wrapper);
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

  bool fMIPSol = true;
  if (wherefrom == CPX_CALLBACK_MIP_CUT_LOOP || wherefrom == CPX_CALLBACK_MIP_CUT_LAST) {
    int oldnodeid = cutinfo->nodeid;
    double oldnodeobjval = cutinfo->nodeobjval;

    fMIPSol = false;

    /* Retrieve nodeid and node objval of the current node */

    status = cw->dll_CPXgetcallbacknodeinfo(env, cbdata, wherefrom, 0,
                                            CPX_CALLBACK_INFO_NODE_SEQNUM, &cutinfo->nodeid);
    if (status != 0) {
      fprintf(stderr, "Failed to get node id.\n");
      goto TERMINATE;
    }

    status = cw->dll_CPXgetcallbacknodeinfo(env, cbdata, wherefrom, 0,
                                            CPX_CALLBACK_INFO_NODE_OBJVAL, &cutinfo->nodeobjval);
    if (status != 0) {
      fprintf(stderr, "Failed to get node objval.\n");
      goto TERMINATE;
    }

    /* Abort the cut loop if we are stuck at the same node
       as before and there is no progress in the node objval */

    if (oldnodeid == cutinfo->nodeid) {
      double objchg = (cutinfo->nodeobjval - oldnodeobjval);
      /* Multiply objchg by objsen to normalize
         the change in the objective function to
         the case of a minimization problem */
      objchg *= cutinfo->objsen;
      if (objchg <= EPSOBJ) {
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

  if (info->cutcbfn != nullptr) {  // if cut handler given
    MIPWrapper::Output outpRlx;
    outpRlx.x = info->pOutput->x;  // using the sol output storage  TODO?
    outpRlx.nCols = info->pOutput->nCols;
    assert(outpRlx.x && outpRlx.nCols);
    status = cw->dll_CPXgetcallbacknodex(env, cbdata, wherefrom, (double*)outpRlx.x, 0,
                                         outpRlx.nCols - 1);
    if (status != 0) {
      fprintf(stderr, "Cut callback: failed to get node solution.\n");
      goto TERMINATE;
    }
    MIPWrapper::CutInput cutInput;
    info->cutcbfn(outpRlx, cutInput, info->psi, fMIPSol);
    static int nCuts = 0;
    nCuts += cutInput.size();
    // if ( cutInput.size() )
    //  cerr << "\n   N CUTS:  " << nCuts << endl;
    for (auto& cd : cutInput) {
      if ((cd.mask & (MIPWrapper::MaskConsType_Usercut | MIPWrapper::MaskConsType_Lazy)) == 0) {
        throw runtime_error("Cut callback: should be user/lazy");
      }
      /* Use a cut violation tolerance of 0.01 */
      if (true) {  // NOLINT: cutvio > 0.01 ) {
        status = cw->dll_CPXcutcallbackadd(env, cbdata, wherefrom, cd.rmatind.size(), cd.rhs,
                                           get_cplex_constr_cense(cd.sense), cd.rmatind.data(),
                                           cd.rmatval.data(),
                                           CPX_USECUT_FORCE);  // PURGE?
        if (status != 0) {
          fprintf(stderr, "CPLEX callback: failed to add cut.\n");
          goto TERMINATE;
        }
        addedcuts++;
      }
    }
  }

  /* Tell CPLEX that cuts have been created */
  if (addedcuts > 0) {
    *useraction_p = CPX_CALLBACK_SET;
  }

TERMINATE:

  return (status);

} /* END myusercutcallback */

// ----------------- END Cut callbacks ------------------

MIPCplexWrapper::Status MIPCplexWrapper::convertStatus(int cplexStatus) {
  Status s = Status::UNKNOWN;
  /* Converting the status. */
  switch (cplexStatus) {
    case CPXMIP_OPTIMAL:
      s = Status::OPT;
      wrapAssert(dll_CPXgetsolnpoolnumsolns(_env, _lp) != 0, "Optimality reported but pool empty?",
                 false);
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
      wrapAssert(dll_CPXgetsolnpoolnumsolns(_env, _lp) != 0, "Feasibility reported but pool empty?",
                 false);
      break;
    case CPXMIP_UNBOUNDED:
      s = Status::UNBND;
      break;
      //      case CPXMIP_ABORT_INFEAS:
    case CPXMIP_FAIL_INFEAS:
      s = Status::ERROR_STATUS;
      break;
    default:
      //      case CPXMIP_OPTIMAL_TOL:
      //      case CPXMIP_ABORT_RELAXATION_UNBOUNDED:
      if (dll_CPXgetsolnpoolnumsolns(_env, _lp) != 0) {
        s = Status::SAT;
      } else {
        s = Status::UNKNOWN;
      }
  }
  return s;
}

void msgfunction(void* handle, const char* msg_string) { cerr << msg_string << flush; }

void MIPCplexWrapper::solve() {  // Move into ancestor?

  /////////////// Last-minute solver options //////////////////
  // Before all manual params ???
  if (!_options->sReadParams.empty()) {
    _status = dll_CPXreadcopyparam(_env, _options->sReadParams.c_str());
    wrapAssert(_status == 0, "Failed to read CPLEX parameters.", false);
  }

  /* Turn on output to the screen */
  if (fVerbose) {
    CPXCHANNELptr chnl[4];
    dll_CPXgetchannels(_env, &chnl[0], &chnl[1], &chnl[2], &chnl[3]);
    for (int i = 0; i < 3; ++i) {
      _status = dll_CPXaddfuncdest(_env, chnl[i], nullptr, msgfunction);
    }
    //     status = dll_CPXsetintparam(env, CPXPARAM_ScreenOutput,
    //       fVerbose ? CPX_ON : CPX_OFF);  // also when flagIntermediate?  TODO
    //     wrapAssert(!status, "  CPLEX Warning: Failure to switch screen indicator.", false);
  }
  _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Display,
                               fVerbose ? 2 : 0);  // also when flagIntermediate?  TODO
  wrapAssert(_status == 0, "  CPLEX Warning: Failure to switch logging.", false);
  /// Make it wall time by default, 12.8
  //    status =  dll_CPXsetintparam (env, CPXPARAM_ClockType, 1);            // CPU time
  //    wrapAssert(!status, "  CPLEX Warning: Failure to measure CPU time.", false);
  _status = dll_CPXsetintparam(_env, CPX_PARAM_MIPCBREDLP, CPX_OFF);  // Access original model
  wrapAssert(_status == 0, "  CPLEX Warning: Failure to set access original model in callbacks.",
             false);
  if (!_options->sExportModel.empty()) {
    _status = dll_CPXwriteprob(_env, _lp, _options->sExportModel.c_str(), nullptr);
    wrapAssert(_status == 0, "Failed to write LP to disk.", false);
  }

  /// TODO
  //     if(all_solutions && obj.getImpl()) {
  //       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
  //       _ilocplex->use(SolutionCallback(_iloenv, lastObjVal, *this));
  // Turn off CPLEX logging

  if (_options->nThreads > 0) {
    _status = dll_CPXsetintparam(_env, CPXPARAM_Threads, _options->nThreads);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_Threads.", false);
  }

  if (_options->nTimeout > 0) {
    _status = dll_CPXsetdblparam(_env, CPXPARAM_TimeLimit,
                                 static_cast<double>(_options->nTimeout) / 1000.0);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_TimeLimit.", false);
  }
  if (_options->nSolLimit > 0) {
    _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Limits_Solutions, _options->nSolLimit);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_MIP_Limits_Solutions.", false);
  }
  if (_options->nSeed >= 0) {
    _status = dll_CPXsetintparam(_env, CPXPARAM_RandomSeed, _options->nSeed);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_RandomSeed.", false);
  }
  if (_options->nMIPFocus > 0) {
    _status = dll_CPXsetintparam(_env, CPXPARAM_Emphasis_MIP, _options->nMIPFocus);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_Emphasis_MIP.", false);
  }

  if (_options->nWorkMemLimit > 0) {
    _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Strategy_File, 3);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_MIP_Strategy_File.", false);
    _status = dll_CPXsetdblparam(_env, CPXPARAM_WorkMem,
                                 1024.0 * _options->nWorkMemLimit);  // MB in CPLEX
    wrapAssert(_status == 0, "Failed to set CPXPARAM_WorkMem.", false);
  }

  if (!_options->sNodefileDir.empty()) {
    _status = dll_CPXsetstrparam(_env, CPXPARAM_WorkDir, _options->sNodefileDir.c_str());
    wrapAssert(_status == 0, "Failed to set CPXPARAM_WorkDir.", false);
  }

  if (_options->absGap >= 0.0) {
    _status = dll_CPXsetdblparam(_env, CPXPARAM_MIP_Tolerances_AbsMIPGap, _options->absGap);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_MIP_Tolerances_AbsMIPGap.", false);
  }
  if (_options->relGap >= 0.0) {
    _status = dll_CPXsetdblparam(_env, CPXPARAM_MIP_Tolerances_MIPGap, _options->relGap);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_MIP_Tolerances_MIPGap.", false);
  }
  if (_options->intTol >= 0.0) {
    _status = dll_CPXsetdblparam(_env, CPXPARAM_MIP_Tolerances_Integrality, _options->intTol);
    wrapAssert(_status == 0, "Failed to set CPXPARAM_MIP_Tolerances_Integrality.", false);
  }

  //    status =  dll_CPXsetdblparam (_env, CPXPARAM_MIP_Tolerances_ObjDifference, objDiff);
  //    wrapAssert(!status, "Failed to set CPXPARAM_MIP_Tolerances_ObjDifference.", false);

  /// Solution callback
  output.nCols = colObj.size();
  _x.resize(output.nCols);
  output.x = &_x[0];
  if (_options->flagIntermediate && (cbui.solcbfn != nullptr)) {
    _status = dll_CPXsetinfocallbackfunc(_env, solcallback, &cbui);
    wrapAssert(_status == 0, "Failed to set solution callback", false);
  }
  if (cbui.cutcbfn != nullptr) {
    assert(cbui.cutMask & (MaskConsType_Usercut | MaskConsType_Lazy));
    if ((cbui.cutMask & MaskConsType_Usercut) != 0) {
      // For user cuts, needs to keep some info after presolve
      if (fVerbose) {
        cerr << "  MIPCplexWrapper: user cut callback enabled, setting params" << endl;
      }
      CUTINFO usercutinfo;  // THREADS?  TODO
      usercutinfo.info = &cbui;
      /* Init information on the node objval for the user cut callback */
      initnodeobjvalinfo(this, _env, _lp, &usercutinfo);
      /* Assure linear mappings between the presolved and original
          models */
      _status = dll_CPXsetintparam(_env, CPXPARAM_Preprocessing_Linear, 0);
      wrapAssert(_status == 0, "CPLEX: setting prepro_linear");
      /* Turn on traditional search for use with control callbacks */
      _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Strategy_Search, CPX_MIPSEARCH_TRADITIONAL);
      wrapAssert(_status == 0, "CPLEX: setting traditional search");
      /* Let MIP callbacks work on the original model */
      _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Strategy_CallbackReducedLP, CPX_OFF);
      wrapAssert(_status == 0, "CPLEX: setting callbacks to work on orig model");
      /// And
      /* Set up to use MIP usercut callback */

      _status = dll_CPXsetusercutcallbackfunc(_env, myusercutcallback, &usercutinfo);
      wrapAssert(_status == 0, "CPLEX: setting user cut callback");
    }
    if ((cbui.cutMask & MaskConsType_Lazy) != 0) {
      if (fVerbose) {
        cerr << "  MIPCplexWrapper: lazy cut callback enabled, setting params" << endl;
      }
      CUTINFO lazyconinfo;
      lazyconinfo.info = &cbui;
      /* Init information on the node objval for the user cut callback.
          No need to initialize the information on the node objval,
          for the lazy constraint callback, because those information are
          used only in the user cut callback. */
      initnodeobjvalinfo(this, _env, _lp, &lazyconinfo);
      /* Assure linear mappings between the presolved and original
          models */
      _status = dll_CPXsetintparam(_env, CPXPARAM_Preprocessing_Linear, 0);
      wrapAssert(_status == 0, "CPLEX: setting prepro_linear");
      /* Turn on traditional search for use with control callbacks */
      //       _status = dll_CPXsetintparam (_env, CPXPARAM_MIP_Strategy_Search,
      //                                 CPX_MIPSEARCH_TRADITIONAL);
      //       wrapAssert ( !_status, "CPLEX: setting traditional search" );
      /* Let MIP callbacks work on the original model */
      _status = dll_CPXsetintparam(_env, CPXPARAM_MIP_Strategy_CallbackReducedLP, CPX_OFF);
      wrapAssert(_status == 0, "CPLEX: setting callbacks to work on orig model");
      /* Set up to use MIP lazyconstraint callback. The callback funtion
       * registered is the same, but the data will be different. */

      _status = dll_CPXsetlazyconstraintcallbackfunc(_env, myusercutcallback, &lazyconinfo);
      wrapAssert(_status == 0, "CPLEX: setting lazy cut callback");
    }
  }

  // Process extra flags
  for (auto& it : _options->extraParams) {
    int param;
    int param_type;
    auto name = it.first.substr(8);
    _status = dll_CPXgetparamnum(_env, name.c_str(), &param);
    wrapAssert(_status == 0, "CPLEX: could not find parameter " + name);
    _status = dll_CPXgetparamtype(_env, param, &param_type);
    wrapAssert(_status == 0, "CPLEX: could not get type for parameter " + name);
    switch (param_type) {
      case CPX_PARAMTYPE_INT:
        _status = dll_CPXsetintparam(_env, param, stoi(it.second));
        wrapAssert(_status == 0, "CPLEX: failed to set parameter " + name);
        break;
      case CPX_PARAMTYPE_LONG:
        _status = dll_CPXsetlongparam(_env, param, stoll(it.second));
        wrapAssert(_status == 0, "CPLEX: failed to set parameter " + name);
        break;
      case CPX_PARAMTYPE_DOUBLE:
        _status = dll_CPXsetdblparam(_env, param, stod(it.second));
        wrapAssert(_status == 0, "CPLEX: failed to set parameter " + name);
        break;
      case CPX_PARAMTYPE_STRING:
        _status = dll_CPXsetstrparam(_env, param, it.second.c_str());
        wrapAssert(_status == 0, "CPLEX: failed to set parameter " + name);
        break;
      default:
        wrapAssert(false, "CPLEX: unknown type for parameter " + name);
        break;
    }
  }

  /// after all modifs
  if (!_options->sWriteParams.empty()) {
    _status = dll_CPXwriteparam(_env, _options->sWriteParams.c_str());
    wrapAssert(_status == 0, "Failed to write CPLEX parameters.", false);
  }

  // _status = dll_CPXgettime (_env, &output.dCPUTime);
  // wrapAssert(!_status, "Failed to get time stamp.", false);
  cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
  cbui.pOutput->cCPUTime0 = std::clock();

  /* Optimize the problem and obtain solution. */
  _status = dll_CPXmipopt(_env, _lp);
  wrapAssert(_status == 0, "Failed to optimize MIP.");

  output.dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output.dWallTime0).count();
  double tmNow = std::clock();
  // _status = dll_CPXgettime (_env, &tmNow);   Buggy in 12.7.1.0
  wrapAssert(_status == 0, "Failed to get time stamp.", false);
  output.dCPUTime = (tmNow - cbui.pOutput->cCPUTime0) / CLOCKS_PER_SEC;

  int solstat = dll_CPXgetstat(_env, _lp);
  output.status = convertStatus(solstat);
  output.statusName = dll_CPXgetstatstring(_env, solstat, _cplexStatusBuffer);

  /// Continuing to fill the output object:
  if (Status::OPT == output.status || Status::SAT == output.status) {
    _status = dll_CPXgetobjval(_env, _lp, &output.objVal);
    wrapAssert(_status == 0, "No MIP objective value available.");

    /* The size of the problem should be obtained by asking CPLEX what
        the actual size is, rather than using what was passed to CPXcopylp.
        cur_numrows and cur_numcols store the current number of rows and
        columns, respectively.  */   // ?????????????? TODO

    //    int cur_numrows = dll_CPXgetnumrows (_env, lp);
    int cur_numcols = dll_CPXgetnumcols(_env, _lp);
    assert(cur_numcols == colObj.size());

    _x.resize(cur_numcols);
    output.x = &_x[0];
    _status = dll_CPXgetx(_env, _lp, &_x[0], 0, cur_numcols - 1);
    wrapAssert(_status == 0, "Failed to get variable values.");
    if (cbui.solcbfn != nullptr /*&& (!_options->flagIntermediate || !cbui.printed)*/) {
      cbui.solcbfn(output, cbui.psi);
    }
  }
  output.bestBound = 1e308;
  _status = dll_CPXgetbestobjval(_env, _lp, &output.bestBound);
  wrapAssert(_status == 0, "Failed to get the best bound.", false);
  output.nNodes = dll_CPXgetnodecnt(_env, _lp);
  output.nOpenNodes = dll_CPXgetnodeleftcnt(_env, _lp);
}

void MIPCplexWrapper::setObjSense(int s) {
  _status = dll_CPXchgobjsen(_env, _lp, -s);  // +1 for min in CPLEX
  wrapAssert(_status == 0, "Failed to set obj sense.");
}
