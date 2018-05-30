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
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/process.hh>

using namespace std;

namespace MiniZinc {
  
  FZN_SolverFactory::FZN_SolverFactory(void) {
    SolverConfig sc("org.minizinc.mzn-fzn",MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
    sc.name("Generic FlatZinc driver");
    sc.mznlibVersion(1);
    sc.description("MiniZinc generic FlatZinc solver plugin");
    sc.requiredFlags({"--fzn-cmd"});
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
    << "  --fzn-time-limit <ms>\n     Set a hard timelimit that overrides those set for the solver using --fzn-flag(s).\n"
    << "  --fzn-sigint\n     Send SIGINT instead of SIGTERM.\n"
    << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions.\n"
    << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
    << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
                           "     or <this_exe> --fzn ..., --ozn ...\n"
    << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags instead.\n"
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
    double dd;
    int nn=-1;
    
    if ( cop.getOption( "--fzn-cmd --flatzinc-cmd", &buffer) ) {
      _opt.fzn_solver = buffer;
    } else if ( cop.getOption( "-b --backend --solver-backend", &buffer) ) {
      _opt.backend = buffer;
    } else if ( cop.getOption( "--fzn-flags --flatzinc-flags", &buffer) ) {
      string old = _opt.fzn_flags;
      old += ' ';
      old += buffer;
      _opt.fzn_flags = old;
    } else if ( cop.getOption( "--fzn-time-limit", &nn) ) {
      _opt.fzn_time_limit_ms = nn;
    } else if ( cop.getOption( "--fzn-sigint") ) {
      _opt.fzn_sigint = true;
    } else if ( cop.getOption( "--fzn-flag --flatzinc-flag", &buffer) ) {
      string old = _opt.fzn_flag;
      old += " \"";
      old += buffer;
      old += "\" ";
      _opt.fzn_flag = old;
    } else if ( cop.getOption( "-n --num-solutions", &nn) ) {
      _opt.numSols = nn;
    } else if ( cop.getOption( "-a --all --all-solns --all-solutions") ) {
      _opt.allSols = true;
    } else if ( cop.getOption( "-p --parallel", &nn) ) {
      _opt.parallel = nn;
    } else if ( cop.getOption( "-k --keep-files" ) ) {
    } else if ( cop.getOption( "-r --seed --random-seed", &dd) ) {
    } else {
      return false;
    }
    return true;
  }
  


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
    string sFlags = opt.fzn_flags;
    if ( sFlags.size() )
      cmd_line.push_back( sFlags );
    string sFlagQuoted = opt.fzn_flag;
    if ( sFlagQuoted.size() )
      cmd_line.push_back( sFlagQuoted );
    if ( opt.numSols != 1 ) {
      ostringstream oss;
      oss << "-n " << opt.numSols;
      cmd_line.push_back( oss.str() );
    }
    if ( opt.allSols ) {
      cmd_line.push_back( "-a" );
    }
    if ( opt.parallel.size() ) {
      ostringstream oss;
      oss << "-p " << opt.parallel;
      cmd_line.push_back( oss.str() );
    }
    if (opt.printStatistics) {
      cmd_line.push_back( "-s" );
    }
    if (opt.verbose) {
      cmd_line.push_back( "-v" );
      std::cerr << "Using FZN solver " << cmd_line[0]
        << " for solving, parameters: ";
      for ( int i=1; i<cmd_line.size(); ++i )
        cerr << "" << cmd_line[i] << " ";
      cerr << std::endl;
    }
    int timelimit = opt.fzn_time_limit_ms;
    bool sigint = opt.fzn_sigint;
    
    FileUtils::TmpFile fznFile(".fzn");
    std::ofstream os(fznFile.name());
    Printer p(os, 0, true);
    for (Model::iterator it = _fzn->begin(); it != _fzn->end(); ++it) {
      if(!(*it)->removed() && !(*it)->isa<IncludeI>()) {
        Item* item = *it;
        p.print(item);
      }
    }
    cmd_line.push_back(fznFile.name());
    Process<Solns2Out> proc(cmd_line, getSolns2Out(), timelimit, sigint);
    proc.run();

    return getSolns2Out()->status;
  }

  void FZNSolverInstance::processFlatZinc(void) {}

  void FZNSolverInstance::resetSolver(void) {}

  Expression*
  FZNSolverInstance::getSolutionValue(Id* id) {
    assert(false);
    return NULL;
  }
}
