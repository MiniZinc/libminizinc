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

#ifdef _WIN32
#define NOMINMAX     // Need this before all (implicit) include's of Windows.h
#endif

#include <minizinc/solvers/fzn_solverinstance.hh>
#include <cstdio>
#include <fstream>

#include <minizinc/timer.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/pathfileprinter.hh>
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/process.hh>

#ifdef _WIN32
#undef ERROR
#endif

using namespace std;

namespace MiniZinc {
  
  FZN_SolverFactory::FZN_SolverFactory(void) {
    SolverConfig sc("org.minizinc.mzn-fzn",MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
    sc.name("Generic FlatZinc driver");
    sc.mznlibVersion(1);
    sc.description("MiniZinc generic FlatZinc solver plugin");
    sc.requiredFlags({"--fzn-cmd"});
    sc.stdFlags({"-a","-n","-f","-p","-s","-r","-v"});
    sc.tags({"__internal__"});
    SolverConfigs::registerBuiltinSolver(sc);
  }
  
  string FZN_SolverFactory::getDescription(SolverInstanceBase::Options*)  {
    string v = "FZN solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }


  string FZN_SolverFactory::getVersion(SolverInstanceBase::Options*) {
    return MZN_VERSION_MAJOR;
  }

  string FZN_SolverFactory::getId()
  {
    return "org.minizinc.mzn-fzn";
  }
  
  void FZN_SolverFactory::printHelp(ostream& os)
  {
    os
    << "MZN-FZN plugin options:" << std::endl
    << "  --fzn-cmd , --flatzinc-cmd <exe>\n     the backend solver filename.\n"
    << "  -b, --backend, --solver-backend <be>\n     the backend codename. Currently passed to the solver.\n"
    << "  --fzn-flags <options>, --flatzinc-flags <options>\n     Specify option to be passed to the FlatZinc interpreter.\n"
    << "  --fzn-flag <option>, --flatzinc-flag <option>\n     As above, but for a single option string that need to be quoted in a shell.\n"
    << "  -n <n>, --num-solutions <n>\n     An upper bound on the number of solutions to output. The default should be 1.\n"
    << "  -t <ms>, --solver-time-limit <ms>, --fzn-time-limit <ms>\n     Set time limit (in milliseconds) for solving.\n"
    << "  --fzn-sigint\n     Send SIGINT instead of SIGTERM.\n"
    << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions.\n"
    << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
    << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
                           "     or <this_exe> --fzn ..., --ozn ...\n"
    << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags instead.\n"
    << "  --use-stdin\n    Pass flatzinc to solver via stdin instead of temp file.\n"
    ;
  }

  SolverInstanceBase::Options* FZN_SolverFactory::createOptions(void) {
    return new FZNSolverOptions;
  }

  SolverInstanceBase* FZN_SolverFactory::doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) {
    return new FZNSolverInstance(env, log, opt);
  }

  bool FZN_SolverFactory::processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv)
  {
    FZNSolverOptions& _opt = static_cast<FZNSolverOptions&>(*opt);
    CLOParser cop( i, argv );
    string buffer;
    int nn=-1;
    
    if ( cop.getOption( "--fzn-cmd --flatzinc-cmd", &buffer) ) {
      _opt.fzn_solver = buffer;
    } else if ( cop.getOption( "-b --backend --solver-backend", &buffer) ) {
      _opt.backend = buffer;
    } else if ( cop.getOption( "--fzn-flags --flatzinc-flags", &buffer) ) {
      std::vector<std::string> cmdLine = FileUtils::parseCmdLine(buffer);
      for (auto& s : cmdLine) {
        _opt.fzn_flags.push_back(s);
      }
    } else if ( cop.getOption( "-t --solver-time-limit --fzn-time-limit", &nn) ) {
      _opt.fzn_time_limit_ms = nn;
      if (_opt.supports_t) {
        _opt.solver_time_limit_ms = nn;
        _opt.fzn_time_limit_ms += 1000; // kill 1 second after solver should have stopped
      }
    } else if ( cop.getOption( "--fzn-sigint") ) {
      _opt.fzn_sigint = true;
    } else if ( cop.getOption( "--fzn-needs-paths") ) {
      _opt.fzn_needs_paths = true;
    } else if ( cop.getOption( "--fzn-output-passthrough") ) {
      _opt.fzn_output_passthrough = true;
    } else if ( cop.getOption( "--fzn-flag --flatzinc-flag", &buffer) ) {
      _opt.fzn_flags.push_back(buffer);
    } else if ( _opt.supports_n && cop.getOption( "-n --num-solutions", &nn) ) {
      _opt.numSols = nn;
    } else if ( _opt.supports_a && cop.getOption( "-a --all --all-solns --all-solutions") ) {
      _opt.allSols = true;
    } else if ( cop.getOption( "-p --parallel", &nn) ) {
      if (_opt.supports_p)
        _opt.parallel = to_string(nn);
    } else if ( cop.getOption( "-k --keep-files" ) ) {
    } else if ( cop.getOption( "-r --seed --random-seed", &buffer) ) {
      if (_opt.supports_r) {
        _opt.fzn_flags.push_back("-r");
        _opt.fzn_flags.push_back(buffer);
      }
    } else if ( cop.getOption( "-s --solver-statistics") ) {
      if (_opt.supports_s) {
        _opt.printStatistics = true;
      }
    } else if ( cop.getOption( "-v --verbose-solving") ) {
      _opt.verbose = true;
    } else if ( cop.getOption( "-f --free-search") ) {
      if (_opt.supports_f)
        _opt.fzn_flags.push_back("-f");
    } else if ( cop.getOption( "--use-stdin") ) {
      _opt.fzn_use_stdin = true;
    } else {
      for (auto& fznf : _opt.fzn_solver_flags) {
        if (fznf.t==MZNFZNSolverFlag::FT_ARG && cop.getOption(fznf.n.c_str(), &buffer)) {
          _opt.fzn_flags.push_back(fznf.n);
          _opt.fzn_flags.push_back(buffer);
          return true;
        } else if (fznf.t==MZNFZNSolverFlag::FT_NOARG && cop.getOption(fznf.n.c_str())) {
          _opt.fzn_flags.push_back(fznf.n);
          return true;
        }
      }

      return false;
    }
    return true;
  }
  
  void FZN_SolverFactory::setAcceptedFlags(SolverInstanceBase::Options* opt, const std::vector<MZNFZNSolverFlag>& flags) {
    FZNSolverOptions& _opt = static_cast<FZNSolverOptions&>(*opt);
    _opt.fzn_solver_flags.clear();
    for (auto& f : flags) {
      if (f.n=="-a") {
        _opt.supports_a = true;
      } else if (f.n=="-n") {
        _opt.supports_n = true;
      } else if (f.n=="-f") {
        _opt.supports_f = true;
      } else if (f.n=="-p") {
        _opt.supports_p = true;
      } else if (f.n=="-s") {
        _opt.supports_s = true;
      } else if (f.n=="-r") {
        _opt.supports_r = true;
      } else if (f.n=="-v") {
        _opt.supports_v = true;
      } else if (f.n=="-t") {
        _opt.supports_t = true;
      } else {
        _opt.fzn_solver_flags.push_back(f);
      }
    }
  }

  class FZN_Provider : public InputProvider {

  protected:
    FZNSolverInstance* _inst;
    Printer p;

  public:
    explicit FZN_Provider(FZNSolverInstance* inst) : _inst(inst), p(*getStream(), 0, true) {};

    void provide() override {
      _inst->printModel(p);
    }

  };

  FZNSolverInstance::FZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

  FZNSolverInstance::~FZNSolverInstance(void) {}

  SolverInstance::Status
  FZNSolverInstance::solve(void) {
    FZNSolverOptions& opt = static_cast<FZNSolverOptions&>(*_options);
    if (opt.fzn_solver.empty()) {
      throw InternalError("No FlatZinc solver specified");
    }
    /// Passing options to solver
    vector<string> cmd_line;
    cmd_line.push_back( opt.fzn_solver );
    string sBE = opt.backend;
    if ( sBE.size() ) {
      cmd_line.push_back( "-b" );
      cmd_line.push_back( sBE );
    }
    for (auto& f : opt.fzn_flags) {
      cmd_line.push_back( f );
    }
    if ( opt.numSols != 1 ) {
      cmd_line.push_back( "-n" );
      ostringstream oss;
      oss << opt.numSols;
      cmd_line.push_back( oss.str() );
    }
    if ( opt.allSols ) {
      cmd_line.push_back( "-a" );
    }
    if ( opt.parallel.size() ) {
      cmd_line.push_back( "-p" );
      ostringstream oss;
      oss << opt.parallel;
      cmd_line.push_back( oss.str() );
    }
    if (opt.printStatistics) {
      cmd_line.push_back( "-s" );
    }
    if (opt.solver_time_limit_ms != 0) {
      cmd_line.push_back( "-t" );
      std::ostringstream oss;
      oss << opt.solver_time_limit_ms;
      cmd_line.push_back( oss.str() );
    }
    if (opt.verbose) {
      if (opt.supports_v)
        cmd_line.push_back( "-v" );
      std::cerr << "Using FZN solver " << cmd_line[0]
        << " for solving, parameters: ";
      for ( int i=1; i<cmd_line.size(); ++i )
        cerr << "" << cmd_line[i] << " ";
      cerr << std::endl;
    }
    int timelimit = opt.fzn_time_limit_ms;
    bool sigint = opt.fzn_sigint;

    FileUtils::TmpFile* pathsFile = NULL;
    if(opt.fzn_needs_paths) {
      pathsFile = new FileUtils::TmpFile(".paths");
      std::ofstream ofs(pathsFile->name());
      PathFilePrinter pfp(ofs, _env.envi());
      pfp.print(_fzn);

      cmd_line.push_back("--paths");
      cmd_line.push_back(pathsFile->name());
    }

    FZN_Provider* input = nullptr;
    FileUtils::TmpFile* fznFile = nullptr;
    if (opt.fzn_use_stdin) {
      input = new FZN_Provider(this);
    } else {
      fznFile = new FileUtils::TmpFile(".fzn");
      std::ofstream os(fznFile->name());
      Printer p(os, 0, true);
      printModel(p);
      cmd_line.push_back(fznFile->name());
    }

    if(!opt.fzn_output_passthrough) {
      Process<Solns2Out> proc(cmd_line, getSolns2Out(), timelimit, sigint, input);
      int exitStatus = proc.run();
      delete fznFile;
      delete pathsFile;
      return exitStatus == 0 ? getSolns2Out()->status : SolverInstance::ERROR;
    } else {
      Solns2Log s2l(getSolns2Out()->getOutput(), _log);
      Process<Solns2Log> proc(cmd_line, &s2l, timelimit, sigint, input);
      int exitStatus = proc.run();
      delete fznFile;
      delete pathsFile;
      return exitStatus==0 ? SolverInstance::NONE : SolverInstance::ERROR;
    }
  }

  void FZNSolverInstance::processFlatZinc(void) {}

  void FZNSolverInstance::resetSolver(void) {}

  Expression*
  FZNSolverInstance::getSolutionValue(Id* id) {
    assert(false);
    return NULL;
  }

  void FZNSolverInstance::printModel(Printer p) {

    for (FunctionIterator it = _fzn->begin_functions(); it != _fzn->end_functions(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    for (VarDeclIterator it = _fzn->begin_vardecls(); it != _fzn->end_vardecls(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    for (ConstraintIterator it = _fzn->begin_constraints(); it != _fzn->end_constraints(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        p.print(&item);
      }
    }
    p.print(_fzn->solveItem());

  }

}
