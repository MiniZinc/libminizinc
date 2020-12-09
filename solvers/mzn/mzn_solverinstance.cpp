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

#include <minizinc/process.hh>
#include <minizinc/solvers/mzn_solverinstance.hh>
#include <minizinc/timer.hh>

#include <cstdio>
#include <fstream>

using namespace std;

namespace MiniZinc {

MZNSolverFactory::MZNSolverFactory() {
  SolverConfig sc("org.minizinc.mzn-mzn",
                  MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
  sc.name("Generic MiniZinc driver");
  sc.mznlibVersion(1);
  sc.description("MiniZinc generic MiniZinc solver plugin");
  sc.requiredFlags({"-m"});
  sc.tags({"__internal__"});
  sc.supportsFzn(false);
  sc.supportsMzn(true);
  sc.needsSolns2Out(false);
  SolverConfigs::registerBuiltinSolver(sc);
}

string MZNSolverFactory::getDescription(SolverInstanceBase::Options* /*opt*/) {
  string v = "MZN solver plugin, compiled  " __DATE__ "  " __TIME__;
  return v;
}

string MZNSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return MZN_VERSION_MAJOR;
}

string MZNSolverFactory::getId() { return "org.minizinc.mzn-mzn"; }

void MZNSolverFactory::printHelp(ostream& os) {
  os << "MZN-MZN plugin options:" << std::endl
     << "  -m, --minizinc-cmd <exe>\n     the backend solver filename.\n"
     << "  --mzn-flags <options>, --minizinc-flags <options>, --backend-flags <options>\n"
        "     Specify option to be passed to the MiniZinc interpreter.\n"
     << "  --mzn-flag <option>, --minizinc-flag <option>, --backend-flag <option>\n"
        "     As above, but for a single option string that need to be quoted in a shell.\n"
     << "  -t <ms>, --solver-time-limit <ms>, --mzn-time-limit <ms>\n"
        "     Set time limit for solving.\n"
     << "  --mzn-sigint\n     Send SIGINT instead of SIGTERM.\n";
}

SolverInstanceBase::Options* MZNSolverFactory::createOptions() { return new MZNSolverOptions; }

SolverInstanceBase* MZNSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                 SolverInstanceBase::Options* opt) {
  return new MZNSolverInstance(env, log, opt);
}

void MZNSolverFactory::setAcceptedFlags(SolverInstanceBase::Options* opt,
                                        const std::vector<MZNFZNSolverFlag>& flags) {
  auto& _opt = static_cast<MZNSolverOptions&>(*opt);
  _opt.mznSolverFlags.clear();
  for (const auto& f : flags) {
    if (f.n == "-t") {
      _opt.supportsT = true;
    } else {
      _opt.mznSolverFlags.push_back(f);
    }
  }
}

bool MZNSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                     std::vector<std::string>& argv,
                                     const std::string& workingDir) {
  auto& _opt = static_cast<MZNSolverOptions&>(*opt);
  CLOParser cop(i, argv);
  string buffer;
  int nn = -1;

  if (cop.getOption("-m --minizinc-cmd", &buffer)) {
    _opt.mznSolver = buffer;
  } else if (cop.getOption("--mzn-flags --minizinc-flags --backend-flags", &buffer)) {
    std::vector<std::string> cmdLine = FileUtils::parse_cmd_line(buffer);
    for (auto& s : cmdLine) {
      _opt.mznFlags.push_back(s);
    }
  } else if (cop.getOption("-t --solver-time-limit --mzn-time-limit", &nn)) {
    _opt.mznTimeLimitMilliseconds = nn;
    if (_opt.supportsT) {
      _opt.solverTimeLimitMilliseconds = nn;
      _opt.mznTimeLimitMilliseconds += 1000;  // kill 1 second after solver should have stopped
    }
  } else if (cop.getOption("--mzn-sigint")) {
    _opt.mznSigint = true;
  } else if (cop.getOption("--mzn-flag --minizinc-flag --backend-flag", &buffer)) {
    _opt.mznFlags.push_back(buffer);
  } else if (cop.getOption("--solver-statistics")) {
    _opt.printStatistics = true;
  } else if (cop.getOption("--verbose-solving")) {
    _opt.verbose = true;
  } else {
    for (auto& mznf : _opt.mznSolverFlags) {
      if (mznf.t == MZNFZNSolverFlag::FT_ARG && cop.getOption(mznf.n.c_str(), &buffer)) {
        _opt.mznFlags.push_back(mznf.n);
        _opt.mznFlags.push_back(buffer);
        return true;
      }
      if (mznf.t == MZNFZNSolverFlag::FT_NOARG && cop.getOption(mznf.n.c_str())) {
        _opt.mznFlags.push_back(mznf.n);
        return true;
      }
    }
    std::string input_file(argv[i]);
    if (input_file.length() <= 4) {
      return false;
    }
    size_t last_dot = input_file.find_last_of('.');
    if (last_dot == string::npos) {
      return false;
    }
    std::string extension = input_file.substr(last_dot, string::npos);
    if (extension == ".mzn" || extension == ".mzc" || extension == ".fzn" || extension == ".dzn" ||
        extension == ".json") {
      _opt.mznFlags.push_back(input_file);
    } else {
      return false;
    }
  }
  return true;
}

MZNSolverInstance::MZNSolverInstance(Env& env, std::ostream& log,
                                     SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options) {}

MZNSolverInstance::~MZNSolverInstance() {}

SolverInstance::Status MZNSolverInstance::solve() {
  auto& opt = static_cast<MZNSolverOptions&>(*_options);
  if (opt.mznSolver.empty()) {
    throw InternalError("No MiniZinc solver specified");
  }
  /// Passing options to solver
  vector<string> cmd_line;
  cmd_line.push_back(opt.mznSolver);
  for (auto& f : opt.mznFlags) {
    cmd_line.push_back(f);
  }
  if (opt.printStatistics) {
    cmd_line.emplace_back("-s");
  }
  if (opt.verbose) {
    cmd_line.emplace_back("-v");
    _log << "Using MZN solver " << cmd_line[0] << " for solving, parameters: ";
    for (int i = 1; i < cmd_line.size(); ++i) {
      _log << "" << cmd_line[i] << " ";
    }
    _log << std::endl;
  }
  if (opt.solverTimeLimitMilliseconds != 0) {
    cmd_line.emplace_back("-t");
    std::ostringstream oss;
    oss << opt.solverTimeLimitMilliseconds;
    cmd_line.push_back(oss.str());
  }
  int timelimit = opt.mznTimeLimitMilliseconds;
  bool sigint = opt.mznSigint;
  Solns2Log s2l(getSolns2Out()->getOutput(), _log);
  Process<Solns2Log> proc(cmd_line, &s2l, timelimit, sigint);
  int exitCode = proc.run();

  return exitCode == 0 ? SolverInstance::UNKNOWN : SolverInstance::ERROR;
}

void MZNSolverInstance::processFlatZinc() {}

void MZNSolverInstance::resetSolver() {}

}  // namespace MiniZinc
