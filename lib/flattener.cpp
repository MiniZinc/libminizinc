/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* A basic mzn2fzn wrapper, can be used as a plugin
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/flattener.hh>
#include <minizinc/pathfileprinter.hh>

#include <fstream>

#ifdef HAS_GECODE
#include <minizinc/solvers/gecode_solverinstance.hh>

#include <utility>
#endif

using namespace std;
using namespace MiniZinc;

void Flattener::printVersion(ostream& os) {
  os << "MiniZinc to FlatZinc converter, version " << MZN_VERSION_MAJOR << "." << MZN_VERSION_MINOR
     << "." << MZN_VERSION_PATCH;
  if (!std::string(MZN_BUILD_REF).empty()) {
    os << ", build " << MZN_BUILD_REF;
  }
  os << std::endl;
  os << "Copyright (C) 2014-" << string(__DATE__).substr(7, 4)
     << " Monash University, NICTA, Data61" << std::endl;
}

void Flattener::printHelp(ostream& os) const {
  os << std::endl
     << "Flattener input options:" << std::endl
     << "  --instance-check-only\n    Check the model instance (including data) for errors, but "
        "do "
        "not\n    convert to FlatZinc."
     << std::endl
     << "  -e, --model-check-only\n    Check the model (without requiring data) for errors, but "
        "do "
        "not\n    convert to FlatZinc."
     << std::endl
     << "  --model-interface-only\n    Only extract parameters and output variables." << std::endl
     << "  --model-types-only\n    Only output variable (enum) type information." << std::endl
     << "  --no-optimize\n    Do not optimize the FlatZinc" << std::endl
     << "  --no-chain-compression\n    Do not simplify chains of implication constraints."
     << std::endl
     << "  -m <file>, --model <file>\n    File named <file> is the model." << std::endl
     << "  -d <file>, --data <file>\n    File named <file> contains data used by the model."
     << std::endl
     << "  -D <data>, --cmdline-data <data>\n    Include the given data assignment in the model."
     << std::endl
     << "  --stdlib-dir <dir>\n    Path to MiniZinc standard library directory" << std::endl
     << "  -G <dir>, --globals-dir <dir>, --mzn-globals-dir <dir>\n    Search for included "
        "globals "
        "in <stdlib>/<dir>."
     << std::endl
     << "  -, --input-from-stdin\n    Read problem from standard input" << std::endl
     << "  -I <dir>, --search-dir <dir>\n    Additionally search for included files in <dir>."
     << std::endl
     << "  -D \"fMIPdomains=true\"\n    Switch on MIPDomain Unification" << std::endl
     << "  --MIPDMaxIntvEE <n>\n    MIPD: max integer domain subinterval length to enforce "
        "equality encoding, default "
     << _optMIPDmaxIntvEE << std::endl
     << "  --MIPDMaxDensEE <n>\n    MIPD: max domain cardinality to N subintervals ratio\n    to "
        "enforce equality encoding, default "
     << _optMIPDmaxDensEE << ", either condition triggers" << std::endl
     << "  --only-range-domains\n    When no MIPdomains: all domains contiguous, holes replaced "
        "by "
        "inequalities"
     << std::endl
     << "  --allow-multiple-assignments\n    Allow multiple assignments to the same variable "
        "(e.g. "
        "in dzn)"
     << std::endl
     << "  --no-half-reifications\n    Only use fully reified constraints, even when a half "
        "reified constraint is defined."
     << std::endl
     << "  --compile-solution-checker <file>.mzc.mzn\n    Compile solution checker model"
     << std::endl
     << std::endl
     << "Flattener two-pass options:" << std::endl
     << "  --two-pass\n    Flatten twice to make better flattening decisions for the target"
     << std::endl
#ifdef HAS_GECODE
     << "  --use-gecode\n    Perform root-node-propagation with Gecode (adds --two-pass)"
     << std::endl
     << "  --shave\n    Probe bounds of all variables at the root node (adds --use-gecode)"
     << std::endl
     << "  --sac\n    Probe values of all variables at the root node (adds --use-gecode)"
     << std::endl
     << "  --pre-passes <n>\n    Number of times to apply shave/sac pass (0 = fixed-point, 1 = "
        "default)"
     << std::endl
#endif
     << "  -O<n>\n    Two-pass optimisation levels:" << std::endl
     << "    -O0:    Disable optimize (--no-optimize)  -O1:    Single pass (default)" << std::endl
     << "    -O2:    Same as: --two-pass"
#ifdef HAS_GECODE
     << "               -O3:    Same as: --use-gecode" << std::endl
     << "    -O4:    Same as: --shave                  -O5:    Same as: --sac" << std::endl
#else
     << "\n    -O3,4,5:    Disabled [Requires MiniZinc with built-in Gecode support]" << std::endl
#endif
     << "  -g\n    Debug mode: Forces -O0 and records all domain changes as constraints instead "
        "of "
        "applying them"
     << std::endl
     << std::endl;
  os << "Flattener output options:" << std::endl
     << "  --no-output-ozn, -O-\n    Do not output ozn file" << std::endl
     << "  --output-base <name>\n    Base name for output files" << std::endl
     << (_fOutputByDefault
             ? "  -o <file>, --fzn <file>, --output-to-file <file>, --output-fzn-to-file <file>\n"
             : "  --fzn <file>, --output-fzn-to-file <file>\n")
     << "    Filename for generated FlatZinc output" << std::endl
     << "  --ozn, --output-ozn-to-file <file>\n    Filename for model output specification "
        "(--ozn- "
        "for none)"
     << std::endl
     << "  --keep-paths\n    Don't remove path annotations from FlatZinc" << std::endl
     << "  --output-paths\n    Output a symbol table (.paths file)" << std::endl
     << "  --output-paths-to-file <file>\n    Output a symbol table (.paths file) to <file>"
     << std::endl
     << "  --output-detailed-timing\n    Output detailed profiling information of compilation time"
     << std::endl
     << "  --output-to-stdout, --output-fzn-to-stdout\n    Print generated FlatZinc to standard "
        "output"
     << std::endl
     << "  --output-ozn-to-stdout\n    Print model output specification to standard output"
     << std::endl
     << "  --output-paths-to-stdout\n    Output symbol table to standard output" << std::endl
     << "  --output-mode <item|dzn|json|checker>\n    Create output according to output item "
        "(default), or output compatible\n    with dzn or json format, or for solution checking"
     << std::endl
     << "  --output-objective\n    Print value of objective function in dzn or json output"
     << std::endl
     << "  --output-output-item\n    Print the output item as a string in the dzn or json output"
     << std::endl
     << "  -Werror\n    Turn warnings into errors" << std::endl;
}

bool Flattener::processOption(int& i, std::vector<std::string>& argv,
                              const std::string& workingDir) {
  CLOParser cop(i, argv);
  string buffer;
  int intBuffer;

  if (cop.getOption("-I --search-dir", &buffer)) {
    _includePaths.push_back(FileUtils::file_path(buffer + "/", workingDir));
  } else if (cop.getOption("--no-typecheck")) {
    _flags.typecheck = false;
  } else if (cop.getOption("--instance-check-only")) {
    _flags.instanceCheckOnly = true;
  } else if (cop.getOption("-e --model-check-only")) {
    _flags.modelCheckOnly = true;
  } else if (cop.getOption("--model-interface-only")) {
    _flags.modelInterfaceOnly = true;
  } else if (cop.getOption("--model-types-only")) {
    _flags.modelTypesOnly = true;
  } else if (cop.getOption("-v --verbose")) {
    _flags.verbose = true;
  } else if (cop.getOption("--newfzn")) {
    _flags.newfzn = true;
  } else if (cop.getOption("--no-optimize --no-optimise")) {
    _flags.optimize = false;
  } else if (cop.getOption("--no-chain-compression")) {
    _flags.chainCompression = false;
  } else if (cop.getOption("--no-output-ozn -O-")) {
    _flags.noOutputOzn = true;
  } else if (cop.getOption("--output-base", &_flagOutputBase)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption(_fOutputByDefault ? "-o --fzn --output-to-file --output-fzn-to-file"
                                             : "--fzn --output-fzn-to-file",
                           &buffer)) {
    _flagOutputFzn = FileUtils::file_path(buffer, workingDir);
  } else if (cop.getOption("--output-paths")) {
    _fopts.collectMznPaths = true;
  } else if (cop.getOption("--output-paths-to-file", &buffer)) {
    _flagOutputPaths = FileUtils::file_path(buffer, workingDir);
    _fopts.collectMznPaths = true;
  } else if (cop.getOption("--output-to-stdout --output-fzn-to-stdout")) {
    _flags.outputFznStdout = true;
  } else if (cop.getOption("--output-ozn-to-stdout")) {
    _flags.outputOznStdout = true;
  } else if (cop.getOption("--output-paths-to-stdout")) {
    _fopts.collectMznPaths = true;
    _flags.outputPathsStdout = true;
  } else if (cop.getOption("--output-detailed-timing")) {
    _fopts.detailedTiming = true;
  } else if (cop.getOption("--output-mode", &buffer)) {
    if (buffer == "dzn") {
      _flagOutputMode = FlatteningOptions::OUTPUT_DZN;
    } else if (buffer == "json") {
      _flagOutputMode = FlatteningOptions::OUTPUT_JSON;
    } else if (buffer == "item") {
      _flagOutputMode = FlatteningOptions::OUTPUT_ITEM;
    } else if (buffer == "checker") {
      _flagOutputMode = FlatteningOptions::OUTPUT_CHECKER;
    } else {
      return false;
    }
  } else if (cop.getOption("--output-objective")) {
    _flags.outputObjective = true;
  } else if (cop.getOption("--output-output-item")) {
    _flags.outputOutputItem = true;
  } else if (cop.getOption("- --input-from-stdin")) {
    _flags.stdinInput = true;
  } else if (cop.getOption("-d --data", &buffer)) {
    auto last_dot = buffer.find_last_of('.');
    if (last_dot == string::npos) {
      return false;
    }
    auto extension = buffer.substr(last_dot, string::npos);
    if (extension != ".dzn" && extension != ".json") {
      return false;
    }
    _datafiles.push_back(FileUtils::file_path(buffer, workingDir));
  } else if (cop.getOption("--stdlib-dir", &buffer)) {
    _stdLibDir = FileUtils::file_path(buffer, workingDir);
  } else if (cop.getOption("-G --globals-dir --mzn-globals-dir",
                           &_globalsDir)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("-D --cmdline-data", &buffer)) {
    _datafiles.push_back("cmd:/" + buffer);
  } else if (cop.getOption("--allow-unbounded-vars")) {
    _flags.allowUnboundedVars = true;
  } else if (cop.getOption("--only-range-domains")) {
    _flags.onlyRangeDomains = true;
  } else if (cop.getOption("--no-MIPdomains")) {  // internal
    _flags.noMIPdomains = true;
  } else if (cop.getOption("--MIPDMaxIntvEE",
                           &_optMIPDmaxIntvEE)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--MIPDMaxDensEE",
                           &_optMIPDmaxDensEE)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("-Werror")) {
    _flags.werror = true;
  } else if (cop.getOption("--use-gecode")) {
#ifdef HAS_GECODE
    _flags.twoPass = true;
    _flags.gecode = true;
#else
    _log << "warning: Gecode not available. Ignoring '--use-gecode'\n";
#endif
  } else if (cop.getOption("--sac")) {
#ifdef HAS_GECODE
    _flags.twoPass = true;
    _flags.gecode = true;
    _flags.sac = true;
#else
    _log << "warning: Gecode not available. Ignoring '--sac'\n";
#endif

  } else if (cop.getOption("--shave")) {
#ifdef HAS_GECODE
    _flags.twoPass = true;
    _flags.gecode = true;
    _flags.shave = true;
#else
    _log << "warning: Gecode not available. Ignoring '--shave'\n";
#endif
  } else if (cop.getOption("--two-pass")) {
    _flags.twoPass = true;
  } else if (cop.getOption("--pre-passes", &intBuffer)) {
    if (intBuffer >= 0) {
      _flagPrePasses = static_cast<unsigned int>(intBuffer);
    }
  } else if (cop.getOption("-O", &intBuffer)) {
    switch (intBuffer) {
      case 0: {
        _flags.optimize = false;
        break;
      }
      case 1: {
        // Default settings
        break;
      }
      case 2: {
        _flags.twoPass = true;
        break;
      }
      case 3: {
        _flags.twoPass = true;
        _flags.gecode = true;
        break;
      }
      case 4: {
        _flags.twoPass = true;
        _flags.gecode = true;
        _flags.shave = true;
        break;
      }
      case 5: {
        _flags.twoPass = true;
        _flags.gecode = true;
        _flags.sac = true;
        break;
      }
      default: {
        _log << "% Error: Unsupported optimisation level, cannot process -O" << intBuffer << "."
             << std::endl;
        return false;
      }
    }
    // ozn options must be after the -O<n> optimisation options
  } else if (cop.getOption("--ozn --output-ozn-to-file", &buffer)) {
    _flagOutputOzn = FileUtils::file_path(buffer, workingDir);
  } else if (cop.getOption("-g")) {
    _flags.optimize = false;
    _flags.twoPass = false;
    _flags.gecode = false;
    _flags.shave = false;
    _flags.sac = false;
    _fopts.recordDomainChanges = true;
  } else if (string(argv[i]) == "--keep-paths") {
    _flags.keepMznPaths = true;
    _fopts.collectMznPaths = true;
  } else if (string(argv[i]) == "--only-toplevel-presolve") {
    _fopts.onlyToplevelPaths = true;
  } else if (cop.getOption("--allow-multiple-assignments")) {
    _flags.allowMultiAssign = true;
  } else if (cop.getOption("--no-half-reifications")) {
    _fopts.enableHalfReification = false;
  } else if (string(argv[i]) == "--input-is-flatzinc") {
    _isFlatzinc = true;
  } else if (cop.getOption("--compile-solution-checker", &buffer)) {
    if (buffer.length() >= 8 && buffer.substr(buffer.length() - 8, string::npos) == ".mzc.mzn") {
      _flags.compileSolutionCheckModel = true;
      _flags.modelCheckOnly = true;
      _filenames.push_back(FileUtils::file_path(buffer, workingDir));
    } else {
      _log << "Error: solution checker model must have extension .mzc.mzn" << std::endl;
      return false;
    }
  } else if (cop.getOption("-m --model", &buffer)) {
    if (buffer.length() <= 4) {
      return false;
    }
    auto extension = buffer.substr(buffer.length() - 4, string::npos);
    auto isChecker =
        buffer.length() > 8 && buffer.substr(buffer.length() - 8, string::npos) == ".mzc.mzn";
    if ((extension == ".mzn" && !isChecker) || extension == ".fzn") {
      if (extension == ".fzn") {
        _isFlatzinc = true;
        if (_fOutputByDefault) {  // mzn2fzn mode
          return false;
        }
      }
      _filenames.push_back(FileUtils::file_path(buffer, workingDir));
      return true;
    }
    _log << "Error: model must have extension .mzn (or .fzn)" << std::endl;
    return false;
  } else {
    std::string input_file(argv[i]);
    if (input_file.length() <= 4) {
      return false;
    }
    size_t last_dot = input_file.find_last_of('.');
    if (last_dot == string::npos) {
      return false;
    }
    std::string extension = input_file.substr(last_dot, string::npos);
    if (extension == ".mzc" ||
        (input_file.length() >= 8 &&
         input_file.substr(input_file.length() - 8, string::npos) == ".mzc.mzn")) {
      _flagSolutionCheckModel = input_file;
    } else if (extension == ".mzn" || extension == ".fzn") {
      if (extension == ".fzn") {
        _isFlatzinc = true;
        if (_fOutputByDefault) {  // mzn2fzn mode
          return false;
        }
      }
      _filenames.push_back(input_file);
    } else if (extension == ".dzn" || extension == ".json") {
      _datafiles.push_back(input_file);
    } else {
      if (_fOutputByDefault) {
        _log << "Error: cannot handle file extension " << extension << "." << std::endl;
      }
      return false;
    }
  }
  return true;
}

Flattener::Flattener(std::ostream& os, std::ostream& log, std::string stdlibDir)
    : _os(os), _log(log), _stdLibDir(std::move(stdlibDir)) {}

Flattener::~Flattener() {
  if (_pEnv != nullptr) {  // ??? TODO
    if (_isFlatzinc) {
      _pEnv->swap();
    }
  }
}

Env* Flattener::multiPassFlatten(const vector<unique_ptr<Pass> >& passes) {
  Env& e = *getEnv();

  Env* pre_env = &e;
  size_t npasses = passes.size();
  pre_env->envi().finalPassNumber = static_cast<unsigned int>(npasses);
  Timer starttime;
  bool verbose = false;
  for (unsigned int i = 0; i < passes.size(); i++) {
    pre_env->envi().currentPassNumber = i;
    if (verbose) {
      _log << "Start pass " << i << ":\n";
    }

    Env* out_env = passes[i]->run(pre_env, _log);
    if (out_env == nullptr) {
      return nullptr;
    }
    if (pre_env != &e && pre_env != out_env) {
      delete pre_env;
    }
    pre_env = out_env;

    if (verbose) {
      _log << "Finish pass " << i << ": " << starttime.stoptime() << "\n";
    }
  }

  return pre_env;
}

class FlattenTimeout {
public:
  FlattenTimeout(unsigned long long int t) { GC::setTimeout(t); }
  ~FlattenTimeout() { GC::setTimeout(0); }
};

void Flattener::flatten(const std::string& modelString, const std::string& modelName) {
  FlattenTimeout flatten_timeout(_fopts.timeout);
  Timer flatten_time;
  _starttime.reset();

  if (_flags.verbose) {
    printVersion(_log);
  }

  if (_filenames.empty() && !_flagSolutionCheckModel.empty()) {
    // Compile solution check model as if it were a normal model
    _filenames.push_back(_flagSolutionCheckModel);
    _flagSolutionCheckModel = "";
  }

  if (_filenames.empty() && !_flags.stdinInput && modelString.empty()) {
    throw Error("Error: no model file given.");
  }

  if (_stdLibDir.empty()) {
    throw Error(
        "Error: unknown minizinc standard library directory.\n"
        "Specify --stdlib-dir on the command line or set the\n"
        "MZN_STDLIB_DIR environment variable.");
  }

  if (!_globalsDir.empty()) {
    _includePaths.insert(_includePaths.begin(),
                         FileUtils::file_path(_stdLibDir + "/" + _globalsDir + "/"));
  }
  _includePaths.push_back(FileUtils::file_path(_stdLibDir + "/std/"));

  for (auto& includePath : _includePaths) {
    if (!FileUtils::directory_exists(includePath)) {
      throw Error("Cannot access include directory " + includePath);
    }
  }

  if (_flagOutputBase.empty()) {
    if (_filenames.empty()) {
      _flagOutputBase = "mznout";
    } else {
      _flagOutputBase = _filenames[0].substr(0, _filenames[0].length() - 4);
    }
  }

  if (_filenames.end() != find(_filenames.begin(), _filenames.end(), _flagOutputFzn) ||
      _datafiles.end() != find(_datafiles.begin(), _datafiles.end(), _flagOutputFzn)) {
    _log << "  WARNING: fzn filename '" << _flagOutputFzn << "' matches an input file, ignoring."
         << endl;
    _flagOutputFzn = "";
  }
  if (_filenames.end() != find(_filenames.begin(), _filenames.end(), _flagOutputOzn) ||
      _datafiles.end() != find(_datafiles.begin(), _datafiles.end(), _flagOutputOzn)) {
    _log << "  WARNING: ozn filename '" << _flagOutputOzn << "' matches an input file, ignoring."
         << endl;
    _flagOutputOzn = "";
  }

  if (_fOutputByDefault) {
    if (_flagOutputFzn.empty()) {
      _flagOutputFzn = _flagOutputBase + ".fzn";
    }
    if (_flagOutputPaths.empty() && _fopts.collectMznPaths) {
      _flagOutputPaths = _flagOutputBase + ".paths";
    }
    if (_flagOutputOzn.empty() && !_flags.noOutputOzn) {
      _flagOutputOzn = _flagOutputBase + ".ozn";
    }
  }

  {
    std::stringstream errstream;

    Model* m;
    _pEnv.reset(new Env(nullptr, _os, _log));
    Env* env = getEnv();

    if (!_flags.compileSolutionCheckModel && !_flagSolutionCheckModel.empty()) {
      // Extract variables to check from solution check model
      if (_flags.verbose) {
        _log << "Parsing solution checker model " << _flagSolutionCheckModel << " ..." << endl;
      }
      bool isCompressedChecker =
          _flagSolutionCheckModel.size() >= 4 &&
          _flagSolutionCheckModel.substr(_flagSolutionCheckModel.size() - 4) == ".mzc";
      std::vector<std::string> smm_model({_flagSolutionCheckModel});
      Model* smm = parse(*env, smm_model, _datafiles, "", "", _includePaths, _isFlatzinc, false,
                         false, _flags.verbose, errstream);
      if (_flags.verbose) {
        _log << " done parsing (" << _starttime.stoptime() << ")" << std::endl;
      }
      if (smm != nullptr) {
        _log << errstream.str();
        errstream.str("");
        std::ostringstream smm_oss;
        std::ostringstream smm_stats_oss;
        Printer p(smm_oss, 0, false);
        p.print(smm);
        Env smm_env(smm);
        GCLock lock;
        vector<TypeError> typeErrors;
        try {
          MiniZinc::typecheck(smm_env, smm, typeErrors, true, false, true);
          if (!typeErrors.empty()) {
            if (!isCompressedChecker) {
              for (auto& typeError : typeErrors) {
                if (_flags.verbose) {
                  _log << std::endl;
                }
                _log << typeError.loc() << ":" << std::endl;
                _log << typeError.what() << ": " << typeError.msg() << std::endl;
              }
            }
            throw Error("multiple type errors");
          }
          for (auto& i : *smm) {
            if (auto* vdi = i->dynamicCast<VarDeclI>()) {
              if (vdi->e()->e() == nullptr) {
                env->envi().checkVars.emplace_back(vdi->e());
              } else if (vdi->e()->ann().contains(constants().ann.rhs_from_assignment)) {
                smm_stats_oss << *vdi;
              }
            }
          }
          smm->compact();
          std::string smm_compressed =
              FileUtils::encode_base64(FileUtils::deflate_string(smm_oss.str()));
          auto* ti = new TypeInst(Location().introduce(), Type::parstring(), nullptr);
          auto* checkString =
              new VarDecl(Location().introduce(), ti, ASTString("_mzn_solution_checker"),
                          new StringLit(Location().introduce(), smm_compressed));
          auto* checkStringI = new VarDeclI(Location().introduce(), checkString);
          env->output()->addItem(checkStringI);

          for (FunctionIterator it = smm->functions().begin(); it != smm->functions().end(); ++it) {
            if (it->id() == "checkStatistics") {
              smm_stats_oss << *it;
              smm_stats_oss << "int: mzn_stats_failures;\n";
              smm_stats_oss << "int: mzn_stats_solutions;\n";
              smm_stats_oss << "int: mzn_stats_nodes;\n";
              smm_stats_oss << "int: mzn_stats_time;\n";
              smm_stats_oss << "output "
                               "[checkStatistics(mzn_stats_failures,mzn_stats_solutions,mzn_stats_"
                               "nodes,mzn_stats_time)];\n";
              std::string smm_stats_compressed =
                  FileUtils::encode_base64(FileUtils::deflate_string(smm_stats_oss.str()));
              auto* ti = new TypeInst(Location().introduce(), Type::parstring(), nullptr);
              auto* checkStatsString =
                  new VarDecl(Location().introduce(), ti, ASTString("_mzn_stats_checker"),
                              new StringLit(Location().introduce(), smm_stats_compressed));
              auto* checkStatsStringI = new VarDeclI(Location().introduce(), checkStatsString);
              env->output()->addItem(checkStatsStringI);
            }
          }
        } catch (TypeError& e) {
          if (isCompressedChecker) {
            _log << "Warning: type error in solution checker model\n";
          } else {
            throw;
          }
        }
      } else {
        if (isCompressedChecker) {
          _log << "Warning: syntax error in solution checker model\n";
        } else {
          _log << errstream.str();
          throw Error("parse error");
        }
      }
    }

    if (_flags.compileSolutionCheckModel) {
      if (!modelString.empty()) {
        throw Error("Cannot compile solution checker model with additional model inputs.");
      }
      if (_flags.stdinInput) {
        throw Error(
            "Cannot compile solution checker model with additional model from standard input.");
      }
      if (_filenames.size() != 1) {
        throw Error("Cannot compile solution checker model with more than one model given.");
      }
    }

    if (!_flagSolutionCheckModel.empty() && _filenames.empty()) {
      throw Error("Cannot run solution checker without model.");
    }

    std::string modelText = modelString;
    if (_flags.stdinInput) {
      std::string input =
          std::string(istreambuf_iterator<char>(std::cin), istreambuf_iterator<char>());
      modelText += input;
    }

    if (_flags.verbose) {
      _log << "Parsing file(s) ";
      for (int i = 0; i < _filenames.size(); ++i) {
        _log << (i == 0 ? "" : ", '") << _filenames[i] << '\'';
      }
      for (const auto& sFln : _datafiles) {
        _log << ", '" << sFln << '\'';
      }
      _log << " ..." << std::endl;
    }
    errstream.str("");
    m = parse(*env, _filenames, _datafiles, modelText, modelName.empty() ? "stdin" : modelName,
              _includePaths, _isFlatzinc, false, false, _flags.verbose, errstream);
    if (!_globalsDir.empty()) {
      _includePaths.erase(_includePaths.begin());
    }
    if (m == nullptr) {
      throw Error(errstream.str());
    }
    _log << errstream.str();
    env->model(m);
    if (_flags.typecheck) {
      if (_flags.verbose) {
        _log << " done parsing (" << _starttime.stoptime() << ")" << std::endl;
      }

      if (_flags.instanceCheckOnly || _flags.modelCheckOnly || _flags.modelInterfaceOnly ||
          _flags.modelTypesOnly) {
        std::ostringstream compiledSolutionCheckModel;
        if (_flags.compileSolutionCheckModel) {
          Printer p(compiledSolutionCheckModel, 0);
          p.print(m);
        }
        GCLock lock;
        vector<TypeError> typeErrors;
        MiniZinc::typecheck(
            *env, m, typeErrors,
            _flags.modelTypesOnly || _flags.modelInterfaceOnly || _flags.modelCheckOnly,
            _flags.allowMultiAssign);
        if (!typeErrors.empty()) {
          for (auto& typeError : typeErrors) {
            if (_flags.verbose) {
              _log << std::endl;
            }
            _log << typeError.loc() << ":" << std::endl;
            _log << typeError.what() << ": " << typeError.msg() << std::endl;
          }
          throw Error("multiple type errors");
        }
        if (_flags.modelInterfaceOnly) {
          MiniZinc::output_model_interface(*env, m, _os, _includePaths);
        }
        if (_flags.modelTypesOnly) {
          MiniZinc::output_model_variable_types(*env, m, _os, _includePaths);
        }
        if (_flags.compileSolutionCheckModel) {
          std::string mzc(FileUtils::deflate_string(compiledSolutionCheckModel.str()));
          mzc = FileUtils::encode_base64(mzc);
          std::string mzc_filename = _filenames[0].substr(0, _filenames[0].size() - 4);
          if (_flags.verbose) {
            _log << "Write solution checker to " << mzc_filename << "\n";
          }
          std::ofstream mzc_f(FILE_PATH(mzc_filename));
          mzc_f << mzc;
          mzc_f.close();
        }
        status = SolverInstance::NONE;
      } else {
        if (_isFlatzinc) {
          GCLock lock;
          vector<TypeError> typeErrors;
          MiniZinc::typecheck(*env, m, typeErrors,
                              _flags.modelCheckOnly || _flags.modelInterfaceOnly,
                              _flags.allowMultiAssign, true);
          if (!typeErrors.empty()) {
            for (auto& typeError : typeErrors) {
              if (_flags.verbose) {
                _log << std::endl;
              }
              _log << typeError.loc() << ":" << std::endl;
              _log << typeError.what() << ": " << typeError.msg() << std::endl;
            }
            throw Error("multiple type errors");
          }
          MiniZinc::register_builtins(*env);
          env->swap();
          populate_output(*env);
        } else {
          if (_flags.verbose) {
            _log << "Flattening ...";
          }

          _fopts.onlyRangeDomains = _flags.onlyRangeDomains;
          _fopts.verbose = _flags.verbose;
          _fopts.outputMode = _flagOutputMode;
          _fopts.outputObjective = _flags.outputObjective;
          _fopts.outputOutputItem = _flags.outputOutputItem;
          _fopts.hasChecker = !_flagSolutionCheckModel.empty();
#ifdef HAS_GECODE
          GecodeOptions gopts;
          gopts.onlyRangeDomains = _flags.onlyRangeDomains;
          gopts.sac = _flags.sac;
          gopts.allowUnboundedVars = _flags.allowUnboundedVars;
          gopts.shave = _flags.shave;
          gopts.printStatistics = _flags.statistics;
          gopts.prePasses = _flagPrePasses;
#endif
          FlatteningOptions pass_opts = _fopts;
          CompilePassFlags cfs;
          cfs.noMIPdomains = _flags.noMIPdomains;
          cfs.verbose = _flags.verbose;
          cfs.statistics = _flags.statistics;
          cfs.optimize = _flags.optimize;
          cfs.chainCompression = _flags.chainCompression;
          cfs.newfzn = _flags.newfzn;
          cfs.werror = _flags.werror;
          cfs.modelCheckOnly = _flags.modelCheckOnly;
          cfs.modelInterfaceOnly = _flags.modelInterfaceOnly;
          cfs.allowMultiAssign = _flags.allowMultiAssign;

          std::vector<unique_ptr<Pass> > managed_passes;

          if (_flags.twoPass) {
            std::string library = _stdLibDir + (_flags.gecode ? "/gecode_presolver/" : "/std/");
            bool differentLibrary = (library != _stdLibDir + "/" + _globalsDir + "/");
            managed_passes.emplace_back(new CompilePass(env, pass_opts, cfs, library, _includePaths,
                                                        true, differentLibrary));
#ifdef HAS_GECODE
            if (_flags.gecode) {
              managed_passes.emplace_back(new GecodePass(&gopts));
            }
#endif
          }
          managed_passes.emplace_back(new CompilePass(env, _fopts, cfs,
                                                      _stdLibDir + "/" + _globalsDir + "/",
                                                      _includePaths, _flags.twoPass, false));

          Env* out_env = multiPassFlatten(managed_passes);
          if (out_env == nullptr) {
            exit(EXIT_FAILURE);
          }

          if (out_env != env) {
            _pEnv.reset(out_env);
          }
          env = out_env;
          if (_flags.verbose) {
            _log << " done (" << _starttime.stoptime() << "),"
                 << " max stack depth " << env->maxCallStack() << std::endl;
          }
        }

        if (_flags.statistics) {
          FlatModelStatistics stats = statistics(*env);
          _os << "% Generated FlatZinc statistics:\n";

          _os << "%%%mzn-stat: paths=" << env->envi().getPathMap().size() << endl;

          if (stats.n_bool_vars != 0) {
            _os << "%%%mzn-stat: flatBoolVars=" << stats.n_bool_vars << endl;
          }
          if (stats.n_int_vars != 0) {
            _os << "%%%mzn-stat: flatIntVars=" << stats.n_int_vars << endl;
          }
          if (stats.n_float_vars != 0) {
            _os << "%%%mzn-stat: flatFloatVars=" << stats.n_float_vars << endl;
          }
          if (stats.n_set_vars != 0) {
            _os << "%%%mzn-stat: flatSetVars=" << stats.n_set_vars << endl;
          }

          if (stats.n_bool_ct != 0) {
            _os << "%%%mzn-stat: flatBoolConstraints=" << stats.n_bool_ct << endl;
          }
          if (stats.n_int_ct != 0) {
            _os << "%%%mzn-stat: flatIntConstraints=" << stats.n_int_ct << endl;
          }
          if (stats.n_float_ct != 0) {
            _os << "%%%mzn-stat: flatFloatConstraints=" << stats.n_float_ct << endl;
          }
          if (stats.n_set_ct != 0) {
            _os << "%%%mzn-stat: flatSetConstraints=" << stats.n_set_ct << endl;
          }

          if (stats.n_reif_ct != 0) {
            _os << "%%%mzn-stat: evaluatedReifiedConstraints=" << stats.n_reif_ct << endl;
          }
          if (stats.n_imp_ct != 0) {
            _os << "%%%mzn-stat: evaluatedHalfReifiedConstraints=" << stats.n_imp_ct << endl;
          }

          if (stats.n_imp_del != 0) {
            _os << "%%%mzn-stat: eliminatedImplications=" << stats.n_imp_del << endl;
          }
          if (stats.n_lin_del != 0) {
            _os << "%%%mzn-stat: eliminatedLinearConstraints=" << stats.n_lin_del << endl;
          }

          /// Objective / SAT. These messages are used by mzn-test.py.
          SolveI* solveItem = env->flat()->solveItem();
          if (solveItem->st() != SolveI::SolveType::ST_SAT) {
            if (solveItem->st() == SolveI::SolveType::ST_MAX) {
              _os << "%%%mzn-stat: method=\"maximize\"" << endl;
            } else {
              _os << "%%%mzn-stat: method=\"minimize\"" << endl;
            }
          } else {
            _os << "%%%mzn-stat: method=\"satisfy\"" << endl;
          }

          _os << "%%%mzn-stat: flatTime=" << flatten_time.s() << endl;
          _os << "%%%mzn-stat-end" << endl << endl;
        }

        if (_flags.outputPathsStdout) {
          if (_flags.verbose) {
            _log << "Printing Paths to stdout ..." << std::endl;
          }
          PathFilePrinter pfp(_os, env->envi());
          pfp.print(env->flat());
          if (_flags.verbose) {
            _log << " done (" << _starttime.stoptime() << ")" << std::endl;
          }
        } else if (!_flagOutputPaths.empty()) {
          if (_flags.verbose) {
            _log << "Printing Paths to '" << _flagOutputPaths << "' ..." << std::flush;
          }
          std::ofstream ofs(FILE_PATH(_flagOutputPaths), ios::out);
          check_io_status(ofs.good(), " I/O error: cannot open fzn output file. ");
          PathFilePrinter pfp(ofs, env->envi());
          pfp.print(env->flat());
          check_io_status(ofs.good(), " I/O error: cannot write fzn output file. ");
          ofs.close();
          if (_flags.verbose) {
            _log << " done (" << _starttime.stoptime() << ")" << std::endl;
          }
        }

        if ((_fopts.collectMznPaths || _flags.twoPass) && !_flags.keepMznPaths) {
          class RemovePathAnnotations : public ItemVisitor {
          public:
            static void removePath(Annotation& a) { a.removeCall(constants().ann.mzn_path); }
            static void vVarDeclI(VarDeclI* vdi) { removePath(vdi->e()->ann()); }
            static void vConstraintI(ConstraintI* ci) { removePath(ci->e()->ann()); }
            static void vSolveI(SolveI* si) {
              removePath(si->ann());
              if (Expression* e = si->e()) {
                removePath(e->ann());
              }
            }
          } removePaths;
          iter_items<RemovePathAnnotations>(removePaths, env->flat());
        }

        if (_flags.outputFznStdout) {
          if (_flags.verbose) {
            _log << "Printing FlatZinc to stdout ..." << std::endl;
          }
          Printer p(_os, 0);
          p.print(env->flat());
          if (_flags.verbose) {
            _log << " done (" << _starttime.stoptime() << ")" << std::endl;
          }
        } else if (!_flagOutputFzn.empty()) {
          if (_flags.verbose) {
            _log << "Printing FlatZinc to '" << _flagOutputFzn << "' ..." << std::flush;
          }
          std::ofstream ofs(FILE_PATH(_flagOutputFzn), ios::out);
          check_io_status(ofs.good(), " I/O error: cannot open fzn output file. ");
          Printer p(ofs, 0);
          p.print(env->flat());
          check_io_status(ofs.good(), " I/O error: cannot write fzn output file. ");
          ofs.close();
          if (_flags.verbose) {
            _log << " done (" << _starttime.stoptime() << ")" << std::endl;
          }
        }
        if (!_flags.noOutputOzn) {
          if (_flags.outputOznStdout) {
            if (_flags.verbose) {
              _log << "Printing .ozn to stdout ..." << std::endl;
            }
            Printer p(_os, 0);
            p.print(env->output());
            if (_flags.verbose) {
              _log << " done (" << _starttime.stoptime() << ")" << std::endl;
            }
          } else if (!_flagOutputOzn.empty()) {
            if (_flags.verbose) {
              _log << "Printing .ozn to '" << _flagOutputOzn << "' ..." << std::flush;
            }
            std::ofstream ofs(FILE_PATH(_flagOutputOzn), std::ios::out);
            check_io_status(ofs.good(), " I/O error: cannot open ozn output file. ");
            Printer p(ofs, 0);
            p.print(env->output());
            check_io_status(ofs.good(), " I/O error: cannot write ozn output file. ");
            ofs.close();
            if (_flags.verbose) {
              _log << " done (" << _starttime.stoptime() << ")" << std::endl;
            }
          }
        }
      }
    } else {  // !flag_typecheck
      Printer p(_os);
      p.print(m);
    }
  }

  if (getEnv()->envi().failed()) {
    status = SolverInstance::UNSAT;
  }

  if (_flags.verbose) {
    size_t mem = GC::maxMem();
    if (mem < 1024) {
      _log << "Maximum memory " << mem << " bytes";
    } else if (mem < 1024 * 1024) {
      _log << "Maximum memory " << mem / 1024 << " Kbytes";
    } else {
      _log << "Maximum memory " << mem / (1024 * 1024) << " Mbytes";
    }
    _log << "." << std::endl;
  }
}

void Flattener::printStatistics(ostream& /*os*/) {}
