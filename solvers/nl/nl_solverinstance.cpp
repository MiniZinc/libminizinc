/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */


 /* This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
#define NOMINMAX     // Need this before all (implicit) include's of Windows.h
#endif

#include <minizinc/solvers/nl/nl_solverinstance.hh>
#include <minizinc/solvers/nl/nl_file.hh>
#include <cstdio>
#include <cstring>
#include <fstream>

#ifdef _WIN32
#undef ERROR
#endif

using namespace std;

namespace MiniZinc {

  /** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** **/
  // Solver Factory

  NL_SolverFactory::NL_SolverFactory(void) {
    SolverConfig sc("org.minizinc.mzn-nl",MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
    sc.name("Generic Non Linear driver");
    sc.mznlibVersion(1);
    sc.description("MiniZinc generic Non Linear solver plugin");
    sc.requiredFlags({"--nl-cmd"});
    sc.mznlib("-Glinear");
    //sc.stdFlags({"-a","-n","-f","-p","-s","-r","-v"});
    sc.tags({"__internal__"});
    SolverConfigs::registerBuiltinSolver(sc);
  }

  string NL_SolverFactory::getDescription(SolverInstanceBase::Options*)  {
    string v = "NL solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }

  string NL_SolverFactory::getVersion(SolverInstanceBase::Options*) {
    return MZN_VERSION_MAJOR;
  }

  string NL_SolverFactory::getId() { return "org.minizinc.mzn-nl"; }

  void NL_SolverFactory::printHelp(ostream& os) {
    os
    << "MZN-NL plugin options: NL_SolverFactory::printHelp TODO" << std::endl
    // << "  --nl-cmd , --nonlinear-cmd <exe>\n     the backend solver filename.\n"
    // << "  -b, --backend, --solver-backend <be>\n     the backend codename. Currently passed to the solver.\n"
    // << "  --fzn-flags <options>, --flatzinc-flags <options>\n     Specify option to be passed to the FlatZinc interpreter.\n"
    // << "  --fzn-flag <option>, --flatzinc-flag <option>\n     As above, but for a single option string that need to be quoted in a shell.\n"
    // << "  -n <n>, --num-solutions <n>\n     An upper bound on the number of solutions to output. The default should be 1.\n"
    // << "  -t <ms>, --solver-time-limit <ms>, --fzn-time-limit <ms>\n     Set time limit (in milliseconds) for solving.\n"
    // << "  --fzn-sigint\n     Send SIGINT instead of SIGTERM.\n"
    // << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions.\n"
    // << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
    // << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
    //                        "     or <this_exe> --fzn ..., --ozn ...\n"
    // << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags instead.\n"
    ;
  }

  SolverInstanceBase::Options* NL_SolverFactory::createOptions(void) {
    return new NLSolverOptions;
  }

  SolverInstanceBase* NL_SolverFactory::doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) {
    return new NLSolverInstance(env, log, opt);
  }

  bool NL_SolverFactory::processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv) {
    cerr << "NL_SolverFactory::processOption TODO: does not process any option for now" << endl;
    return true;
  }


  /** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** **/
  // Solver Instance

  NLSolverInstance::NLSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

  NLSolverInstance::~NLSolverInstance(void) {}

  void NLSolverInstance::processFlatZinc(void) {}

  void NLSolverInstance::resetSolver(void) {}

  SolverInstance::Status
  NLSolverInstance::solve(void) {
    cerr << "Launching NLSolverInstance::solve" << endl;
    // --- --- --- 1) Check options
    // --- --- --- 2) Prepare for the translation
    // --- --- --- 3) Testing of the AST

    // Note: copy the structure of the pretty printer

    for (FunctionIterator it = _fzn->begin_functions(); it != _fzn->end_functions(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }

    for (VarDeclIterator it = _fzn->begin_vardecls(); it != _fzn->end_vardecls(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }

    for (ConstraintIterator it = _fzn->begin_constraints(); it != _fzn->end_constraints(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }

    analyse(_fzn->solveItem());

    cout << nl_file;

    // Back to minizinc with the result
    // var = value;
    // 10 dashes marks: the end of a solution
    // 10 equals signs: the end of the search if successful
    // See sols2out.hh
    getSolns2Out()->feedRawDataChunk("text");

  /*
        Process<Solns2Out> proc(cmd_line, getSolns2Out(), timelimit, sigint);
      int exitStatus = proc.run();
      delete pathsFile;
      return exitStatus == 0 ? getSolns2Out()->status : SolverInstance::ERROR;
      */

    return SolverInstance::NONE;
  }

  // TODO later
  Expression* NLSolverInstance::getSolutionValue(Id* id) { assert(false); return NULL; }

  /** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** **/
  /** Analyse an item
   * An item is a "top node" in the ast.
   * In flatzinc, we can only have the cases 'variable declaration' and 'constraint'.
   */
  void NLSolverInstance::analyse(const Item* i) {
    // Guard
    if (i==NULL) return;

    // Switch on the id of item
    switch (i->iid()) {

      case Item::II_INC: {
        cerr << "Should not happen. (include \"" << i->cast<IncludeI>()->f() << "\")" << endl;
        assert(false);
      } break;

      // Case of the variable declaration.
      // Because it is a variable declaration, the expression associated to the item is necessary a VarDecl.
      // From the VarDecl, we can obtain the type and the RHS expression. Use this to analyse further.
      case Item::II_VD: {
        cerr << "II_VD: Variable Declaration. [";
        const VarDecl& vd = *i->cast<VarDeclI>()->e();
        const TypeInst &ti        = *vd.ti()->cast<TypeInst>();
        const Expression &rhs     = *vd.e();
        nl_file.analyse_vdecl(vd, ti, rhs);
        cerr << "]OK." << endl;
      } break;

      case Item::II_ASN:{
        cerr << "Should not happen." << endl;
        assert(false);
      } break;

      // Case of the constraint.
      // Constraint are expressed through builtin calls.
      // Hence, the expression associated to the item must be a E_CALL.
      case Item::II_CON: {
        cerr << "II_CON: Constraint. [";
        Expression* e = i->cast<ConstraintI>()->e();
        if(e->eid() == Expression::E_CALL){
          const Call& c = *e->cast<Call>();
          nl_file.analyse_constraint(c);
        } else {
          cerr << "Contraint is not a builtin call." << endl;
          assert(false);
        }
        cerr << "]OK." << endl;
      } break;

      // Case of the 'solve' directive
      case Item::II_SOL: {
        const SolveI& si = *i->cast<SolveI>();
        nl_file.analyse_solve(si.st(), si.e());
      } break;

      case Item::II_OUT: {
        cerr << "Should not happen." << endl;
        assert(false);
      } break;

      case Item::II_FUN: {
        cerr << "'FUN' item are ignored." << endl;
      } break;
    }// END OF SWITCH
  }

}
