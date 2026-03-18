/*
 *  main authors:
 *     Karsten Lehmann <karsten@satalia.com>
 */

/* this source code form is subject to the terms of the mozilla public
 * license, v. 2.0. if a copy of the mpl was not distributed with this
 * file, you can obtain one at http://mozilla.org/mpl/2.0/. */

#include <minizinc/config.hh>
#include <minizinc/exception.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/solvers/MIP/MIP_xpress_wrap.hh>
#include <minizinc/utils.hh>

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
  XPRSprob problem;
  XpressPlugin* plugin;
  size_t nCols;
  std::vector<double>* x;  // Pointer to wrapper's _x vector
};

// Message callback for solver output
static void XPRS_CC xpress_message_callback(XPRSprob prob, void* context, const char* msg, int len,
                                            int msgtype) {
  // msgtype: 1=INFO, 3=WARNING, 4=ERROR (see XPRSaddcbmessage documentation)
  // Check msgtype > 0 (not len > 0) because Xpress sends len == 0 messages for line breaks
  if (msg != nullptr && msgtype > 0) {
    std::ostream& out = (msgtype == 1) ? std::cout : std::cerr;
    out.write(msg, len);
    out << std::endl;  // Always write newline after each message
  }
}

class XpressException : public runtime_error {
public:
  XpressException(const string& msg) : runtime_error(" MIPxpressWrapper: " + msg) {}
};

// Helper to check Xpress return codes and throw on error
static void check_xpress_return(int rc, const char* operation) {
  if (rc != 0) {
    throw XpressException(std::string(operation) + " (error code: " + std::to_string(rc) + ")");
  }
}

XpressPlugin::XpressPlugin() : _inner(XpressPlugin::dlls()) { loadDll(); }

XpressPlugin::XpressPlugin(const std::string& dll_file) : _inner(dll_file) { loadDll(); }

void XpressPlugin::loadDll() {
  load_symbol_dynamic(_inner, XPRSinit);
  load_symbol_dynamic(_inner, XPRSfree);
  load_symbol_dynamic(_inner, XPRSgetversion);
  load_symbol_dynamic(_inner, XPRSgetlicerrmsg);
  load_symbol_dynamic(_inner, XPRScreateprob);
  load_symbol_dynamic(_inner, XPRSdestroyprob);
  load_symbol_dynamic(_inner, XPRSloadlp);
  load_symbol_dynamic(_inner, XPRSloadmip);
  load_symbol_dynamic(_inner, XPRSaddrows);
  load_symbol_dynamic(_inner, XPRSaddcols);
  load_symbol_dynamic(_inner, XPRSoptimize);
  load_symbol_dynamic(_inner, XPRSgetsolution);
  load_symbol_dynamic(_inner, XPRSchgobjsense);
  load_symbol_dynamic(_inner, XPRSchgbounds);
  load_symbol_dynamic(_inner, XPRSchgcoltype);
  load_symbol_dynamic(_inner, XPRSwriteprob);
  load_symbol_dynamic(_inner, XPRSsetlogfile);
  load_symbol_dynamic(_inner, XPRSsetintcontrol);
  load_symbol_dynamic(_inner, XPRSsetdblcontrol);
  load_symbol_dynamic(_inner, XPRSgetintattrib);
  load_symbol_dynamic(_inner, XPRSgetdblattrib);
  load_symbol_dynamic(_inner, XPRSgetlasterror);
  load_symbol_dynamic(_inner, XPRSaddcbintsol);
  load_symbol_dynamic(_inner, XPRSaddcbmessage);
  load_symbol_dynamic(_inner, XPRSgetcontrolinfo);
  load_symbol_dynamic(_inner, XPRSgetintcontrol);
  load_symbol_dynamic(_inner, XPRSgetintcontrol64);
  load_symbol_dynamic(_inner, XPRSgetdblcontrol);
  load_symbol_dynamic(_inner, XPRSsetintcontrol64);
  load_symbol_dynamic(_inner, XPRSgetstringcontrol);
  load_symbol_dynamic(_inner, XPRSsetstrcontrol);

  // Optional functions (may not be available in all Xpress versions)
  try {
    load_symbol_dynamic(_inner, XPRSaddmipsol);
  } catch (const MiniZinc::PluginError&) {
    XPRSaddmipsol = nullptr;  // Function not available, set to nullptr
  }
  try {
    load_symbol_dynamic(_inner, XPRSaddindicators);
  } catch (const MiniZinc::PluginError&) {
    XPRSaddindicators = nullptr;  // Function not available, set to nullptr
  }
  try {
    load_symbol_dynamic(_inner, XPRSaddqmatrix);
  } catch (const MiniZinc::PluginError&) {
    XPRSaddqmatrix = nullptr;  // Function not available, set to nullptr
  }
  load_symbol_dynamic(_inner, XPRSsaveas);
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

  // Call XPRSinit for each problem - Xpress reference-counts internally
  // and only releases the license on the last XPRSfree call
  int rc = _plugin->XPRSinit(nullptr);
  check_xpress_return(rc, "Failed to initialize Xpress");

  rc = _plugin->XPRScreateprob(&_problem);
  if (rc != 0) {
    _plugin->XPRSfree();  // Undo the init on failure
    check_xpress_return(rc, "Failed to create Xpress problem");
  }
}

void MIPxpressWrapper::closeXpress() {
  if (_problem != nullptr) {
    _plugin->XPRSdestroyprob(_problem);
    _problem = nullptr;
  }
  // Pair XPRSfree with XPRSinit - Xpress reference-counts internally
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
  } catch (MiniZinc::PluginError&) {
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
    } catch (MiniZinc::PluginError&) {
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

vector<string> MIPxpressWrapper::getTags() {
  // Quadratic constraints now supported via XPRSaddqmatrix
  // Current C API migration supports: LP, MIP, indicator constraints, warm start, quadratic
  return {"mip", "float", "api", "float_times"};
}

vector<string> MIPxpressWrapper::getStdFlags() { return {"-i", "-s", "-p", "-r"}; }

vector<MiniZinc::SolverConfig::ExtraFlag> MIPxpressWrapper::getExtraFlags(
    FactoryOptions& factoryOpt) {
  try {
    Options opts;
    MIPxpressWrapper p(factoryOpt, &opts);

    XPRSprob prb = p._problem;
    // Using string parameter names because there doesn't seem to be a way to recover
    // the name from a parameter ID number
    static std::vector<std::string> all_params = {"algaftercrossover",
                                                  "algafternetwork",
                                                  "alternativeredcosts",
                                                  "autocutting",
                                                  "autoperturb",
                                                  "autoscaling",
                                                  "backtrack",
                                                  "backtracktie",
                                                  "backgroundmaxthreads",
                                                  "backgroundselect",
                                                  "baralg",
                                                  "barcores",
                                                  "barcrash",
                                                  "bardualstop",
                                                  "barfailiterlimit",
                                                  "barfreescale",
                                                  "bargapstop",
                                                  "bargaptarget",
                                                  "barhgextrapolate",
                                                  "barhggpu",
                                                  "barhggpublocksize",
                                                  "barhgmaxrestarts",
                                                  "barhgops",
                                                  "barhgprecision",
                                                  "barhgreltol",
                                                  "barindeflimit",
                                                  "bariterative",
                                                  "bariterlimit",
                                                  "barkernel",
                                                  "barlargebound",
                                                  "barobjperturb",
                                                  "barobjscale",
                                                  "barorder",
                                                  "barorderthreads",
                                                  "baroutput",
                                                  "barperturb",
                                                  "barpresolveops",
                                                  "barprimalstop",
                                                  "barrefiter",
                                                  "barregularize",
                                                  "barrhsscale",
                                                  "barsolution",
                                                  "barstart",
                                                  "barstartweight",
                                                  "barstepstop",
                                                  "barthreads",
                                                  "bigm",
                                                  "bigmmethod",
                                                  "branchchoice",
                                                  "branchdisj",
                                                  "branchstructural",
                                                  "breadthfirst",
                                                  "cachesize",
                                                  "callbackchecktimedelay",
                                                  "callbackchecktimeworkdelay",
                                                  "callbackfrommainthread",
                                                  "checkinputdata",
                                                  "choleskyalg",
                                                  "choleskytol",
                                                  "clamping",
                                                  "compute",
                                                  "computeexecservice",
                                                  "computejobpriority",
                                                  "computelog",
                                                  "concurrentthreads",
                                                  "conflictcuts",
                                                  "corespercpu",
                                                  "covercuts",
                                                  "cpialpha",
                                                  "cpuplatform",
                                                  "cputime",
                                                  "crash",
                                                  "crossover",
                                                  "crossoveraccuracytol",
                                                  "crossoveriterlimit",
                                                  "crossoverops",
                                                  "crossoverthreads",
                                                  "cstyle",
                                                  "cutdepth",
                                                  "cutfactor",
                                                  "cutfreq",
                                                  "cutselect",
                                                  "cutstrategy",
                                                  "defaultalg",
                                                  "densecollimit",
                                                  "deterministic",
                                                  "deterministiclog",
                                                  "dualgradient",
                                                  "dualize",
                                                  "dualizeops",
                                                  "dualperturb",
                                                  "dualstrategy",
                                                  "dualthreads",
                                                  "eigenvaluetol",
                                                  "elimfillin",
                                                  "elimtol",
                                                  "escapenames",
                                                  "etatol",
                                                  "extracols",
                                                  "extraelems",
                                                  "extramipents",
                                                  "extrapresolve",
                                                  "extraqcelements",
                                                  "extraqcrows",
                                                  "extrarows",
                                                  "extrasetelems",
                                                  "extrasets",
                                                  "feasibilityjump",
                                                  "feasibilitypump",
                                                  "feastol",
                                                  "feastolperturb",
                                                  "feastoltarget",
                                                  "forceoutput",
                                                  "forceparalleldual",
                                                  "genconsabstransformation",
                                                  "genconsdualreductions",
                                                  "globalboundingbox",
                                                  "globallsheurstrategy",
                                                  "globalnlpcuts",
                                                  "globalnuminitnlpcuts",
                                                  "globalpresolveobbt",
                                                  "globalspatialbranchcuttingeffort",
                                                  "globalspatialbranchifpreferorig",
                                                  "globalspatialbranchpropagationeffort",
                                                  "globaltreenlpcuts",
                                                  "gomcuts",
                                                  "gpuplatform",
                                                  "heurbeforelp",
                                                  "heurdepth",
                                                  "heurdiveiterlimit",
                                                  "heurdiverandomize",
                                                  "heurdivesoftrounding",
                                                  "heurdivespeedup",
                                                  "heurdivestrategy",
                                                  "heuremphasis",
                                                  "heurforcespecialobj",
                                                  "heurfreq",
                                                  "heurmaxsol",
                                                  "heurnodes",
                                                  "heursearchbackgroundselect",
                                                  "heursearchcopycontrols",
                                                  "heursearcheffort",
                                                  "heursearchfreq",
                                                  "heursearchrootcutfreq",
                                                  "heursearchrootselect",
                                                  "heursearchtreeselect",
                                                  "heurshiftprop",
                                                  "heurstrategy",
                                                  "heurthreads",
                                                  "historycosts",
                                                  "ifcheckconvexity",
                                                  "iislog",
                                                  "iisops",
                                                  "indlinbigm",
                                                  "indprelinbigm",
                                                  "inputtol",
                                                  "invertfreq",
                                                  "invertmin",
                                                  "iotimeout",
                                                  "keepbasis",
                                                  "keepnrows",
                                                  "l1cache",
                                                  "linelength",
                                                  "lnpbest",
                                                  "lnpiterlimit",
                                                  "localchoice",
                                                  "lpflags",
                                                  "lpfolding",
                                                  "lpiterlimit",
                                                  "lplog",
                                                  "lplogdelay",
                                                  "lplogstyle",
                                                  "lprefineiterlimit",
                                                  "lpthreads",
                                                  "markowitztol",
                                                  "matrixtol",
                                                  "maxchecksonmaxcuttime",
                                                  "maxchecksonmaxtime",
                                                  "maxcuttime",
                                                  "maxiis",
                                                  "maximpliedbound",
                                                  "maxlocalbacktrack",
                                                  "maxmcoeffbufferelems",
                                                  "maxmemoryhard",
                                                  "maxmemorysoft",
                                                  "maxmipsol",
                                                  "maxmiptasks",
                                                  "maxnode",
                                                  "maxpagelines",
                                                  "maxscalefactor",
                                                  "maxstalltime",
                                                  "maxtime",
                                                  "maxtreefilesize",
                                                  "mcfcutstrategy",
                                                  "mipabscutoff",
                                                  "mipabsgapnotify",
                                                  "mipabsgapnotifybound",
                                                  "mipabsgapnotifyobj",
                                                  "mipabsstop",
                                                  "mipaddcutoff",
                                                  "mipcomponents",
                                                  "mipconcurrentnodes",
                                                  "mipconcurrentsolves",
                                                  "mipdualreductions",
                                                  "mipfracreduce",
                                                  "mipkappafreq",
                                                  "miplog",
                                                  "mippresolve",
                                                  "miprampup",
                                                  "miprefineiterlimit",
                                                  "miprelcutoff",
                                                  "miprelgapnotify",
                                                  "miprelstop",
                                                  "miprestart",
                                                  "miprestartfactor",
                                                  "miprestartgapthreshold",
                                                  "mipterminationmethod",
                                                  "mipthreads",
                                                  "miptol",
                                                  "miptoltarget",
                                                  "miqcpalg",
                                                  "mps18compatible",
                                                  "mpsboundname",
                                                  "mpsecho",
                                                  "mpsformat",
                                                  "mpsobjname",
                                                  "mpsrangename",
                                                  "mpsrhsname",
                                                  "multiobjlog",
                                                  "multiobjops",
                                                  "mutexcallbacks",
                                                  "netstalllimit",
                                                  "nodeprobingeffort",
                                                  "nodeselection",
                                                  "numericalemphasis",
                                                  "objscalefactor",
                                                  "optimalitytol",
                                                  "optimalitytoltarget",
                                                  "outputcontrols",
                                                  "outputlog",
                                                  "outputmask",
                                                  "outputtol",
                                                  "penalty",
                                                  "perturb",
                                                  "pivottol",
                                                  "ppfactor",
                                                  "preanalyticcenter",
                                                  "prebasisred",
                                                  "prebndredcone",
                                                  "prebndredquad",
                                                  "precliquestrategy",
                                                  "precoefelim",
                                                  "precomponents",
                                                  "precomponentseffort",
                                                  "preconedecomp",
                                                  "preconfiguration",
                                                  "preconvertobjtocons",
                                                  "preconvertseparable",
                                                  "predomcol",
                                                  "predomrow",
                                                  "preduprow",
                                                  "preelimquad",
                                                  "prefolding",
                                                  "preimplications",
                                                  "prelindep",
                                                  "preobjcutdetect",
                                                  "prepermute",
                                                  "prepermuteseed",
                                                  "preprobing",
                                                  "preprotectdual",
                                                  "prerooteffort",
                                                  "prerootthreads",
                                                  "prerootworklimit",
                                                  "presolve",
                                                  "presolvemaxgrow",
                                                  "presolveops",
                                                  "presolvepasses",
                                                  "presort",
                                                  "pricingalg",
                                                  "primalops",
                                                  "primalperturb",
                                                  "primalunshift",
                                                  "pseudocost",
                                                  "pwldualreductions",
                                                  "pwlnonconvextransformation",
                                                  "qccuts",
                                                  "qcrootalg",
                                                  "qsimplexops",
                                                  "quadraticunshift",
                                                  "randomseed",
                                                  "refactor",
                                                  "refineops",
                                                  "relaxtreememorylimit",
                                                  "relpivottol",
                                                  "repairindefiniteq",
                                                  "repairinfeasmaxtime",
                                                  "repairinfeastimelimit",
                                                  "resourcestrategy",
                                                  "rltcuts",
                                                  "rootpresolve",
                                                  "sbbest",
                                                  "sbeffort",
                                                  "sbestimate",
                                                  "sbiterlimit",
                                                  "sbselect",
                                                  "scaling",
                                                  "sdpcutstrategy",
                                                  "serializepreintsol",
                                                  "sifting",
                                                  "siftpasses",
                                                  "siftpresolveops",
                                                  "siftswitch",
                                                  "sleeponthreadwait",
                                                  "soltimelimit",
                                                  "sosreftol",
                                                  "symmetry",
                                                  "symselect",
                                                  "threads",
                                                  "timelimit",
                                                  "trace",
                                                  "treecompression",
                                                  "treecovercuts",
                                                  "treecutselect",
                                                  "treediagnostics",
                                                  "treefileloginterval",
                                                  "treegomcuts",
                                                  "treememorylimit",
                                                  "treememorysavingtarget",
                                                  "treeqccuts",
                                                  "tunerhistory",
                                                  "tunermaxtime",
                                                  "tunermethod",
                                                  "tunermethodfile",
                                                  "tunermode",
                                                  "tuneroutput",
                                                  "tuneroutputpath",
                                                  "tunerpermute",
                                                  "tunersessionname",
                                                  "tunertarget",
                                                  "tunerthreads",
                                                  "tunerrootalg",
                                                  "tunerverbose",
                                                  "usersolheuristic",
                                                  "varselection",
                                                  "version",
                                                  "worklimit"};
    std::vector<MiniZinc::SolverConfig::ExtraFlag> res;
    for (auto param : all_params) {
      int n;
      int t;
      int rc = p._plugin->XPRSgetcontrolinfo(prb, param.c_str(), &n, &t);
      if (rc != 0) {
        // Skip parameters that don't exist in this Xpress version
        continue;
      }
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
  } catch (MiniZinc::PluginError&) {
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
     << "--solver-time-limit <N>        stop search after N milliseconds wall time, if negative, "
        "it will only stop if at least one solution was found"
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
     << "-r <N>, --seed <N>, --random-seed <N>   random seed, integer" << std::endl
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
  } else if (cop.get("--solver-time-limit", &nTimeout)) {    // NOLINT: Allow repeated empty if
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
  // Set output log level via control
  _plugin->XPRSsetintcontrol(_problem, XPRS_OUTPUTLOG, _options->msgLevel);

  _plugin->XPRSsetlogfile(_problem, _options->logFile.c_str());
  if (_options->nTimeout > 0) {
    double timeLimitSec = static_cast<double>(_options->nTimeout) / 1000.0;
    _plugin->XPRSsetdblcontrol(_problem, XPRS_TIMELIMIT, timeLimitSec);
  }
  _plugin->XPRSsetintcontrol(_problem, XPRS_MAXMIPSOL, _options->numSolutions);
  _plugin->XPRSsetdblcontrol(_problem, XPRS_MIPABSSTOP, _options->absGap);
  _plugin->XPRSsetdblcontrol(_problem, XPRS_MIPRELSTOP, _options->relGap);

  if (_options->numThreads > 0) {
    _plugin->XPRSsetintcontrol(_problem, XPRS_THREADS, _options->numThreads);
  }

  if (_options->randomSeed != 0) {
    _plugin->XPRSsetintcontrol(_problem, XPRS_RANDOMSEED, _options->randomSeed);
  }

  for (auto& it : _options->extraParams) {
    auto name = it.first.substr(9);
    int n;
    int t;
    int rc = _plugin->XPRSgetcontrolinfo(_problem, name.c_str(), &n, &t);
    if (rc != 0) {
      // Skip parameters that don't exist in this Xpress version
      continue;
    }
    switch (t) {
      case XPRS_TYPE_INT:
        _plugin->XPRSsetintcontrol(_problem, n, stoi(it.second));
        break;
      case XPRS_TYPE_INT64:
        _plugin->XPRSsetintcontrol64(_problem, n, stoll(it.second));
        break;
      case XPRS_TYPE_DOUBLE:
        _plugin->XPRSsetdblcontrol(_problem, n, stod(it.second));
        break;
      case XPRS_TYPE_STRING:
        _plugin->XPRSsetstrcontrol(_problem, n, it.second.c_str());
        break;
      default:
        throw XpressException("Unknown type for parameter " + name);
    }
  }
}

static MIPWrapper::Status convert_status(int solStatus) {
  switch (solStatus) {
    case XPRS_SOLSTATUS_OPTIMAL:
      return MIPWrapper::Status::OPT;
    case XPRS_SOLSTATUS_FEASIBLE:
      return MIPWrapper::Status::SAT;
    case XPRS_SOLSTATUS_INFEASIBLE:
      return MIPWrapper::Status::UNSAT;
    case XPRS_SOLSTATUS_UNBOUNDED:
      return MIPWrapper::Status::UNBND;
    default:
      return MIPWrapper::Status::UNKNOWN;
  }
}

static string get_status_name(int solStatus) {
  switch (solStatus) {
    case XPRS_SOLSTATUS_OPTIMAL:
      return "optimal";
    case XPRS_SOLSTATUS_FEASIBLE:
      return "feasible";
    case XPRS_SOLSTATUS_INFEASIBLE:
      return "infeasible";
    case XPRS_SOLSTATUS_UNBOUNDED:
      return "unbounded";
    case XPRS_SOLSTATUS_NOTFOUND:
      return "no solution found";
    default:
      return "unknown";
  }
}

static void set_output_variables(XpressPlugin* plugin, MIPxpressWrapper::Output* output,
                                 XPRSprob problem, size_t nCols, std::vector<double>* x) {
  x->resize(nCols);
  int status;
  plugin->XPRSgetsolution(problem, &status, x->data(), 0, static_cast<int>(nCols) - 1);
  output->x = x->data();
}

static void set_output_attributes(XpressPlugin* plugin, MIPxpressWrapper::Output* output,
                                  XPRSprob _problem, int solStatus = -1) {
  // If solStatus not provided, query it from the problem
  if (solStatus < 0) {
    plugin->XPRSgetintattrib(_problem, XPRS_SOLSTATUS, &solStatus);
  }
  output->status = convert_status(solStatus);
  output->statusName = get_status_name(solStatus);

  plugin->XPRSgetdblattrib(_problem, XPRS_OBJVAL, &output->objVal);
  plugin->XPRSgetdblattrib(_problem, XPRS_BESTBOUND, &output->bestBound);

  plugin->XPRSgetintattrib(_problem, XPRS_NODES, &output->nNodes);
  plugin->XPRSgetintattrib(_problem, XPRS_ACTIVENODES, &output->nOpenNodes);

  output->dWallTime =
      std::chrono::duration<double>(std::chrono::steady_clock::now() - output->dWallTime0).count();
  output->dCPUTime = double(std::clock() - output->cCPUTime0) / CLOCKS_PER_SEC;
}

static void XPRS_CC user_sol_notify_callback(XPRSprob problem, void* userData) {
  auto* data = (UserSolutionCallbackData*)userData;
  MIPWrapper::CBUserInfo* info = data->info;

  set_output_attributes(data->plugin, info->pOutput, problem);
  set_output_variables(data->plugin, info->pOutput, problem, data->nCols, data->x);

  if (info->solcbfn != nullptr) {
    (*info->solcbfn)(*info->pOutput, info->psi);
  }
}

void MIPxpressWrapper::doAddVars(size_t n, double* obj, double* lb, double* ub, VarType* vt,
                                 string* names) {
  if (_problemLoaded) {
    throw XpressException("Cannot add variables after problem is loaded");
  }
  if (obj == nullptr || lb == nullptr || ub == nullptr || vt == nullptr) {
    throw XpressException("invalid input");
  }

  for (size_t i = 0; i < n; ++i) {
    _obj.push_back(obj[i]);
    _lb.push_back(lb[i]);
    _ub.push_back(ub[i]);

    // Convert variable type to character
    switch (vt[i]) {
      case REAL:
        _vtype.push_back('C');  // Continuous
        break;
      case INT:
        _vtype.push_back('I');  // Integer
        break;
      case BINARY:
        _vtype.push_back('B');  // Binary
        break;
      default:
        throw XpressException("Unknown variable type");
    }
  }
  _nCols += n;
}

void MIPxpressWrapper::addRow(int nnz, int* rmatind, double* rmatval, LinConType sense, double rhs,
                              int mask, const string& rowName) {
  if (_problemLoaded) {
    throw XpressException("Cannot add rows after problem is loaded");
  }
  // Convert constraint sense
  char rowtype;
  switch (sense) {
    case LQ:
      rowtype = 'L';  // <=
      break;
    case EQ:
      rowtype = 'E';  // ==
      break;
    case GQ:
      rowtype = 'G';  // >=
      break;
    default:
      throw XpressException("Unknown constraint type");
  }

  _rowtype.push_back(rowtype);
  _rhs.push_back(rhs);
  _rng.push_back(0.0);  // No range
  _start.push_back(static_cast<int>(_rowind.size()));

  // Add constraint coefficients
  for (int i = 0; i < nnz; ++i) {
    _rowind.push_back(rmatind[i]);
    _rowcoef.push_back(rmatval[i]);
  }

  _nRows++;
}

void MIPxpressWrapper::writeModelIfRequested() {
  if (!_options->writeModelFile.empty()) {
    const std::string& filename = _options->writeModelFile;
    // Use XPRSsaveas for .svf files (saves internal state), XPRSwriteprob otherwise
    int rc;
    if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".svf") {
      rc = _plugin->XPRSsaveas(_problem, filename.c_str());
      check_xpress_return(rc, ("Failed to save problem to " + filename).c_str());
    } else {
      const char* flags = "";
      rc = _plugin->XPRSwriteprob(_problem, filename.c_str(), flags);
      check_xpress_return(rc, ("Failed to write problem to " + filename).c_str());
    }
  }
}

void MIPxpressWrapper::loadProblem() {
  if (_problemLoaded) {
    return;
  }

  if (_nCols == 0) {
    // Empty problem - nothing to load
    _problemLoaded = true;
    return;
  }

  // Collect MIP entities (integer and binary variables)
  std::vector<int> entind;
  std::vector<char> coltype;
  for (size_t i = 0; i < _nCols; ++i) {
    if (_vtype[i] == 'I' || _vtype[i] == 'B') {
      entind.push_back(static_cast<int>(i));
      coltype.push_back(_vtype[i]);
    }
  }

  // Load problem with variables (no constraints initially)
  std::vector<int> empty_start(_nCols + 1, 0);  // All columns start at 0 (empty)
  int nentities = static_cast<int>(entind.size());

  // Use XPRSloadmip to load variables with types in one call (avoids bound reset issue)
  int rc = _plugin->XPRSloadmip(
      _problem, "mzn_problem", static_cast<int>(_nCols), 0,    // 0 rows initially
      nullptr, nullptr, nullptr,                               // No row data yet
      _obj.data(), empty_start.data(), nullptr,                // Objective with empty matrix
      nullptr, nullptr, _lb.data(), _ub.data(), nentities, 0,  // nentities, nsets=0
      nentities > 0 ? coltype.data() : nullptr,                // Entity types
      nentities > 0 ? entind.data() : nullptr,                 // Entity indices
      nullptr, nullptr, nullptr, nullptr, nullptr);            // No limits, no SOS sets
  check_xpress_return(rc, "Failed to load MIP problem");

  // Add all constraints (pass nullptr for range to allow quadratic equalities)
  if (_nRows > 0) {
    rc = _plugin->XPRSaddrows(_problem, static_cast<int>(_nRows), static_cast<int>(_rowind.size()),
                              _rowtype.data(), _rhs.data(), nullptr, _start.data(), _rowind.data(),
                              _rowcoef.data());
    check_xpress_return(rc, "Failed to add rows");
  }

  // Add indicator constraints if any
  if (!_indicatorRows.empty()) {
    if (_plugin->XPRSaddindicators != nullptr) {
      rc = _plugin->XPRSaddindicators(_problem, static_cast<int>(_indicatorRows.size()),
                                      _indicatorRows.data(), _indicatorVars.data(),
                                      _indicatorComplements.data());
      check_xpress_return(rc, "Failed to add indicator constraints");
    } else {
      throw XpressException(
          "Indicator constraints requested but not supported in this version of Xpress");
    }
  }

  // Add bilinear terms using XPRSaddqmatrix
  if (!_bilinearTerms.empty()) {
    if (_plugin->XPRSaddqmatrix == nullptr) {
      throw XpressException("XPRSaddqmatrix not available for bilinear constraints");
    }

    // Add each bilinear term as a quadratic matrix entry
    // Note: Q matrix is symmetric, so x*y term needs coefficient 0.5
    // (contribution is 0.5*Q[x,y]*x*y + 0.5*Q[y,x]*y*x = Q[x,y]*x*y when Q is symmetric)
    for (const auto& term : _bilinearTerms) {
      int mqcol1[] = {term.x};
      int mqcol2[] = {term.y};
      double dqe[] = {0.5};  // Coefficient 0.5 for symmetric Q matrix

      rc = _plugin->XPRSaddqmatrix(_problem, term.row, 1, mqcol1, mqcol2, dqe);
      check_xpress_return(rc, "Failed to add quadratic matrix");
    }
  }

  _problemLoaded = true;
}

void MIPxpressWrapper::solve() {
  // Set output log level and message callback for solver output
  _plugin->XPRSsetintcontrol(_problem, XPRS_OUTPUTLOG, _options->msgLevel);
  if (_options->msgLevel > 0) {
    _plugin->XPRSaddcbmessage(_problem, xpress_message_callback, nullptr, 0);
  }

  // Load problem if not already loaded
  loadProblem();

  // Set options
  setOptions();

  // Write model if requested
  writeModelIfRequested();

  // Set callback if needed
  setUserSolutionCallback();

  // Set objective sense
  int obj_rc = _plugin->XPRSchgobjsense(_problem, _objsense);

  // Start timing
  cbui.pOutput->dWallTime0 = output.dWallTime0 = std::chrono::steady_clock::now();
  cbui.pOutput->cCPUTime0 = output.cCPUTime0 = std::clock();

  // Solve using XPRSoptimize (general optimizer that handles LP, MIP, and NLP)
  // Signature: XPRSoptimize(prob, flags, *solvestatus, *solstatus)
  // solvestatus = how did the optimization process go
  // solstatus = what is the solution status (OPTIMAL, FEASIBLE, etc.)
  int solveStatus = 0;
  int solStatus = 0;
  int rc = _plugin->XPRSoptimize(_problem, "", &solveStatus, &solStatus);

  if (rc != 0) {
    if (rc == 864 || rc == 862) {
      // Error 862/864: Unexpected QCQP error (should not occur with correct implementation)
      throw XpressException(
          "Xpress error 862/864: Quadratic constraint error. This should not occur. "
          "Please report this issue.");
    }
    char errmsg[512];
    _plugin->XPRSgetlasterror(_problem, errmsg);
    std::string errStr = "Error while solving (rc=" + std::to_string(rc) + "): " + errmsg;
    throw XpressException(errStr);
  }

  // Retrieve solution using XPRSgetsolution (works for LP, MIP, and NLP)
  _x.resize(_nCols);
  int getSolStatus;
  _plugin->XPRSgetsolution(_problem, &getSolStatus, _x.data(), 0, static_cast<int>(_nCols) - 1);
  output.x = _x.data();

  // Get status and attributes using solution status from XPRSoptimize
  set_output_attributes(_plugin, &output, _problem, solStatus);

  if (!_options->intermediateSolutions && cbui.solcbfn != nullptr) {
    cbui.solcbfn(output, cbui.psi);
  }
}

void MIPxpressWrapper::setUserSolutionCallback() {
  if (!_options->intermediateSolutions) {
    return;
  }

  auto* data = new UserSolutionCallbackData{&cbui, _problem, _plugin, _nCols, &_x};

  _plugin->XPRSaddcbintsol(_problem, user_sol_notify_callback, data, 0);
}

void MIPxpressWrapper::setObjSense(int s) {
  _objsense = (s == 1) ? XPRS_OBJ_MAXIMIZE : XPRS_OBJ_MINIMIZE;
  if (_problemLoaded) {
    _plugin->XPRSchgobjsense(_problem, _objsense);
  }
}

void MIPxpressWrapper::setVarLB(int iVar, double lb) {
  _lb[iVar] = lb;
  if (_problemLoaded) {
    char btype = 'L';
    int colind = iVar;
    _plugin->XPRSchgbounds(_problem, 1, &colind, &btype, &lb);
  }
}

void MIPxpressWrapper::setVarUB(int iVar, double ub) {
  _ub[iVar] = ub;
  if (_problemLoaded) {
    char btype = 'U';
    int colind = iVar;
    _plugin->XPRSchgbounds(_problem, 1, &colind, &btype, &ub);
  }
}

void MIPxpressWrapper::setVarBounds(int iVar, double lb, double ub) {
  _lb[iVar] = lb;
  _ub[iVar] = ub;
  if (_problemLoaded) {
    char btype[2] = {'L', 'U'};
    int colind[2] = {iVar, iVar};
    double bval[2] = {lb, ub};
    _plugin->XPRSchgbounds(_problem, 2, colind, btype, bval);
  }
}

void MIPxpressWrapper::addIndicatorConstraint(int iBVar, int bVal, int nnz, int* rmatind,
                                              double* rmatval, LinConType sense, double rhs,
                                              const string& rowName) {
  // Check if XPRSaddindicators is available
  if (_plugin->XPRSaddindicators == nullptr) {
    throw XpressException("Indicator constraints not supported in this version of Xpress");
  }

  // In Xpress, indicator constraints work as follows:
  // 1. Add the constraint as a normal row
  // 2. Mark it as an indicator constraint using XPRSaddindicators
  //
  // The indicator semantics: if binary variable iBVar == bVal, then the constraint is active

  // First, add the constraint as a normal row
  addRow(nnz, rmatind, rmatval, sense, rhs, 0, rowName);

  // The row we just added is at index (_nRows - 1)
  int rowIndex = static_cast<int>(_nRows - 1);

  // Determine complement flag for XPRSaddindicators:
  // 1 means: constraint is active if binary variable == 1
  // -1 means: constraint is active if binary variable == 0
  int complement = (bVal == 0) ? -1 : 1;

  // If problem is already loaded, we need to add the indicator now
  if (_problemLoaded) {
    int rc = _plugin->XPRSaddindicators(_problem, 1, &rowIndex, &iBVar, &complement);
    check_xpress_return(rc, "Failed to add indicator constraint");
  } else {
    // Store indicator info for later when we load the problem
    // We need to track these for when loadProblem() is called
    _indicatorRows.push_back(rowIndex);
    _indicatorVars.push_back(iBVar);
    _indicatorComplements.push_back(complement);
  }
}

bool MIPxpressWrapper::addWarmStart(const std::vector<VarId>& vars,
                                    const std::vector<double>& vals) {
  assert(vars.size() == vals.size());
  static_assert(sizeof(VarId) == sizeof(int), "VarId should be (u)int currently");

  // Check if XPRSaddmipsol is available
  if (_plugin->XPRSaddmipsol == nullptr) {
    return false;  // Warm start not supported in this version
  }

  if (!_problemLoaded) {
    // Problem must be loaded before adding MIP start
    loadProblem();
  }

  // XPRSaddmipsol takes arrays of column indices and values
  // ilength = number of non-default values to set
  // mipsolcol = array of column indices
  // mipsolval = array of values
  // solname = name of the solution (can be nullptr)
  int rc = _plugin->XPRSaddmipsol(_problem, static_cast<int>(vars.size()), vals.data(),
                                  reinterpret_cast<const int*>(vars.data()), nullptr);
  if (rc != 0) {
    return false;  // Warm start failed, but don't throw - just indicate failure
  }
  return true;
}

void MIPxpressWrapper::addTimes(int x, int y, int z, const string& rowName) {
  // Bilinear equality constraint: z = x * y
  // Implemented via XPRSaddqmatrix (adds x*y quadratic term to the row)

  if (_plugin->XPRSaddqmatrix == nullptr) {
    throw XpressException("Bilinear constraints require XPRSaddqmatrix, which is not available.");
  }

  if (_problemLoaded) {
    throw XpressException("Cannot add bilinear constraints after problem is loaded.");
  }

  // Add the linear row: -z = 0 (we'll add x*y quadratic term later via XPRSaddqmatrix)
  int colind[] = {z};
  double colval[] = {-1.0};
  addRow(1, colind, colval, EQ, 0.0, MaskConsType_Normal, rowName);

  // Store bilinear term information for later processing in loadProblem()
  BilinearTerm term;
  term.row = static_cast<int>(_nRows - 1);
  term.x = x;
  term.y = y;
  term.z = z;
  _bilinearTerms.push_back(term);
}
