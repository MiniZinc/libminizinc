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
#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/utils_savestream.hh>

#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

#ifdef GUROBI_PLUGIN
#ifdef HAS_DLFCN_H
#include <dlfcn.h>
#elif defined HAS_WINDOWS_H
#define NOMINMAX  // Ensure the words min/max remain available
#include <Windows.h>
#undef ERROR
#endif
#endif

#include <minizinc/solvers/MIP/MIP_gurobi_wrap.hh>
#include <minizinc/utils.hh>

using namespace std;

string MIPGurobiWrapper::getDescription(FactoryOptions& factoryOpt,
                                        MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "MIP wrapper for Gurobi library " << getVersion(factoryOpt, nullptr);
  oss << ".  Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}

string MIPGurobiWrapper::getVersion(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  MIPGurobiWrapper mgw(factoryOpt, nullptr);  // to avoid opening the env
  try {
    mgw.checkDLL();
    int major;
    int minor;
    int technical;
    mgw.dll_GRBversion(&major, &minor, &technical);
    oss << major << '.' << minor << '.' << technical;
    return oss.str();
  } catch (MiniZinc::InternalError&) {
    return "<unknown version>";
  }
}

vector<string> MIPGurobiWrapper::getRequiredFlags(FactoryOptions& f) {
  FactoryOptions factoryOpt;
  MIPGurobiWrapper mgw(factoryOpt, nullptr);
  try {
    mgw.checkDLL();
    return {};
  } catch (MiniZinc::InternalError&) {
    return {"--gurobi-dll"};
  }
}

vector<string> MIPGurobiWrapper::getFactoryFlags() { return {"--gurobi-dll"}; }

string MIPGurobiWrapper::getId() { return "gurobi"; }

string MIPGurobiWrapper::getName() { return "Gurobi"; }

vector<string> MIPGurobiWrapper::getTags() { return {"mip", "float", "api"}; }

vector<string> MIPGurobiWrapper::getStdFlags() { return {"-i", "-p", "-s", "-v"}; }

vector<string> gurobi_dlls() {
  const vector<string> versions = {
      "913", "912", "911", "910", "904",  // Potential future versions which should load correctly
      "903", "902", "901", "900", "811", "810", "801", "800", "752",
      "751", "750", "702", "701", "700", "652", "651", "650"};
  vector<string> dlls;
  string lastMajorVersion;
  for (const auto& version : versions) {
    string majorVersion = version.substr(0, 2);
    if (majorVersion != lastMajorVersion) {
      dlls.push_back("gurobi" + majorVersion);
      lastMajorVersion = majorVersion;
    }
#ifdef _WIN32
    dlls.push_back("C:\\gurobi" + version + "\\win64\\bin\\gurobi" + majorVersion + ".dll");
#elif __APPLE__
    dlls.push_back("/Library/gurobi" + version + "/mac64/lib/libgurobi" + majorVersion + ".dylib");
#else
    dlls.push_back("/opt/gurobi" + version + "/linux64/lib/libgurobi" + majorVersion + ".so");
#endif
  }

  return dlls;
}

void MIPGurobiWrapper::Options::printHelp(ostream& os) {
  os << "GUROBI MIP wrapper options:"
     << std::endl
     // -s                  print statistics
     //            << "  --readParam <file>  read GUROBI parameters from file
     //               << "--writeParam <file> write GUROBI parameters to file
     //               << "--tuneParam         instruct GUROBI to tune parameters instead of solving
     << "  -f\n    free search (default)" << std::endl
     << "  --fixed-search\n    fixed search (approximation of the model's one by branching "
        "priorities)"
     << std::endl
     << "  --uniform-search\n    'more fixed' search (all variables in the search anns get "
        "priority 1)"
     << std::endl
     << "  --mipfocus <n>\n    1: feasibility, 2: optimality, 3: move bound (default is 0, "
        "balanced)"
     << std::endl
     << "  -i\n    print intermediate solutions for optimization problems" << std::endl
     << "  -p <N>, --parallel <N>\n    use N threads, default: 1."
     << std::endl
     //   << "  --nomippresolve     disable MIP presolving   NOT IMPL" << std::endl
     << "  --solver-time-limit <N>, --solver-time\n"
        "    stop search after N milliseconds wall time"
     << std::endl
     << "  --solver-time-limit-feas <N>, --solver-tlf\n"
        "    stop search after N milliseconds wall time after the first feasible solution"
     << std::endl
     << "  -n <N>, --num-solutions <N>\n"
        "    stop search after N solutions"
     << std::endl
     << "  -r <N>, --random-seed <N>\n"
        "    random seed, integer"
     << std::endl
     << "  --workmem <N>, --nodefilestart <N>\n"
        "    maximal RAM for node tree used before writing to node file, GB, default: 0.5"
     << std::endl
     << "  --nodefiledir <path>\n"
        "    nodefile directory"
     << std::endl
     << "  --writeModel <file>\n    write model to <file> (.lp, .mps, .sav, ...)" << std::endl
     << "  --readParam <file>\n     read GUROBI parameters from file" << std::endl
     << "  --writeParam <file>\n    write GUROBI parameters to file" << std::endl
     << "  --readConcurrentParam <fileN>\n"
        "    read GUROBI parameters from file. Several such commands provide the"
        "    parameter files for concurrent solves (applied after all other settings)"
     << std::endl
     //   << "  --tuneParam         instruct GUROBI to tune parameters instead of solving   NOT
     //   IMPL"

     << "\n  --absGap <n>\n    absolute gap |primal-dual| to stop" << std::endl
     << "  --relGap <n>\n    relative gap |primal-dual|/<solver-dep> to stop. Default 1e-8, set <0 "
        "to use backend's default"
     << std::endl
     << "  --feasTol <n>\n   primal feasibility tolerance. Default 1e-8" << std::endl
     << "  --intTol <n>\n    integrality tolerance for a variable. Gurobi recommends at least "
        "feasTol. Default 1e-8"
     << std::endl
     //   << "  --objDiff <n>       objective function discretization. Default 1.0" << std::endl

     << "  --nonConvex <n>\n    non-convexity. -1: solver default, 0: none, 1: if presolved, 2: "
        "global. Default value 2."
     << std::endl

     << "\n  --gurobi-dll <file> or <basename>\n    Gurobi DLL, or base name, such as gurobi75, "
        "when using plugin. Default range tried: "
     << gurobi_dlls().front() << " .. " << gurobi_dlls().back() << std::endl
     << std::endl;
}

bool MIPGurobiWrapper::FactoryOptions::processOption(int& i, std::vector<std::string>& argv,
                                                     const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  return cop.get("--gurobi-dll", &gurobiDll);
}

bool MIPGurobiWrapper::Options::processOption(int& i, std::vector<std::string>& argv,
                                              const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  std::string buf;
  if (cop.get("-i")) {
    flagIntermediate = true;
  } else if (string(argv[i]) == "-f") {              // NOLINT: Allow repeated empty if
  } else if (string(argv[i]) == "--fixed-search") {  // NOLINT: Allow repeated empty if
    nFreeSearch = MIPGurobiWrapper::SearchType::FIXED_SEARCH;
  } else if (string(argv[i]) == "--uniform-search") {  // NOLINT: Allow repeated empty if
    nFreeSearch = MIPGurobiWrapper::SearchType::UNIFORM_SEARCH;
  } else if (cop.get("--mipfocus --mipFocus --MIPFocus --MIPfocus",
                     &nMIPFocus)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--writeModel --exportModel --writemodel --exportmodel", &buf)) {
    sExportModel = MiniZinc::FileUtils::file_path(buf, workingDir);
  } else if (cop.get("-p --parallel", &nThreads)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--solver-time-limit --solver-time",
                     &nTimeout1000)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--solver-time-limit-feas --solver-tlf",
                     &nTimeoutFeas1000)) {                 // NOLINT: Allow repeated empty if
  } else if (cop.get("-n --num-solutions", &nSolLimit)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("-r --random-seed", &nSeed)) {        // NOLINT: Allow repeated empty if
  } else if (cop.get("--workmem --nodefilestart",
                     &nWorkMemLimit)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--nodefiledir --NodefileDir",
                     &sNodefileDir)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--readParam --readParams", &buf)) {
    sReadParams = MiniZinc::FileUtils::file_path(buf, workingDir);
  } else if (cop.get("--writeParam --writeParams", &buf)) {
    sWriteParams = MiniZinc::FileUtils::file_path(buf, workingDir);
  } else if (cop.get("--readConcurrentParam --readConcurrentParams", &buf)) {
    sConcurrentParamFiles.push_back(MiniZinc::FileUtils::file_path(buf, workingDir));
  } else if (cop.get("--absGap", &absGap)) {    // NOLINT: Allow repeated empty if
  } else if (cop.get("--relGap", &relGap)) {    // NOLINT: Allow repeated empty if
  } else if (cop.get("--feasTol", &feasTol)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--intTol", &intTol)) {    // NOLINT: Allow repeated empty if
  } else if (cop.get("--nonConvex --nonconvex --NonConvex",
                     &nonConvex)) {  // NOLINT: Allow repeated empty if
    //   } else if ( cop.get( "--objDiff", &objDiff ) ) {
  } else {
    return false;
  }
  return true;
}

void MIPGurobiWrapper::wrapAssert(bool cond, const string& msg, bool fTerm) {
  if (!cond) {
    _gurobiBuffer = "[NO ERROR STRING GIVEN]";
    if (_error != 0) {
      _gurobiBuffer = dll_GRBgeterrormsg(_env);
    }
    string msgAll =
        ("  MIPGurobiWrapper runtime error:  " + _gurobiBuffer + "\nMessage from caller: " + msg);
    cerr << msgAll << "\nGurobi error code: " << _error << endl;
    if (fTerm) {
      cerr << "TERMINATING." << endl;
      throw runtime_error(msgAll);
    }
  }
}

#ifdef GUROBI_PLUGIN

namespace {
void* dll_open(const char* file) {
#ifdef HAS_DLFCN_H
  if (MiniZinc::FileUtils::is_absolute(file)) {
    return dlopen(file, RTLD_NOW);
  }
  return dlopen((std::string("lib") + file + ".so").c_str(), RTLD_NOW);

#else
  if (MiniZinc::FileUtils::is_absolute(file)) {
    return LoadLibrary(file);
  }
  return LoadLibrary((std::string(file) + ".dll").c_str());
#endif
}
void* dll_sym(void* dll, const char* sym) {
#ifdef HAS_DLFCN_H
  void* ret = dlsym(dll, sym);
#else
  void* ret = GetProcAddress((HMODULE)dll, sym);
#endif
  if (ret == nullptr) {
    throw MiniZinc::InternalError("cannot load symbol " + string(sym) + " from gurobi dll");
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

void MIPGurobiWrapper::checkDLL() {
#ifdef GUROBI_PLUGIN
  _gurobiDll = nullptr;
  if (!_factoryOptions.gurobiDll.empty()) {
    _gurobiDll = dll_open(_factoryOptions.gurobiDll.c_str());
  } else {
    for (const auto& s : gurobi_dlls()) {
      _gurobiDll = dll_open(s.c_str());
      if (nullptr != _gurobiDll) {
        break;
      }
    }
  }

  if (_gurobiDll == nullptr) {
    if (_factoryOptions.gurobiDll.empty()) {
      throw MiniZinc::InternalError("cannot load gurobi dll, specify --gurobi-dll");
    }
    throw MiniZinc::InternalError("cannot load gurobi dll `" + _factoryOptions.gurobiDll + "'");
  }

  *(void**)(&dll_GRBversion) = dll_sym(_gurobiDll, "GRBversion");
  *(void**)(&dll_GRBaddconstr) = dll_sym(_gurobiDll, "GRBaddconstr");
  *(void**)(&dll_GRBaddgenconstrMin) = dll_sym(_gurobiDll, "GRBaddgenconstrMin");
  *(void**)(&dll_GRBaddqconstr) = dll_sym(_gurobiDll, "GRBaddqconstr");
  *(void**)(&dll_GRBaddgenconstrIndicator) = dll_sym(_gurobiDll, "GRBaddgenconstrIndicator");
  *(void**)(&dll_GRBaddvars) = dll_sym(_gurobiDll, "GRBaddvars");
  *(void**)(&dll_GRBcbcut) = dll_sym(_gurobiDll, "GRBcbcut");
  *(void**)(&dll_GRBcbget) = dll_sym(_gurobiDll, "GRBcbget");
  *(void**)(&dll_GRBcblazy) = dll_sym(_gurobiDll, "GRBcblazy");
  *(void**)(&dll_GRBfreeenv) = dll_sym(_gurobiDll, "GRBfreeenv");
  *(void**)(&dll_GRBfreemodel) = dll_sym(_gurobiDll, "GRBfreemodel");
  *(void**)(&dll_GRBgetdblattr) = dll_sym(_gurobiDll, "GRBgetdblattr");
  *(void**)(&dll_GRBgetdblattrarray) = dll_sym(_gurobiDll, "GRBgetdblattrarray");
  *(void**)(&dll_GRBgetenv) = dll_sym(_gurobiDll, "GRBgetenv");
  *(void**)(&dll_GRBgeterrormsg) = dll_sym(_gurobiDll, "GRBgeterrormsg");
  *(void**)(&dll_GRBgetintattr) = dll_sym(_gurobiDll, "GRBgetintattr");
  *(void**)(&dll_GRBloadenv) = dll_sym(_gurobiDll, "GRBloadenv");
  *(void**)(&dll_GRBgetconcurrentenv) = dll_sym(_gurobiDll, "GRBgetconcurrentenv");
  *(void**)(&dll_GRBnewmodel) = dll_sym(_gurobiDll, "GRBnewmodel");
  *(void**)(&dll_GRBoptimize) = dll_sym(_gurobiDll, "GRBoptimize");
  *(void**)(&dll_GRBreadparams) = dll_sym(_gurobiDll, "GRBreadparams");
  *(void**)(&dll_GRBsetcallbackfunc) = dll_sym(_gurobiDll, "GRBsetcallbackfunc");
  *(void**)(&dll_GRBsetdblparam) = dll_sym(_gurobiDll, "GRBsetdblparam");
  *(void**)(&dll_GRBsetintattr) = dll_sym(_gurobiDll, "GRBsetintattr");
  *(void**)(&dll_GRBsetintattrlist) = dll_sym(_gurobiDll, "GRBsetintattrlist");
  *(void**)(&dll_GRBsetdblattrelement) = dll_sym(_gurobiDll, "GRBsetdblattrelement");
  *(void**)(&dll_GRBsetdblattrlist) = dll_sym(_gurobiDll, "GRBsetdblattrlist");
  *(void**)(&dll_GRBsetobjectiven) = dll_sym(_gurobiDll, "GRBsetobjectiven");
  *(void**)(&dll_GRBsetintparam) = dll_sym(_gurobiDll, "GRBsetintparam");
  *(void**)(&dll_GRBsetstrparam) = dll_sym(_gurobiDll, "GRBsetstrparam");
  *(void**)(&dll_GRBterminate) = dll_sym(_gurobiDll, "GRBterminate");
  *(void**)(&dll_GRBupdatemodel) = dll_sym(_gurobiDll, "GRBupdatemodel");
  *(void**)(&dll_GRBwrite) = dll_sym(_gurobiDll, "GRBwrite");
  *(void**)(&dll_GRBwriteparams) = dll_sym(_gurobiDll, "GRBwriteparams");
  *(void**)(&dll_GRBemptyenv) = dll_sym(_gurobiDll, "GRBemptyenv");
  *(void**)(&dll_GRBgetnumparams) = dll_sym(_gurobiDll, "GRBgetnumparams");
  *(void**)(&dll_GRBgetparamname) = dll_sym(_gurobiDll, "GRBgetparamname");
  *(void**)(&dll_GRBgetparamtype) = dll_sym(_gurobiDll, "GRBgetparamtype");
  *(void**)(&dll_GRBgetintparaminfo) = dll_sym(_gurobiDll, "GRBgetintparaminfo");
  *(void**)(&dll_GRBgetdblparaminfo) = dll_sym(_gurobiDll, "GRBgetdblparaminfo");
  *(void**)(&dll_GRBgetstrparaminfo) = dll_sym(_gurobiDll, "GRBgetstrparaminfo");

#else

  dll_GRBversion = GRBversion;
  dll_GRBaddconstr = GRBaddconstr;
  dll_GRBaddgenconstrIndicator = GRBaddgenconstrIndicator;
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
  dll_GRBsetdblattrelement = GRBsetdblattrelement;
  dll_GRBsetdblattrlist = GRBsetdblattrlist;
  dll_GRBsetintparam = GRBsetintparam;
  dll_GRBsetstrparam = GRBsetstrparam;
  dll_GRBterminate = GRBterminate;
  dll_GRBupdatemodel = GRBupdatemodel;
  dll_GRBwrite = GRBwrite;
  dll_GRBwriteparams = GRBwriteparams;
  dll_GRBemptyenv = GRBemptyenv;
  dll_GRBgetnumparams = GRBgetnumparams;
  dll_GRBgetparamname = GRBgetparamname;
  dll_GRBgetparamtype = GRBgetparamtype;
  dll_GRBgetintparaminfo = dll_GRBgetintparaminfo;
  dll_GRBgetdblparaminfo = dll_GRBgetdblparaminfo;
  dll_GRBgetstrparaminfo = dll_GRBgetstrparaminfo;

#endif
}

void MIPGurobiWrapper::openGUROBI() {
  checkDLL();

  /* Initialize the GUROBI environment */
  {
    //   cout << "% " << flush;               // Gurobi 7.5.2 prints "Academic License..."
    MiniZinc::StreamRedir redirStdout(stdout, stderr);
    _error = dll_GRBloadenv(&_env, nullptr);
  }
  wrapAssert(_error == 0, "Could not open GUROBI environment.");
  _error = dll_GRBsetintparam(_env, "OutputFlag", 0);  // Switch off output
  //   _error = dll_GRBsetintparam(_env, "LogToConsole",
  //                            fVerbose ? 1 : 0);  // also when flagIntermediate?  TODO
  /* Create the problem. */
  _error =
      dll_GRBnewmodel(_env, &_model, "mzn_gurobi", 0, nullptr, nullptr, nullptr, nullptr, nullptr);
  wrapAssert(_model != nullptr, "Failed to create LP.");
}

void MIPGurobiWrapper::closeGUROBI() {
  /* Free model */

  // If not allocated, skip
  if (nullptr != _model) {
    /* Free up the problem as allocated by GRB_createprob, if necessary */
    dll_GRBfreemodel(_model);
    _model = nullptr;
  }

  /* Free environment */

  if (nullptr != _env) {
    dll_GRBfreeenv(_env);
  }
  /// and at last:
//   MIPWrapper::cleanup();
#ifdef GUROBI_PLUGIN
  // dll_close(_gurobiDll);    // Is called too many times, disabling. 2019-05-06
#endif
}

std::vector<MiniZinc::SolverConfig::ExtraFlag> MIPGurobiWrapper::getExtraFlags(
    FactoryOptions& factoryOpt) {
  enum GurobiParamType { T_INT = 1, T_DOUBLE = 2, T_STRING = 3 };

  MIPGurobiWrapper mgw(factoryOpt, nullptr);
  GRBenv* env;
  try {
    mgw.checkDLL();
    mgw.dll_GRBemptyenv(&env);
    int num_params = mgw.dll_GRBgetnumparams(env);
    std::vector<MiniZinc::SolverConfig::ExtraFlag> flags;
    flags.reserve(num_params);
    for (int i = 0; i < num_params; i++) {
      char* name;
      mgw.dll_GRBgetparamname(env, i, &name);
      std::string param_name(name);
      MiniZinc::SolverConfig::ExtraFlag::FlagType param_type;
      std::vector<std::string> param_range;
      std::string param_default;
      int type = mgw.dll_GRBgetparamtype(env, name);
      if (param_name == GRB_INT_PAR_THREADS || param_name == GRB_DBL_PAR_TIMELIMIT ||
          param_name == GRB_INT_PAR_SOLUTIONLIMIT || param_name == GRB_INT_PAR_SEED ||
          param_name == GRB_DBL_PAR_NODEFILESTART || param_name == GRB_STR_PAR_NODEFILEDIR ||
          param_name == GRB_DBL_PAR_MIPGAPABS || param_name == GRB_INT_PAR_MIPFOCUS ||
          param_name == GRB_DBL_PAR_MIPGAP || param_name == GRB_DBL_PAR_INTFEASTOL ||
          param_name == GRB_DBL_PAR_FEASIBILITYTOL ||
#ifdef GRB_INT_PAR_NONCONVEX
          param_name == GRB_INT_PAR_NONCONVEX ||
#endif
          param_name == GRB_INT_PAR_PRECRUSH || param_name == GRB_INT_PAR_LAZYCONSTRAINTS ||
          param_name == GRB_STR_PAR_DUMMY) {
        // These parameters are handled by us or are not useful
        continue;
      }
      switch (type) {
        case T_INT: {
          int current_value;
          int min_value;
          int max_value;
          int default_value;
          mgw.dll_GRBgetintparaminfo(env, name, &current_value, &min_value, &max_value,
                                     &default_value);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
          param_range = {std::to_string(min_value), std::to_string(max_value)};
          param_default = std::to_string(default_value);
          break;
        }
        case T_DOUBLE: {
          double current_value;
          double min_value;
          double max_value;
          double default_value;
          mgw.dll_GRBgetdblparaminfo(env, name, &current_value, &min_value, &max_value,
                                     &default_value);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_FLOAT;
          param_range = {std::to_string(min_value), std::to_string(max_value)};
          param_default = std::to_string(default_value);
          break;
        }
        case T_STRING: {
          char current_value[GRB_MAX_STRLEN];
          char default_value[GRB_MAX_STRLEN];
          mgw.dll_GRBgetstrparaminfo(env, name, current_value, default_value);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_STRING;
          param_default = default_value;
          break;
        }
        default:
          break;
      }
      flags.emplace_back("--gurobi-" + param_name, param_name, param_type, param_range,
                         param_default);
    }
    return flags;
  } catch (MiniZinc::InternalError&) {
    return {};
  }
  return {};
}

void MIPGurobiWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub,
                                 MIPWrapper::VarType* vt, string* names) {
  /// Convert var types:
  vector<char> ctype(n);
  vector<char*> pcNames(n);
  for (size_t i = 0; i < n; ++i) {
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
        throw runtime_error("  MIPWrapper: unknown variable type");
    }
  }
  _error = dll_GRBaddvars(_model, static_cast<int>(n), 0, nullptr, nullptr, nullptr, obj, lb, ub,
                          &ctype[0], &pcNames[0]);
  wrapAssert(_error == 0, "Failed to declare variables.");
  _error = dll_GRBupdatemodel(_model);
  wrapAssert(_error == 0, "Failed to update model.");
}

static char get_grb_sense(MIPWrapper::LinConType s) {
  switch (s) {
    case MIPWrapper::LQ:
      return GRB_LESS_EQUAL;
    case MIPWrapper::EQ:
      return GRB_EQUAL;
    case MIPWrapper::GQ:
      return GRB_GREATER_EQUAL;
    default:
      throw runtime_error("  MIPGurobiWrapper: unknown constraint sense");
  }
}

void MIPGurobiWrapper::addRow(int nnz, int* rmatind, double* rmatval, MIPWrapper::LinConType sense,
                              double rhs, int mask, const string& rowName) {
  //// Make sure in order to notice the indices of lazy constr:
  ++nRows;
  /// Convert var types:
  char ssense = get_grb_sense(sense);
  const char* pRName = rowName.c_str();
  _error = dll_GRBaddconstr(_model, nnz, rmatind, rmatval, ssense, rhs, pRName);
  wrapAssert(_error == 0, "Failed to add constraint.");
  int nLazyAttr = 0;
  const bool fUser = (MaskConsType_Usercut & mask) != 0;
  const bool fLazy = (MaskConsType_Lazy & mask) != 0;
  /// Gurobi 6.5.2 has lazyness 1-3.
  if (fUser) {
    if (fLazy) {
      nLazyAttr = 2;  // just active lazy
    } else {
      nLazyAttr = 3;  // even LP-active
    }
  } else if (fLazy) {
    nLazyAttr = 1;  // very lazy
  }
  if (nLazyAttr != 0) {
    nLazyIdx.push_back(nRows - 1);
    nLazyValue.push_back(nLazyAttr);
  }
}

void MIPGurobiWrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind,
                                              double* rmatval, MIPWrapper::LinConType sense,
                                              double rhs, const string& rowName) {
  wrapAssert(0 <= bVal && 1 >= bVal, "Gurobi: addIndicatorConstraint: bVal not 0/1");
  //// Make sure in order to notice the indices of lazy constr: also here?   TODO
  ++nRows;
  char ssense = get_grb_sense(sense);
  _error = dll_GRBaddgenconstrIndicator(_model, rowName.c_str(), iBVar, bVal, nnz, rmatind, rmatval,
                                        ssense, rhs);
  wrapAssert(_error == 0, "Failed to add indicator constraint.");
}

void MIPGurobiWrapper::addMinimum(int iResultVar, int nnz, int* ind, const std::string& rowName) {
  _error = dll_GRBaddgenconstrMin(_model, rowName.c_str(), iResultVar, nnz, (const int*)ind,
                                  GRB_INFINITY);
  wrapAssert(_error == 0, "Failed: GRBaddgenconstrMin.");
}

void MIPGurobiWrapper::addTimes(int x, int y, int z, const string& rowName) {
  /// As x*y - z == 0
  double zCoef = -1.0;
  double xyCoef = 1.0;
  _error =
      dll_GRBaddqconstr(_model, 1, &z, &zCoef, 1, &x, &y, &xyCoef, GRB_EQUAL, 0.0, rowName.c_str());
  /// Gurobi 9.0.1 says we cannot have GRB_EQUAL but seems to work.
  wrapAssert(_error == 0, "Failed: GRBaddqconstr.");
}

bool MIPGurobiWrapper::addSearch(const std::vector<VarId>& vars, const std::vector<int>& pri) {
  assert(vars.size() == pri.size());
  static_assert(sizeof(VarId) == sizeof(int), "VarId should be (u)int currently");
  _error = dll_GRBsetintattrlist(_model, "BranchPriority", static_cast<int>(vars.size()),
                                 (int*)vars.data(), (int*)pri.data());
  wrapAssert(_error == 0, "Failed to add branching priorities");
  return true;
}

int MIPGurobiWrapper::getFreeSearch() { return _options->nFreeSearch; }

bool MIPGurobiWrapper::addWarmStart(const std::vector<VarId>& vars,
                                    const std::vector<double>& vals) {
  assert(vars.size() == vals.size());
  static_assert(sizeof(VarId) == sizeof(int), "VarId should be (u)int currently");
  // _error = GRBsetdblattrelement(_model, "Start", 0, 1.0);
  _error = dll_GRBsetdblattrlist(_model, "Start", static_cast<int>(vars.size()), (int*)vars.data(),
                                 (double*)vals.data());
  wrapAssert(_error == 0, "Failed to add warm start");
  return true;
}

bool MIPGurobiWrapper::defineMultipleObjectives(const MultipleObjectives& mo) {
  setObjSense(1);  // Maximize
  for (int iobj = 0; iobj < mo.size(); ++iobj) {
    const auto& obj = mo.getObjectives()[iobj];
    int objvar = obj.getVariable();
    double coef = 1.0;
    _error = dll_GRBsetobjectiven(_model, iobj, static_cast<int>(mo.size()) - iobj, obj.getWeight(),
                                  0.0, 0.0, nullptr, 0.0, 1, &objvar, &coef);
    wrapAssert(_error == 0, "Failed to set objective " + std::to_string(iobj));
  }
  return true;
}

void MIPGurobiWrapper::setVarBounds(int iVar, double lb, double ub) {
  wrapAssert(lb <= ub, "mzn-gurobi: setVarBounds: lb>ub");
  _error = dll_GRBsetdblattrelement(_model, GRB_DBL_ATTR_LB, iVar, lb);
  wrapAssert(_error == 0, "mzn-gurobi: failed to set var lb.");
  _error = dll_GRBsetdblattrelement(_model, GRB_DBL_ATTR_UB, iVar, ub);
  wrapAssert(_error == 0, "mzn-gurobi: failed to set var ub.");
}

void MIPGurobiWrapper::setVarLB(int iVar, double lb) {
  _error = dll_GRBsetdblattrelement(_model, GRB_DBL_ATTR_LB, iVar, lb);
  wrapAssert(_error == 0, "mzn-gurobi: failed to set var lb.");
}

void MIPGurobiWrapper::setVarUB(int iVar, double ub) {
  _error = dll_GRBsetdblattrelement(_model, GRB_DBL_ATTR_UB, iVar, ub);
  wrapAssert(_error == 0, "mzn-gurobi: failed to set var ub.");
}

/// SolutionCallback ------------------------------------------------------------------------
/// Gurobi ensures thread-safety
static int __stdcall solcallback(GRBmodel* model, void* cbdata, int where, void* usrdata) {
  auto* info = (MIPWrapper::CBUserInfo*)usrdata;
  auto* gw = static_cast<MIPGurobiWrapper*>(info->wrapper);

  double nodecnt = 0.0;
  double actnodes = 0.0;
  double objVal = 0.0;
  int solcnt = 0;
  int newincumbent = 0;

  if (GRB_CB_MIP == where) {
    /* General MIP callback */
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIP_OBJBND, &info->pOutput->bestBound);
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIP_NODLFT, &actnodes);
    info->pOutput->nOpenNodes = static_cast<int>(actnodes);
    /// Check time after the 1st feas
    if (-1e100 != info->nTime1Feas) {
      double tNow;
      gw->dll_GRBcbget(cbdata, where, GRB_CB_RUNTIME, (void*)&tNow);
      if (tNow - info->nTime1Feas >= info->nTimeoutFeas) {
        gw->dll_GRBterminate(model);
      }
    }
  } else if (GRB_CB_MESSAGE == where) {
    /* Message callback */
    if (info->fVerb) {
      char* msg;
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MSG_STRING, &msg);
      cerr << msg << flush;
    }
  } else if (GRB_CB_MIPSOL == where) {
    /* MIP solution callback */
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_NODCNT, &nodecnt);
    info->pOutput->nNodes = static_cast<int>(nodecnt);
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_OBJ, &objVal);
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOLCNT, &solcnt);

    if (fabs(info->pOutput->objVal - objVal) > 1e-12 * (1.0 + fabs(objVal))) {
      newincumbent = 1;
      // Not confirmed yet, see lazy cuts
      //      info->pOutput->objVal = objVal;
      //      info->pOutput->status = MIPWrapper::SAT;
      //      info->pOutput->statusName = "feasible from a callback";
    }
    if (newincumbent != 0) {
      assert(info->pOutput->x);
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPSOL_SOL, (void*)info->pOutput->x);

      info->pOutput->dWallTime = std::chrono::duration<double>(std::chrono::steady_clock::now() -
                                                               info->pOutput->dWallTime0)
                                     .count();
      info->pOutput->dCPUTime = double(std::clock() - info->pOutput->cCPUTime0) / CLOCKS_PER_SEC;
    }

    /// Callback for lazy cuts
    /// Before printing
    if ((info->cutcbfn != nullptr) && ((info->cutMask & MIPWrapper::MaskConsType_Lazy) != 0)) {
      MIPWrapper::CutInput cutInput;
      cerr << "  GRB: GRB_CB_MIPSOL (" << objVal << ") -> cut callback " << endl;
      info->cutcbfn(*info->pOutput, cutInput, info->psi, true);
      for (auto& cd : cutInput) {
        //         assert( cd.mask & MIPWrapper::MaskConsType_Lazy );
        if ((cd.mask & MIPWrapper::MaskConsType_Lazy) != 0) {  // take only lazy constr generators
          int _error =
              gw->dll_GRBcblazy(cbdata, static_cast<int>(cd.rmatind.size()), cd.rmatind.data(),
                                cd.rmatval.data(), get_grb_sense(cd.sense), cd.rhs);
          if (_error != 0) {
            cerr << "  GRB_wrapper: failed to add lazy cut. " << endl;
          } else {
            newincumbent = -1;
          }
          //             info->pOutput->objVal = 1e100;  // to mark that we can get a new incumbent
          //             which should be printed
        }
      }
    }
    if (solcnt >= 0 /*This is solution number for Gurobi*/ && newincumbent >= 0) {
      if (fabs(info->pOutput->objVal - objVal) > 1e-12 * (1.0 + fabs(objVal))) {
        newincumbent = 1;
        info->pOutput->objVal = objVal;
        info->pOutput->status = MIPWrapper::SAT;
        info->pOutput->statusName = "feasible from a callback";
      }
    }
    if (newincumbent > 0) {
      info->pOutput->dCPUTime = double(std::clock() - info->pOutput->cCPUTime0) / CLOCKS_PER_SEC;

      /// Set time for the 1st feas
      if (0 <= info->nTimeoutFeas && -1e100 == info->nTime1Feas) {
        gw->dll_GRBcbget(cbdata, where, GRB_CB_RUNTIME, (void*)&info->nTime1Feas);
      }

      /// Call the user function:
      if (info->solcbfn != nullptr) {
        (*info->solcbfn)(*info->pOutput, info->psi);
      }

      if (0 == info->nTimeoutFeas) {
        gw->dll_GRBterminate(model);  // Straight after feas
      }
    }
  } else if (GRB_CB_MIPNODE == where) {
    int status;
    gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_STATUS, &status);
    if (status == GRB_OPTIMAL && (info->cutcbfn != nullptr)) {  // if cut handler given
      MIPWrapper::Output outpRlx;
      outpRlx.x = info->pOutput->x;  // using the sol output storage  TODO?
      outpRlx.nCols = info->pOutput->nCols;
      assert(outpRlx.x && outpRlx.nCols);
      //       dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_RELOBJ, outpRlx.objVal);
      gw->dll_GRBcbget(cbdata, where, GRB_CB_MIPNODE_REL, (void*)outpRlx.x);
      //       cerr << "  GRB: GRB_CB_MIPNODE -> cut callback " << endl;
      MIPWrapper::CutInput cutInput;
      info->cutcbfn(outpRlx, cutInput, info->psi, false);
      //       static int nCuts=0;
      //       nCuts += cutInput.size();
      //       if ( cutInput.size() )
      //         cerr << "\n   N CUTS:  " << nCuts << endl;
      for (auto& cd : cutInput) {
        if ((cd.mask & (MIPWrapper::MaskConsType_Usercut | MIPWrapper::MaskConsType_Lazy)) == 0) {
          throw runtime_error("Cut callback: should be user/lazy");
        }
        if ((cd.mask & MIPWrapper::MaskConsType_Usercut) != 0) {
          int _error =
              gw->dll_GRBcbcut(cbdata, static_cast<int>(cd.rmatind.size()), cd.rmatind.data(),
                               cd.rmatval.data(), get_grb_sense(cd.sense), cd.rhs);
          if (_error != 0) {
            cerr << "  GRB_wrapper: failed to add user cut. " << endl;
          }
        }
        if ((cd.mask & MIPWrapper::MaskConsType_Lazy) != 0) {
          int _error =
              gw->dll_GRBcblazy(cbdata, static_cast<int>(cd.rmatind.size()), cd.rmatind.data(),
                                cd.rmatval.data(), get_grb_sense(cd.sense), cd.rhs);
          if (_error != 0) {
            cerr << "  GRB_wrapper: failed to add lazy cut. " << endl;
          }
        }
      }
    }
  }
  return 0;
} /* END logcallback */
// end SolutionCallback ---------------------------------------------------------------------

MIPGurobiWrapper::Status MIPGurobiWrapper::convertStatus(int gurobiStatus) {
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
    int solcount = 0;
    _error = dll_GRBgetintattr(_model, "SolCount", &solcount);
    wrapAssert(_error == 0, "  Failure to access solution count.", false);
    if (solcount != 0) {
      s = Status::SAT;
    }
    oss << "Gurobi stopped with status " << gurobiStatus;
  }
  output.statusName = _gurobiStatusBuffer = oss.str();
  return s;
}

void MIPGurobiWrapper::solve() {        // Move into ancestor?
  _error = dll_GRBupdatemodel(_model);  // for model export
  wrapAssert(_error == 0, "Failed to update model.");

  /// ADDING LAZY CONSTRAINTS IF ANY
  if (!nLazyIdx.empty()) {
    assert(nLazyIdx.size() == nLazyValue.size());
    if (fVerbose) {
      cerr << "  MIPGurobiWrapper: marking " << nLazyIdx.size() << " lazy cuts." << endl;
    }
    _error = dll_GRBsetintattrlist(_model, "Lazy", static_cast<int>(nLazyIdx.size()),
                                   nLazyIdx.data(), nLazyValue.data());
    wrapAssert(_error == 0, "Failed to set constraint attribute.");
    nLazyIdx.clear();
    nLazyValue.clear();
    _error = dll_GRBupdatemodel(_model);  // for model export
    wrapAssert(_error == 0, "Failed to update model after modifying some constraint attr.");
  }

  /////////////// Last-minute solver options //////////////////
  /* Turn on output to file */
  _error = dll_GRBsetstrparam(dll_GRBgetenv(_model), "LogFile",
                              "");  // FAILS to switch off in Ubuntu 15.04
                                    /* Turn on output to the screen */
  _error = dll_GRBsetintparam(dll_GRBgetenv(_model), "OutputFlag",
                              /*fVerbose ? 1 :*/ 0);  // switch off, redirect in callback
  //    _error = dll_GRBsetintparam(dll_GRBgetenv(_model), "LogToConsole",
  //                             fVerbose ? 1 : 0);  // also when flagIntermediate?  TODO
  wrapAssert(_error == 0, "  GUROBI Warning: Failure to switch screen indicator.", false);
  //    _error =  dll_GRB_setintparam (_env, GRB_PARAM_ClockType, 1);            // CPU time
  //    _error =  dll_GRB_setintparam (_env, GRB_PARAM_MIP_Strategy_CallbackReducedLP, GRB__OFF); //
  //    Access original model
  if (!_options->sExportModel.empty()) {
    _error = dll_GRBwrite(_model, _options->sExportModel.c_str());
    wrapAssert(_error == 0, "Failed to write LP to disk.", false);
  }

  /// TODO
  //     if(all_solutions && obj.getImpl()) {
  //       IloNum lastObjVal = (obj.getSense() == IloObjective::Minimize ) ?
  //       _ilogurobi->use(SolutionCallback(_iloenv, lastObjVal, *this));
  // Turn off GUROBI logging

  if (_options->nThreads > 0) {
    _error = dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_THREADS, _options->nThreads);
    //      int nn;    // THE SETTING FAILS TO WORK IN 6.0.5.
    //      _error = dll_getintparam(_env, GRB_INT_PAR_THREADS, &nn);
    //      cerr << "Set " << nThreads << " threads, reported " << nn << endl;
    wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_THREADS.", false);
  }

  if (_options->nTimeout1000 > 0) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), GRB_DBL_PAR_TIMELIMIT,
                                static_cast<double>(_options->nTimeout1000) / 1000.0);
    wrapAssert(_error == 0, "Failed to set GRB_PARAM_TimeLimit.", false);
  }

  if (_options->nSolLimit > 0) {
    _error =
        dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_SOLUTIONLIMIT, _options->nSolLimit);
    wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_SOLLIMIT.", false);
  }

  if (_options->nSeed >= 0) {
    _error = dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_SEED, _options->nSeed);
    wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_SEED.", false);
  }

  if (_options->nWorkMemLimit > 0 && _options->nWorkMemLimit < 1e200) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), "NodefileStart", _options->nWorkMemLimit);
    wrapAssert(_error == 0, "Failed to set NodefileStart.", false);
  }

  if (!_options->sNodefileDir.empty()) {
    _error =
        dll_GRBsetstrparam(dll_GRBgetenv(_model), "NodefileDir", _options->sNodefileDir.c_str());
    wrapAssert(_error == 0, "Failed to set NodefileDir.", false);
  }

  if (_options->absGap >= 0.0) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), "MIPGapAbs", _options->absGap);
    wrapAssert(_error == 0, "Failed to set  MIPGapAbs.", false);
  }
  if (_options->nMIPFocus > 0) {
    _error = dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_MIPFOCUS, _options->nMIPFocus);
    wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_MIPFOCUS.", false);
  }

  if (_options->relGap >= 0.0) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), "MIPGap", _options->relGap);
    wrapAssert(_error == 0, "Failed to set  MIPGap.", false);
  }
  if (_options->intTol >= 0.0) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), "IntFeasTol", _options->intTol);
    wrapAssert(_error == 0, "Failed to set   IntFeasTol.", false);
  }
  if (_options->feasTol >= 0.0) {
    _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), "FeasibilityTol", _options->feasTol);
    wrapAssert(_error == 0, "Failed to set   FeasTol.", false);
  }
  if (_options->nonConvex >= 0) {
#ifdef GRB_INT_PAR_NONCONVEX
    int major;
    int minor;
    int technical;
    dll_GRBversion(&major, &minor, &technical);
    if (major >= 9) {
      _error =
          dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_NONCONVEX, _options->nonConvex);
      wrapAssert(_error == 0, "Failed to set   " GRB_INT_PAR_NONCONVEX, false);
    } else {
      std::cerr << "WARNING: Non-convex solving is unavailable in this version of Gurobi"
                << std::endl;
    }
#else
    std::cerr << "WARNING: Non-convex solving is unavailable in this version of Gurobi"
              << std::endl;
#endif
  }

  /// Solution callback
  output.nCols = static_cast<int>(colObj.size());
  _x.resize(output.nCols);
  output.x = &_x[0];
  SolCallbackFn solcbfn = cbui.solcbfn;
  if (true) {  // NOLINT: Need for logging
    cbui.fVerb = fVerbose;
    cbui.nTimeoutFeas = _options->nTimeoutFeas1000 / 1000.0;
    if (!_options->flagIntermediate) {
      cbui.solcbfn = nullptr;
    }
    if (cbui.cutcbfn != nullptr) {
      assert(cbui.cutMask & (MaskConsType_Usercut | MaskConsType_Lazy));
      if ((cbui.cutMask & MaskConsType_Usercut) != 0) {
        // For user cuts, needs to keep some info after presolve
        if (fVerbose) {
          cerr << "  MIPGurobiWrapper: user cut callback enabled, setting PreCrush=1" << endl;
        }
        _error = dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_PRECRUSH, 1);
        wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_PRECRUSH.", false);
      }
      if ((cbui.cutMask & MaskConsType_Lazy) != 0) {
        // For lazy cuts, Gurobi disables some presolves
        if (fVerbose) {
          cerr << "  MIPGurobiWrapper: lazy cut callback enabled, setting LazyConstraints=1"
               << endl;
        }
        _error = dll_GRBsetintparam(dll_GRBgetenv(_model), GRB_INT_PAR_LAZYCONSTRAINTS, 1);
        wrapAssert(_error == 0, "Failed to set GRB_INT_PAR_LAZYCONSTRAINTS.", false);
      }
    }
    _error = dll_GRBsetcallbackfunc(_model, solcallback, (void*)&cbui);
    wrapAssert(_error == 0, "Failed to set callback", false);
  }

  // Process extra flags options
  for (auto& it : _options->extraParams) {
    auto name = it.first.substr(9);
    int type = dll_GRBgetparamtype(dll_GRBgetenv(_model), name.c_str());
    enum GurobiParamType { T_INT = 1, T_DOUBLE = 2, T_STRING = 3 };
    switch (type) {
      case T_INT:
        _error = dll_GRBsetintparam(dll_GRBgetenv(_model), name.c_str(), stoi(it.second));
        break;
      case T_DOUBLE:
        _error = dll_GRBsetdblparam(dll_GRBgetenv(_model), name.c_str(), stod(it.second));
        break;
      case T_STRING:
        _error = dll_GRBsetstrparam(dll_GRBgetenv(_model), name.c_str(), it.second.c_str());
        break;
      default:
        wrapAssert(false, "Could not determine type of parameter " + name, false);
        break;
    }
    wrapAssert(_error == 0, "Failed to set parameter " + name + " = " + it.second, false);
  }

  /// after all modifs
  if (!_options->sReadParams.empty()) {
    _error = dll_GRBreadparams(dll_GRBgetenv(_model), _options->sReadParams.c_str());
    wrapAssert(_error == 0, "Failed to read GUROBI parameters.", false);
  }

  if (!_options->sWriteParams.empty()) {
    _error = dll_GRBwriteparams(dll_GRBgetenv(_model), _options->sWriteParams.c_str());
    wrapAssert(_error == 0, "Failed to write GUROBI parameters.", false);
  }

  /* See if we should set up concurrent solving */
  if (!_options->sConcurrentParamFiles.empty()) {
    int iSetting = -1;
    for (const auto& paramFile : _options->sConcurrentParamFiles) {
      ++iSetting;
      auto* env_i = dll_GRBgetconcurrentenv(_model, iSetting);
      _error = dll_GRBreadparams(env_i, paramFile.c_str());
      wrapAssert(_error == 0, "Failed to read GUROBI parameters from file " + paramFile, false);
    }
  }

  cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
  output.dCPUTime = cbui.pOutput->cCPUTime0 = std::clock();

  /* Optimize the problem and obtain solution. */
  _error = dll_GRBoptimize(_model);
  wrapAssert(_error == 0, "Failed to optimize MIP.");

  output.dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output.dWallTime0).count();
  output.dCPUTime = (std::clock() - output.dCPUTime) / CLOCKS_PER_SEC;

  int solstat;
  _error = dll_GRBgetintattr(_model, GRB_INT_ATTR_STATUS, &solstat);
  wrapAssert(_error == 0, "Failed to get MIP status.", false);
  output.status = convertStatus(solstat);

  /// Continuing to fill the output object:
  if (Status::OPT == output.status || Status::SAT == output.status) {
    _error = dll_GRBgetdblattr(_model, GRB_DBL_ATTR_OBJVAL, &output.objVal);
    wrapAssert(_error == 0, "No MIP objective value available.");

    //    int cur_numrows = dll_GRB_getnumrows (env, lp);
    int cur_numcols = getNCols();
    assert(cur_numcols == colObj.size());

    _x.resize(cur_numcols);
    output.x = &_x[0];
    _error = dll_GRBgetdblattrarray(_model, GRB_DBL_ATTR_X, 0, cur_numcols, (double*)output.x);
    wrapAssert(_error == 0, "Failed to get variable values.");
    if (!_options->flagIntermediate && (solcbfn != nullptr)) {
      solcbfn(output, cbui.psi);
    }
  }
  output.bestBound = std::numeric_limits<double>::has_quiet_NaN
                         ? std::numeric_limits<double>::quiet_NaN()
                         : std::numeric_limits<double>::max();
  int nObj = 0;
  dll_GRBgetintattr(_model, GRB_INT_ATTR_NUMOBJ, &nObj);
  if (1 >= nObj) {
    _error = dll_GRBgetdblattr(_model, GRB_DBL_ATTR_OBJBOUNDC, &output.bestBound);
    wrapAssert(_error == 0, "Failed to get the best bound.", false);
  }
  double nNodes = -1;
  _error = dll_GRBgetdblattr(_model, GRB_DBL_ATTR_NODECOUNT, &nNodes);
  output.nNodes = static_cast<int>(nNodes);
  output.nOpenNodes = 0;
}

void MIPGurobiWrapper::setObjSense(int s) {
  _error = dll_GRBsetintattr(_model, GRB_INT_ATTR_MODELSENSE, s > 0 ? GRB_MAXIMIZE : GRB_MINIMIZE);
  wrapAssert(_error == 0, "Failed to set obj sense.");
}
