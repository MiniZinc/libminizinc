/*
 *  main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* this source code form is subject to the terms of the mozilla public
 * license, v. 2.0. if a copy of the mpl was not distributed with this
 * file, you can obtain one at http://mozilla.org/mpl/2.0/. */

#include "minizinc/solvers/MIP/MIP_xpress_wrap.hh"

#include "minizinc/config.hh"
#include "minizinc/exception.hh"
#include "minizinc/file_utils.hh"
#include "minizinc/utils.hh"

#include <cmath>
#include <cstring>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>

struct UserSolutionCallbackData {
  MIPWrapper::CBUserInfo* info;
  XPRBprob* problem;
  vector<XPRBvar>* variables;
  XpressPlugin* plugin;
};

class XpressException : public runtime_error {
public:
  XpressException(const string& msg) : runtime_error(" MIPxpressWrapper: " + msg) {}
};

XpressPlugin::XpressPlugin() : Plugin(XpressPlugin::dlls()) { loadDll(); }

XpressPlugin::XpressPlugin(const std::string& dll_file) : Plugin(dll_file) { loadDll(); }

void XpressPlugin::loadDll() {
  load_symbol(XPRSinit);
  load_symbol(XPRSfree);
  load_symbol(XPRSgetversion);
  load_symbol(XPRSgetlicerrmsg);
  load_symbol(XPRBgetXPRSprob);
  load_symbol(XPRBsetmsglevel);
  load_symbol(XPRSsetlogfile);
  load_symbol(XPRSsetintcontrol);
  load_symbol(XPRSsetdblcontrol);
  load_symbol(XPRBgetsol);
  load_symbol(XPRSgetintattrib);
  load_symbol(XPRSgetdblattrib);
  load_symbol(XPRBbegincb);
  load_symbol(XPRBsync);
  load_symbol(XPRBendcb);
  load_symbol(XPRBsetterm);
  load_symbol(XPRBnewvar);
  load_symbol(XPRBnewctr);
  load_symbol(XPRBsetctrtype);
  load_symbol(XPRBexportprob);
  load_symbol(XPRBgetbounds);
  load_symbol(XPRBsetobj);
  load_symbol(XPRBmipoptimize);
  load_symbol(XPRBsetsense);
  load_symbol(XPRSsetcbintsol);
  load_symbol(XPRBsetub);
  load_symbol(XPRBsetlb);
  load_symbol(XPRBsetindicator);
  load_symbol(XPRBnewsol);
  load_symbol(XPRBsetsolvar);
  load_symbol(XPRBaddmipsol);
  load_symbol(XPRBnewprob);
  load_symbol(XPRBdelprob);
  load_symbol(XPRSgetcontrolinfo);
  load_symbol(XPRSgetintcontrol);
  load_symbol(XPRSgetintcontrol64);
  load_symbol(XPRSgetdblcontrol);
  load_symbol(XPRSgetstrcontrol);
  load_symbol(XPRSsetintcontrol64);
  load_symbol(XPRSgetstringcontrol);
  load_symbol(XPRSsetstrcontrol);
}

const std::vector<std::string>& XpressPlugin::dlls() {
  static std::vector<std::string> ret = {
#ifdef _WIN32
      "xprs", "C:\\xpressmp\\bin\\xprs.dll"
#elif __APPLE__
      "libxprs", "/Applications/FICO Xpress/xpressmp/lib/libxprs.dylib"
#else
      "libxprs", "/opt/xpressmp/lib/libxprs.so"
#endif
  };
  return ret;
}

void MIPxpressWrapper::openXpress() {
  checkDLL();
  _problem = _plugin->XPRBnewprob(nullptr);
  _xpressObj = _plugin->XPRBnewctr(_problem, nullptr, XB_N);
}

void MIPxpressWrapper::closeXpress() {
  _plugin->XPRBdelprob(_problem);
  _plugin->XPRSfree();
  delete _plugin;
}

void MIPxpressWrapper::checkDLL() {
  if (!_factoryOptions.xpressDll.empty()) {
    _plugin = new XpressPlugin(_factoryOptions.xpressDll);
  } else {
    _plugin = new XpressPlugin();
  }

  std::vector<std::string> paths;
  if (!_factoryOptions.xprsPassword.empty()) {
    paths.push_back(_factoryOptions.xprsPassword);
  } else {
    paths.emplace_back("");  // Try builtin xpress dirs
    auto dir = MiniZinc::FileUtils::dir_name(_plugin->path());
    auto file = dir + "/../bin/xpauth.xpr";
    if (!dir.empty() && MiniZinc::FileUtils::file_exists(file)) {
      paths.push_back(file);  // Try the bin dir license file if it exists
    }
  }

  for (const auto& path : paths) {
    int ret = _plugin->XPRSinit(path.empty() ? nullptr : path.c_str());
    if (ret == 0) {
      return;
    }
    // Return code of 32 means student licence, but otherwise it's an error
    if (ret == 32) {
      if (_options->verbose) {
        char message[512];
        _plugin->XPRSgetlicerrmsg(message, 512);
        std::cerr << message << std::endl;
      }
      return;
    }
  }

  char message[512];
  _plugin->XPRSgetlicerrmsg(message, 512);
  throw XpressException(message);
}

string MIPxpressWrapper::getDescription(FactoryOptions& factoryOpt,
                                        MiniZinc::SolverInstanceBase::Options* opt) {
  ostringstream oss;
  oss << "  MIP wrapper for FICO Xpress Optimiser version " << getVersion(factoryOpt, opt);
  oss << ".  Compiled  " __DATE__ "  " __TIME__;
  return oss.str();
}

string MIPxpressWrapper::getVersion(FactoryOptions& factoryOpt,
                                    MiniZinc::SolverInstanceBase::Options* opt) {
  try {
    auto* p =
        factoryOpt.xpressDll.empty() ? new XpressPlugin : new XpressPlugin(factoryOpt.xpressDll);
    char v[16];
    p->XPRSgetversion(v);
    delete p;
    return v;
  } catch (MiniZinc::Plugin::PluginError&) {
    return "<unknown version>";
  }
}

vector<string> MIPxpressWrapper::getRequiredFlags(FactoryOptions& factoryOpt) {
  Options opts;
  FactoryOptions triedFactoryOpts;
  vector<string> ret;
  // TODO: This is more complex than it should be
  // We only know if --xpress-password is required if we have the DLL available
  // So we have to try the user supplied --xpress-dll if given
  while (true) {
    try {
      // Try opening without considering factory options
      MIPxpressWrapper w(triedFactoryOpts, &opts);
      return ret;
    } catch (MiniZinc::Plugin::PluginError&) {
      ret.emplace_back("--xpress-dll");  // The DLL needs to be given
      if (triedFactoryOpts.xpressDll == factoryOpt.xpressDll) {
        return ret;
      }
      triedFactoryOpts.xpressDll = factoryOpt.xpressDll;
    } catch (XpressException&) {
      ret.emplace_back("--xpress-password");  // The license needs to be given
      if (triedFactoryOpts.xprsPassword == factoryOpt.xprsPassword) {
        return ret;
      }
      triedFactoryOpts.xprsPassword = factoryOpt.xprsPassword;
    }
  }
}

vector<string> MIPxpressWrapper::getFactoryFlags() {
  return {"--xpress-dll", "--xpress-password"};
};

string MIPxpressWrapper::getId() { return "xpress"; }

string MIPxpressWrapper::getName() { return "Xpress"; }

vector<string> MIPxpressWrapper::getTags() { return {"mip", "float", "api"}; }

vector<string> MIPxpressWrapper::getStdFlags() { return {"-i", "-s", "-p", "-r"}; }

vector<MiniZinc::SolverConfig::ExtraFlag> MIPxpressWrapper::getExtraFlags(
    FactoryOptions& factoryOpt) {
  try {
    Options opts;
    MIPxpressWrapper p(factoryOpt, &opts);

    auto* prb = p._plugin->XPRBgetXPRSprob(p._problem);
    // Using string parameter names because there doesn't seem to be a way to recover
    // the name from a parameter ID number
    static std::vector<std::string> all_params = {
        "algaftercrossover", "algafternetwork", "autoperturb", "backtrack", "backtracktie",
        "baralg", "barcrash", "bardualstop", "barfreescale", "bargapstop", "bargaptarget",
        "barindeflimit", "bariterlimit", "barkernel", "barobjscale", "barorder", "barorderthreads",
        "baroutput", "barpresolveops", "barprimalstop", "barregularize", "barrhsscale",
        "barsolution", "barstart", "barstartweight", "barstepstop", "barthreads", "barcores",
        "bigm", "bigmmethod", "branchchoice", "branchdisj", "branchstructural", "breadthfirst",
        "cachesize", "callbackfrommasterthread", "choleskyalg", "choleskytol", "conflictcuts",
        "concurrentthreads", "corespercpu", "covercuts", "cpuplatform", "cputime", "crash",
        "crossover", "crossoveraccuracytol", "crossoveriterlimit", "crossoverops",
        "crossoverthreads", "cstyle", "cutdepth", "cutfactor", "cutfreq", "cutstrategy",
        "cutselect", "defaultalg", "densecollimit", "deterministic", "dualgradient", "dualize",
        "dualizeops", "dualperturb", "dualstrategy", "dualthreads", "eigenvaluetol", "elimfillin",
        "elimtol", "etatol", "extracols", "extraelems", "extramipents", "extrapresolve",
        "extraqcelements", "extraqcrows", "extrarows", "extrasetelems", "extrasets",
        "feasibilitypump", "feastol", "feastoltarget", "forceoutput", "forceparalleldual",
        "globalfilebias", "globalfileloginterval", "gomcuts", "heurbeforelp", "heurdepth",
        "heurdiveiterlimit", "heurdiverandomize", "heurdivesoftrounding", "heurdivespeedup",
        "heurdivestrategy", "heurforcespecialobj", "heurfreq", "heurmaxsol", "heurnodes",
        "heursearcheffort", "heursearchfreq", "heursearchrootcutfreq", "heursearchrootselect",
        "heursearchtreeselect", "heurstrategy", "heurthreads", "historycosts", "ifcheckconvexity",
        "indlinbigm", "indprelinbigm", "invertfreq", "invertmin", "keepbasis", "keepnrows",
        "l1cache", "linelength", "lnpbest", "lnpiterlimit", "lpflags", "lpiterlimit",
        "lprefineiterlimit", "localchoice", "lpfolding", "lplog", "lplogdelay", "lplogstyle",
        "lpthreads", "markowitztol", "matrixtol", "maxchecksonmaxcuttime", "maxchecksonmaxtime",
        "maxmcoeffbufferelems", "maxcuttime", "maxglobalfilesize", "maxiis", "maximpliedbound",
        "maxlocalbacktrack", "maxmemoryhard", "maxmemorysoft", "maxmiptasks", "maxmipsol",
        "maxnode", "maxpagelines", "maxscalefactor", "maxtime", "mipabscutoff", "mipabsgapnotify",
        "mipabsgapnotifybound", "mipabsgapnotifyobj", "mipabsstop", "mipaddcutoff",
        "mipdualreductions", "mipfracreduce", "mipkappafreq", "miplog", "mippresolve", "miprampup",
        "miqcpalg", "miprefineiterlimit", "miprelcutoff", "miprelgapnotify", "miprelstop",
        "mipterminationmethod", "mipthreads", "miptol", "miptoltarget", "mps18compatible",
        "mpsboundname", "mpsecho", "mpsformat", "mpsobjname", "mpsrangename", "mpsrhsname",
        "mutexcallbacks", "netcuts", "nodeselection", "objscalefactor", "optimalitytol",
        "optimalitytoltarget", "outputlog", "outputmask", "outputtol", "penalty", "perturb",
        "pivottol", "ppfactor", "preanalyticcenter", "prebasisred", "prebndredcone",
        "prebndredquad", "precoefelim", "precomponents", "precomponentseffort", "preconedecomp",
        "preconvertseparable", "predomcol", "predomrow", "preduprow", "preelimquad",
        "preimplications", "prelindep", "preobjcutdetect", "prepermute", "prepermuteseed",
        "preprobing", "preprotectdual", "presolve", "presolvemaxgrow", "presolveops",
        "presolvepasses", "presort", "pricingalg", "primalops", "primalperturb", "primalunshift",
        "pseudocost", "qccuts", "qcrootalg", "qsimplexops", "quadraticunshift",
        //"randomseed",
        "refactor", "refineops", "relaxtreememorylimit", "relpivottol", "repairindefiniteq",
        "repairinfeasmaxtime", "resourcestrategy", "rootpresolve", "sbbest", "sbeffort",
        "sbestimate", "sbiterlimit", "sbselect", "scaling", "sifting", "sleeponthreadwait",
        "sosreftol", "symmetry", "symselect",
        //"threads",
        "trace", "treecompression", "treecovercuts", "treecutselect", "treediagnostics",
        "treegomcuts", "treememorylimit", "treememorysavingtarget", "treepresolve",
        "treepresolve_keepbasis", "treeqccuts", "tunerhistory", "tunermaxtime", "tunermethod",
        "tunermethodfile", "tunermode", "tuneroutput", "tuneroutputpath", "tunerpermute",
        "tunerrootalg", "tunersessionname", "tunertarget", "tunerthreads", "usersolheuristic",
        "varselection",
        //"version"
    };
    std::vector<MiniZinc::SolverConfig::ExtraFlag> res;
    for (auto param : all_params) {
      int n;
      int t;
      p._plugin->XPRSgetcontrolinfo(prb, param.c_str(), &n, &t);
      MiniZinc::SolverConfig::ExtraFlag::FlagType param_type;
      std::string param_default;
      switch (t) {
        case XPRS_TYPE_INT: {
          int d;
          p._plugin->XPRSgetintcontrol(prb, n, &d);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
          param_default = to_string(d);
          break;
        }
        case XPRS_TYPE_INT64: {
          XPRSint64 d;
          p._plugin->XPRSgetintcontrol64(prb, n, &d);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_INT;
          param_default = to_string(d);
          break;
        }
        case XPRS_TYPE_DOUBLE: {
          double d;
          p._plugin->XPRSgetdblcontrol(prb, n, &d);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_FLOAT;
          param_default = to_string(d);
          break;
        }
        case XPRS_TYPE_STRING: {
          int l;
          p._plugin->XPRSgetstringcontrol(prb, n, nullptr, 0, &l);
          char* d = (char*)malloc(l);
          p._plugin->XPRSgetstringcontrol(prb, n, d, l, &l);
          param_type = MiniZinc::SolverConfig::ExtraFlag::FlagType::T_STRING;
          param_default = d;
          break;
        }
        default:
          continue;
      }
      // TODO: Some of these parameters have min/max or are categorical, but there's no way
      // to programatically get the possible values. We could manually maintain it, but it's
      // probably not worth doing right now.
      std::vector<std::string> param_range;  // unused for now
      res.emplace_back("--xpress-" + param, param, param_type, param_range, param_default);
    }
    return res;
  } catch (MiniZinc::Plugin::PluginError&) {
    return {};
  } catch (XpressException&) {
    return {};
  }
  return {};
}

void MIPxpressWrapper::Options::printHelp(ostream& os) {
  os << "XPRESS MIP wrapper options:" << std::endl
     << "--msgLevel <n>       print solver output, default: 0" << std::endl
     << "--logFile <file>     log file" << std::endl
     << "--solver-time-limit <N>        stop search after N milliseconds, if negative, it "
        "will only stop if at least one solution was found"
     << std::endl
     << "-n <N>, --numSolutions <N>   stop search after N solutions" << std::endl
     << "--writeModel <file>  write model to <file>" << std::endl
     << "--writeModelFormat [lp|mps] the file format of the written model(lp "
        "or mps), default: lp"
     << std::endl
     << "--absGap <d>         absolute gap |primal-dual| to stop, default: " << 0 << std::endl
     << "--relGap <d>         relative gap |primal-dual|/<solver-dep> to stop, "
        "default: "
     << 0.0001 << std::endl
     << "-i                   print intermediate solution, default: false" << std::endl
     << "-r <N>, --seed <N>, --random-seed <N>" << std::endl
     << "    random seed, integer"
     << "-p <N>, --parallel <N>   use N threads" << std::endl
     << "--xpress-dll <file>      Xpress DLL file (xprs.dll/libxprs.so/libxprs.dylib)" << std::endl
     << "--xpress-password <dir>  directory where xpauth.xpr is located (optional)" << std::endl
     << std::endl;
}

bool MIPxpressWrapper::FactoryOptions::processOption(int& i, std::vector<std::string>& argv,
                                                     const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  if (cop.get("--xpress-dll", &xpressDll)) {                 // NOLINT: Allow repeated empty if
  } else if (cop.get("--xpress-password", &xprsPassword)) {  // NOLINT: Allow repeated empty if
  } else {
    return false;
  }
  return true;
}

bool MIPxpressWrapper::Options::processOption(int& i, std::vector<std::string>& argv,
                                              const std::string& workingDir) {
  MiniZinc::CLOParser cop(i, argv);
  std::string buffer;
  if (cop.get("--msgLevel", &msgLevel)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--logFile", &buffer)) {
    logFile = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("--solver-time-limit", &timeout)) {     // NOLINT: Allow repeated empty if
  } else if (cop.get("-n --numSolutions", &numSolutions)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--writeModel", &buffer)) {
    writeModelFile = MiniZinc::FileUtils::file_path(buffer, workingDir);
  } else if (cop.get("--writeModelFormat", &writeModelFormat)) {  // NOLINT: Allow repeated empty if
  } else if (cop.get("--relGap", &relGap)) {                      // NOLINT: Allow repeated empty if
  } else if (cop.get("--absGap", &absGap)) {                      // NOLINT: Allow repeated empty if
  } else if (cop.get("-i")) {
    intermediateSolutions = true;
  } else if (cop.get("-p --parallel", &numThreads)) {
  } else if (cop.get("-r --seed --random-seed", &randomSeed)) {
  } else {
    return false;
  }
  return true;
}

void MIPxpressWrapper::setOptions() {
  XPRSprob xprsProblem = _plugin->XPRBgetXPRSprob(_problem);

  _plugin->XPRBsetmsglevel(_problem, _options->msgLevel);

  _plugin->XPRSsetlogfile(xprsProblem, _options->logFile.c_str());
  if (_options->timeout > 1000 || _options->timeout < -1000) {
    _plugin->XPRSsetintcontrol(xprsProblem, XPRS_MAXTIME,
                               static_cast<int>(_options->timeout / 1000));
  }
  _plugin->XPRSsetintcontrol(xprsProblem, XPRS_MAXMIPSOL, _options->numSolutions);
  _plugin->XPRSsetdblcontrol(xprsProblem, XPRS_MIPABSSTOP, _options->absGap);
  _plugin->XPRSsetdblcontrol(xprsProblem, XPRS_MIPRELSTOP, _options->relGap);

  if (_options->numThreads > 0) {
    _plugin->XPRSsetintcontrol(xprsProblem, XPRS_THREADS, _options->numThreads);
  }

  if (_options->randomSeed != 0) {
    _plugin->XPRSsetintcontrol(xprsProblem, XPRS_RANDOMSEED, _options->randomSeed);
  }

  for (auto& it : _options->extraParams) {
    auto name = it.first.substr(9);
    int n;
    int t;
    _plugin->XPRSgetcontrolinfo(xprsProblem, name.c_str(), &n, &t);
    switch (t) {
      case XPRS_TYPE_INT:
        _plugin->XPRSsetintcontrol(xprsProblem, n, stoi(it.second));
        break;
      case XPRS_TYPE_INT64:
        _plugin->XPRSsetintcontrol64(xprsProblem, n, stoll(it.second));
        break;
      case XPRS_TYPE_DOUBLE:
        _plugin->XPRSsetdblcontrol(xprsProblem, n, stod(it.second));
        break;
      case XPRS_TYPE_STRING:
        _plugin->XPRSsetstrcontrol(xprsProblem, n, it.second.c_str());
        break;
      default:
        throw XpressException("Unknown type for parameter " + name);
    }
  }
}

static MIPWrapper::Status convert_status(int xpressStatus) {
  switch (xpressStatus) {
    case XPRB_MIP_OPTIMAL:
      return MIPWrapper::Status::OPT;
    case XPRB_MIP_INFEAS:
      return MIPWrapper::Status::UNSAT;
    case XPRB_MIP_UNBOUNDED:
      return MIPWrapper::Status::UNBND;
    case XPRB_MIP_NO_SOL_FOUND:
      return MIPWrapper::Status::UNKNOWN;
    case XPRB_MIP_NOT_LOADED:
      return MIPWrapper::Status::ERROR_STATUS;
    default:
      return MIPWrapper::Status::UNKNOWN;
  }
}

static string get_status_name(int xpressStatus) {
  string rt = "Xpress stopped with status: ";
  switch (xpressStatus) {
    case XPRB_MIP_OPTIMAL:
      return rt + "Optimal";
    case XPRB_MIP_INFEAS:
      return rt + "Infeasible";
    case XPRB_MIP_UNBOUNDED:
      return rt + "Unbounded";
    case XPRB_MIP_NO_SOL_FOUND:
      return rt + "No solution found";
    case XPRB_MIP_NOT_LOADED:
      return rt + "No problem loaded or error";
    default:
      return rt + "Unknown status";
  }
}

static void set_output_variables(XpressPlugin* plugin, MIPxpressWrapper::Output* output,
                                 vector<XPRBvar>* variables) {
  size_t nCols = variables->size();
  auto* x = (double*)malloc(nCols * sizeof(double));
  for (size_t ii = 0; ii < nCols; ii++) {
    x[ii] = plugin->XPRBgetsol((*variables)[ii]);
  }
  output->x = x;
}

static void set_output_attributes(XpressPlugin* plugin, MIPxpressWrapper::Output* output,
                                  XPRSprob xprsProblem) {
  int xpressStatus = 0;
  plugin->XPRSgetintattrib(xprsProblem, XPRS_MIPSTATUS, &xpressStatus);
  output->status = convert_status(xpressStatus);
  output->statusName = get_status_name(xpressStatus);

  plugin->XPRSgetdblattrib(xprsProblem, XPRS_MIPOBJVAL, &output->objVal);
  plugin->XPRSgetdblattrib(xprsProblem, XPRS_BESTBOUND, &output->bestBound);

  plugin->XPRSgetintattrib(xprsProblem, XPRS_NODES, &output->nNodes);
  plugin->XPRSgetintattrib(xprsProblem, XPRS_ACTIVENODES, &output->nOpenNodes);

  output->dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output->dWallTime0).count();
  output->dCPUTime = double(std::clock() - output->cCPUTime0) / CLOCKS_PER_SEC;
}

static void XPRS_CC user_sol_notify_callback(XPRSprob xprsProblem, void* userData) {
  auto* data = (UserSolutionCallbackData*)userData;
  MIPWrapper::CBUserInfo* info = data->info;

  set_output_attributes(data->plugin, info->pOutput, xprsProblem);

  data->plugin->XPRBbegincb(*(data->problem), xprsProblem);
  data->plugin->XPRBsync(*(data->problem), XPRB_XPRS_SOL);
  set_output_variables(data->plugin, info->pOutput, data->variables);
  data->plugin->XPRBendcb(*(data->problem));

  if (info->solcbfn != nullptr) {
    (*info->solcbfn)(*info->pOutput, info->psi);
  }
}

void MIPxpressWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                                 string* names) {
  if (obj == nullptr || lb == nullptr || ub == nullptr || vt == nullptr || names == nullptr) {
    throw XpressException("invalid input");
  }
  for (size_t i = 0; i < n; ++i) {
    char* var_name = (char*)names[i].c_str();
    int var_type = convertVariableType(vt[i]);
    XPRBvar var = _plugin->XPRBnewvar(_problem, var_type, var_name, lb[i], ub[i]);
    _variables.push_back(var);
    _plugin->XPRBsetterm(_xpressObj, var, obj[i]);
  }
}

void MIPxpressWrapper::addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
                              int mask, const string& rowName) {
  addConstraint(nnz, rmatind, rmatval, sense, rhs, mask, rowName);
}

XPRBctr MIPxpressWrapper::addConstraint(int nnz, int* rmatind, double* rmatval, LinConType sense,
                                        double rhs, int mask, const string& rowName) {
  _nRows++;
  XPRBctr constraint = _plugin->XPRBnewctr(_problem, rowName.c_str(), convertConstraintType(sense));
  for (int i = 0; i < nnz; ++i) {
    _plugin->XPRBsetterm(constraint, _variables[rmatind[i]], rmatval[i]);
  }
  _plugin->XPRBsetterm(constraint, nullptr, rhs);

  return constraint;
}

void MIPxpressWrapper::writeModelIfRequested() {
  int format = XPRB_LP;
  if (_options->writeModelFormat == "lp") {
    format = XPRB_LP;
  } else if (_options->writeModelFormat == "mps") {
    format = XPRB_MPS;
  }
  if (!_options->writeModelFile.empty()) {
    _plugin->XPRBexportprob(_problem, format, _options->writeModelFile.c_str());
  }
}

void MIPxpressWrapper::addDummyConstraint() {
  if (getNCols() == 0) {
    return;
  }

  XPRBctr constraint = _plugin->XPRBnewctr(_problem, "dummy_constraint", XPRB_L);
  _plugin->XPRBsetterm(constraint, _variables[0], 1);
  double ub;
  _plugin->XPRBgetbounds(_variables[0], nullptr, &ub);
  _plugin->XPRBsetterm(constraint, nullptr, ub);
}

void MIPxpressWrapper::solve() {
  if (getNRows() == 0) {
    addDummyConstraint();
  }

  setOptions();
  writeModelIfRequested();
  setUserSolutionCallback();

  _plugin->XPRBsetobj(_problem, _xpressObj);

  cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
  cbui.pOutput->cCPUTime0 = output.dCPUTime = std::clock();

  if (_plugin->XPRBmipoptimize(_problem, "c") == 1) {
    throw XpressException("error while solving");
  }

  set_output_variables(_plugin, &output, &_variables);
  set_output_attributes(_plugin, &output, _plugin->XPRBgetXPRSprob(_problem));

  if (!_options->intermediateSolutions && cbui.solcbfn != nullptr) {
    cbui.solcbfn(output, cbui.psi);
  }
}

void MIPxpressWrapper::setUserSolutionCallback() {
  if (!_options->intermediateSolutions) {
    return;
  }

  auto* data = new UserSolutionCallbackData{&cbui, &_problem, &_variables, _plugin};

  _plugin->XPRSsetcbintsol(_plugin->XPRBgetXPRSprob(_problem), user_sol_notify_callback, data);
}

void MIPxpressWrapper::setObjSense(int s) {
  _plugin->XPRBsetsense(_problem, convertObjectiveSense(s));
}

void MIPxpressWrapper::setVarLB(int iVar, double lb) { _plugin->XPRBsetlb(_variables[iVar], lb); }

void MIPxpressWrapper::setVarUB(int iVar, double ub) { _plugin->XPRBsetub(_variables[iVar], ub); }

void MIPxpressWrapper::setVarBounds(int iVar, double lb, double ub) {
  setVarLB(iVar, lb);
  setVarUB(iVar, ub);
}

void MIPxpressWrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind,
                                              double* rmatval, LinConType sense, double rhs,
                                              const string& rowName) {
  if (bVal != 0 && bVal != 1) {
    throw XpressException("indicator bval not in 0/1");
  }
  XPRBctr constraint = addConstraint(nnz, rmatind, rmatval, sense, rhs, 0, rowName);
  _plugin->XPRBsetindicator(constraint, 2 * bVal - 1, _variables[iBVar]);
}

bool MIPxpressWrapper::addWarmStart(const std::vector<VarId>& vars,
                                    const std::vector<double>& vals) {
  XPRBsol warmstart = _plugin->XPRBnewsol(_problem);
  for (size_t ii = 0; ii < vars.size(); ii++) {
    _plugin->XPRBsetsolvar(warmstart, _variables[vars[ii]], vals[ii]);
  }
  return (_plugin->XPRBaddmipsol(_problem, warmstart, nullptr) == 0);
}

int MIPxpressWrapper::convertConstraintType(LinConType sense) {
  switch (sense) {
    case MIPWrapper::LQ:
      return XPRB_L;
    case MIPWrapper::EQ:
      return XPRB_E;
    case MIPWrapper::GQ:
      return XPRB_G;
    default:
      throw XpressException("unkown constraint sense");
  }
}

int MIPxpressWrapper::convertVariableType(VarType varType) {
  switch (varType) {
    case REAL:
      return XPRB_PL;
    case INT:
      return XPRB_UI;
    case BINARY:
      return XPRB_BV;
    default:
      throw XpressException("unknown variable type");
  }
}

int MIPxpressWrapper::convertObjectiveSense(int s) {
  switch (s) {
    case 1:
      return XPRB_MAXIM;
    case -1:
      return XPRB_MINIM;
    default:
      throw XpressException("unknown objective sense");
  }
}
