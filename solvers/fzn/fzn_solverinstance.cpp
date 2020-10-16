/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/parser.hh>
#include <minizinc/pathfileprinter.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/process.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>
#include <minizinc/timer.hh>
#include <minizinc/typecheck.hh>

#include <cstdio>
#include <fstream>

using namespace std;

namespace MiniZinc {

FZNSolverFactory::FZNSolverFactory() {
  SolverConfig sc("org.minizinc.mzn-fzn",
                  MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
  sc.name("Generic FlatZinc driver");
  sc.mznlibVersion(1);
  sc.description("MiniZinc generic FlatZinc solver plugin");
  sc.requiredFlags({"--fzn-cmd"});
  sc.stdFlags({"-a", "-n", "-f", "-p", "-s", "-r", "-v"});
  sc.tags({"__internal__"});
  SolverConfigs::registerBuiltinSolver(sc);
}

string FZNSolverFactory::getDescription(SolverInstanceBase::Options* /*opt*/) {
  string v = "FZN solver plugin, compiled  " __DATE__ "  " __TIME__;
  return v;
}

string FZNSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return MZN_VERSION_MAJOR;
}

string FZNSolverFactory::getId() { return "org.minizinc.mzn-fzn"; }

void FZNSolverFactory::printHelp(ostream& os) {
  os << "MZN-FZN plugin options:" << std::endl
     << "  --fzn-cmd , --flatzinc-cmd <exe>\n     the backend solver filename.\n"
     << "  -b, --backend, --solver-backend <be>\n     the backend codename. Currently passed to "
        "the solver.\n"
     << "  --fzn-flags <options>, --flatzinc-flags <options> --backend-flags <options>\n"
        "     Specify option to be passed to the FlatZinc interpreter.\n"
     << "  --fzn-flag <option>, --flatzinc-flag <option>, --backend-flag\n"
        "     As above, but for a single option string that need to be quoted in a shell.\n"
     << "  -t <ms>, --solver-time-limit <ms>, --fzn-time-limit <ms>\n     Set time limit (in "
        "milliseconds) for solving.\n"
     << "  --fzn-sigint\n     Send SIGINT instead of SIGTERM.\n"
     << "  -n <n>, --num-solutions <n>\n"
     << "    An upper bound on the number of solutions to output for satisfaction problems. The "
        "default should be 1.\n"
     << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions for satisfaction "
        "problems and intermediate solutions for optimization problems.\n"
     << "  -i, --intermediate --intermediate-solutions\n    Print intermediate solutions for "
        "optimisation problems.\n"
     << "  -n-i, --no-intermediate --no-intermediate-solutions\n    Don't print intermediate "
        "solutions for optimisation problems.\n"
     << "  --all-satisfaction\n    Print all solutions for satisfaction problems.\n"
     << "  --disable-all-satisfaction\n    Don't print all solutions for satisfaction problems.\n"
     << "  -n-o <n>, --num-opt-solutions <n>\n"
     << "    An upper bound on the number of optimal solutions to output for optimisation "
        "problems. The default should be 1.\n"
     << "  -a-o, --all-opt, --all-optimal\n     Print all optimal solutions for optimisation "
        "problems.\n"
     << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is "
        "solver-dependent.\n"
     << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
        "     or <this_exe> --fzn ..., --ozn ...\n"
     << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags "
        "instead.\n"
     << "  --cp-profiler <id>,<port>\n    Send search to cp-profiler with given execution ID and "
        "port.\n";
}

SolverInstanceBase::Options* FZNSolverFactory::createOptions() { return new FZNSolverOptions; }

SolverInstanceBase* FZNSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                 SolverInstanceBase::Options* opt) {
  return new FZNSolverInstance(env, log, opt);
}

bool FZNSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                     std::vector<std::string>& argv) {
  auto& _opt = static_cast<FZNSolverOptions&>(*opt);
  CLOParser cop(i, argv);
  string buffer;
  int nn = -1;

  if (cop.getOption("--fzn-cmd --flatzinc-cmd", &buffer)) {
    _opt.fznSolver = buffer;
  } else if (cop.getOption("-b --backend --solver-backend", &buffer)) {
    _opt.backend = buffer;
  } else if (cop.getOption("--fzn-flags --flatzinc-flags --backend-flags", &buffer)) {
    std::vector<std::string> cmdLine = FileUtils::parse_cmd_line(buffer);
    for (auto& s : cmdLine) {
      _opt.fznFlags.push_back(s);
    }
  } else if (cop.getOption("-t --solver-time-limit --fzn-time-limit", &nn)) {
    _opt.fznTimeLimitMilliseconds = nn;
    if (_opt.supportsT) {
      _opt.solverTimeLimitMilliseconds = nn;
      _opt.fznTimeLimitMilliseconds += 1000;  // kill 1 second after solver should have stopped
    }
  } else if (cop.getOption("--fzn-sigint")) {
    _opt.fznSigint = true;
  } else if (cop.getOption("--fzn-needs-paths")) {
    _opt.fznNeedsPaths = true;
  } else if (cop.getOption("--fzn-output-passthrough")) {
    _opt.fznOutputPassthrough = true;
  } else if (cop.getOption("--fzn-flag --flatzinc-flag --backend-flag", &buffer)) {
    _opt.fznFlags.push_back(buffer);
  } else if (_opt.supportsN && cop.getOption("-n --num-solutions", &nn)) {
    _opt.numSols = nn;
  } else if (cop.getOption("-a")) {
    _opt.fznFlags.emplace_back("-a");
  } else if (cop.getOption("-i")) {
    _opt.fznFlags.emplace_back("-i");
  } else if (_opt.supportsNO && cop.getOption("-n-o --num-optimal", &nn)) {
    _opt.numOptimal = (nn != 0);
  } else if (_opt.supportsAO && cop.getOption("-a-o --all-opt --all-optimal")) {
    _opt.allOptimal = true;
  } else if (cop.getOption("-p --parallel", &nn)) {
    if (_opt.supportsP) {
      _opt.parallel = to_string(nn);
    }
  } else if (cop.getOption("-k --keep-files")) {
    // Deprecated option! Does nothing.
  } else if (cop.getOption("-r --seed --random-seed", &buffer)) {
    if (_opt.supportsR) {
      _opt.fznFlags.emplace_back("-r");
      _opt.fznFlags.push_back(buffer);
    }
  } else if (cop.getOption("-s --solver-statistics")) {
    if (_opt.supportsS) {
      _opt.printStatistics = true;
    }
  } else if (cop.getOption("-v --verbose-solving")) {
    _opt.verbose = true;
  } else if (cop.getOption("-f --free-search")) {
    if (_opt.supportsF) {
      _opt.fznFlags.emplace_back("-f");
    }
  } else if (_opt.supportsCpprofiler && cop.getOption("--cp-profiler", &buffer)) {
    _opt.fznFlags.emplace_back("--cp-profiler");
    _opt.fznFlags.push_back(buffer);
  } else {
    for (auto& fznf : _opt.fznSolverFlags) {
      if (fznf.t == MZNFZNSolverFlag::FT_ARG && cop.getOption(fznf.n.c_str(), &buffer)) {
        _opt.fznFlags.push_back(fznf.n);
        _opt.fznFlags.push_back(buffer);
        return true;
      }
      if (fznf.t == MZNFZNSolverFlag::FT_NOARG && cop.getOption(fznf.n.c_str())) {
        _opt.fznFlags.push_back(fznf.n);
        return true;
      }
    }

    return false;
  }
  return true;
}

void FZNSolverFactory::setAcceptedFlags(SolverInstanceBase::Options* opt,
                                        const std::vector<MZNFZNSolverFlag>& flags) {
  auto& _opt = static_cast<FZNSolverOptions&>(*opt);
  _opt.fznSolverFlags.clear();
  for (const auto& f : flags) {
    if (f.n == "-a") {
      _opt.supportsA = true;
    } else if (f.n == "-n") {
      _opt.supportsN = true;
    } else if (f.n == "-f") {
      _opt.supportsF = true;
    } else if (f.n == "-p") {
      _opt.supportsP = true;
    } else if (f.n == "-s") {
      _opt.supportsS = true;
    } else if (f.n == "-r") {
      _opt.supportsR = true;
    } else if (f.n == "-v") {
      _opt.supportsV = true;
    } else if (f.n == "-t") {
      _opt.supportsT = true;
    } else if (f.n == "-i") {
      _opt.supportsI = true;
    } else if (f.n == "-n-o") {
      _opt.supportsNO = true;
    } else if (f.n == "-a-o") {
      _opt.supportsAO = true;
    } else if (f.n == "--cp-profiler") {
      _opt.supportsCpprofiler = true;
    } else {
      _opt.fznSolverFlags.push_back(f);
    }
  }
}

FZNSolverInstance::FZNSolverInstance(Env& env, std::ostream& log,
                                     SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

FZNSolverInstance::~FZNSolverInstance() {}

SolverInstance::Status FZNSolverInstance::solve() {
  auto& opt = static_cast<FZNSolverOptions&>(*_options);
  if (opt.fznSolver.empty()) {
    throw InternalError("No FlatZinc solver specified");
  }
  /// Passing options to solver
  vector<string> cmd_line;
  cmd_line.push_back(opt.fznSolver);
  string sBE = opt.backend;
  bool is_sat = _fzn->solveItem()->st() == SolveI::SolveType::ST_SAT;
  if (!sBE.empty()) {
    cmd_line.emplace_back("-b");
    cmd_line.push_back(sBE);
  }
  for (auto& f : opt.fznFlags) {
    cmd_line.push_back(f);
  }
  if (opt.allOptimal && !is_sat) {
    cmd_line.emplace_back("-a-o");
  }
  if (static_cast<int>(opt.numOptimal) != 1 && !is_sat) {
    cmd_line.emplace_back("-n-o");
    ostringstream oss;
    oss << opt.numOptimal;
    cmd_line.push_back(oss.str());
  }
  if (opt.numSols != 1 && is_sat) {
    cmd_line.emplace_back("-n");
    ostringstream oss;
    oss << opt.numSols;
    cmd_line.push_back(oss.str());
  }
  if (!opt.parallel.empty()) {
    cmd_line.emplace_back("-p");
    ostringstream oss;
    oss << opt.parallel;
    cmd_line.push_back(oss.str());
  }
  if (opt.printStatistics) {
    cmd_line.emplace_back("-s");
  }
  if (opt.solverTimeLimitMilliseconds != 0) {
    cmd_line.emplace_back("-t");
    std::ostringstream oss;
    oss << opt.solverTimeLimitMilliseconds;
    cmd_line.push_back(oss.str());
  }
  if (opt.verbose) {
    if (opt.supportsV) {
      cmd_line.emplace_back("-v");
    }
    std::cerr << "Using FZN solver " << cmd_line[0] << " for solving, parameters: ";
    for (int i = 1; i < cmd_line.size(); ++i) {
      cerr << "" << cmd_line[i] << " ";
    }
    cerr << std::endl;
  }
  int timelimit = opt.fznTimeLimitMilliseconds;
  bool sigint = opt.fznSigint;

  FileUtils::TmpFile fznFile(".fzn");
  std::ofstream os(FILE_PATH(fznFile.name()));
  Printer p(os, 0, true);
  for (FunctionIterator it = _fzn->functions().begin(); it != _fzn->functions().end(); ++it) {
    if (!it->removed()) {
      Item& item = *it;
      p.print(&item);
    }
  }
  for (VarDeclIterator it = _fzn->vardecls().begin(); it != _fzn->vardecls().end(); ++it) {
    if (!it->removed()) {
      Item& item = *it;
      p.print(&item);
    }
  }
  for (ConstraintIterator it = _fzn->constraints().begin(); it != _fzn->constraints().end(); ++it) {
    if (!it->removed()) {
      Item& item = *it;
      p.print(&item);
    }
  }
  p.print(_fzn->solveItem());
  cmd_line.push_back(fznFile.name());

  FileUtils::TmpFile* pathsFile = nullptr;
  if (opt.fznNeedsPaths) {
    pathsFile = new FileUtils::TmpFile(".paths");
    std::ofstream ofs(FILE_PATH(pathsFile->name()));
    PathFilePrinter pfp(ofs, _env.envi());
    pfp.print(_fzn);

    cmd_line.emplace_back("--paths");
    cmd_line.push_back(pathsFile->name());
  }

  if (!opt.fznOutputPassthrough) {
    Process<Solns2Out> proc(cmd_line, getSolns2Out(), timelimit, sigint);
    int exitStatus = proc.run();
    delete pathsFile;
    return exitStatus == 0 ? getSolns2Out()->status : SolverInstance::ERROR;
  }
  Solns2Log s2l(getSolns2Out()->getOutput(), _log);
  Process<Solns2Log> proc(cmd_line, &s2l, timelimit, sigint);
  int exitStatus = proc.run();
  delete pathsFile;
  return exitStatus == 0 ? SolverInstance::NONE : SolverInstance::ERROR;
}

void FZNSolverInstance::processFlatZinc() {}

void FZNSolverInstance::resetSolver() {}

Expression* FZNSolverInstance::getSolutionValue(Id* id) {
  assert(false);
  return nullptr;
}
}  // namespace MiniZinc
