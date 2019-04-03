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

#include <cstdio>
#include <cstring>
#include <fstream>

#include <minizinc/process.hh>
#include <minizinc/file_utils.hh>

#include <minizinc/solvers/nl/nl_solverinstance.hh>
#include <minizinc/solvers/nl/nl_file.hh>
#include <minizinc/solvers/nl/nl_solreader.hh>


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
    //sc.mznlib("-Glinear");
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
    // << "  --nl-flags <options>\n     Specify option to be passed to the NL solver.\n"
    // << "  --nl-flag <option>\n     As above, but for a single option string that need to be quoted in a shell.\n"
    // << "  -t <ms>, --solver-time-limit <ms>, --fzn-time-limit <ms>\n     Set time limit (in milliseconds) for solving.\n"
    // << "  --nl-sigint\n     Send SIGINT instead of SIGTERM.\n"
    // << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
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
    NLSolverOptions& _opt = static_cast<NLSolverOptions&>(*opt);
    CLOParser cop( i, argv );
    string buffer;
    int nn=-1;
    if ( cop.getOption( "--nl-cmd --nonlinear-cmd", &buffer) ) {
      _opt.nl_solver = buffer;
    } else {
      for (auto& fznf : _opt.nl_solver_flags) {
        if (fznf.t==MZNFZNSolverFlag::FT_ARG && cop.getOption(fznf.n.c_str(), &buffer)) {
          _opt.nl_flags.push_back(fznf.n);
          _opt.nl_flags.push_back(buffer);
          return true;
        } else if (fznf.t==MZNFZNSolverFlag::FT_NOARG && cop.getOption(fznf.n.c_str())) {
          _opt.nl_flags.push_back(fznf.n);
          return true;
        }
      }
      return false;
    }
    return true;
  }


  /** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** **/
  // Solver Instance

  NLSolverInstance::NLSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {
      
      file_mzn = _env.envi().orig_model ? _env.envi().orig_model->filepath().str() : _env.envi().model->filepath().str();
      file_sub = file_mzn.substr(0,file_mzn.find_last_of('.'));
      file_nl  = file_sub+".nl";
      file_sol = file_sub+".sol";

      nl_file = NLFile(env.envi().orig_model ? env.envi().orig_model->filename().str() : env.envi().model->filename().str());

    }

  NLSolverInstance::~NLSolverInstance(void) {}

  void NLSolverInstance::processFlatZinc(void) {}

  void NLSolverInstance::resetSolver(void) {}

  SolverInstance::Status
  NLSolverInstance::solve(void) {
    
    FileUtils::TmpDir tmpdir;
    file_nl = tmpdir.name()+"/model.nl";
    file_sol = tmpdir.name()+"/model.sol";
    
    NLSolverOptions& opt = static_cast<NLSolverOptions&>(*_options);
    DEBUG_MSG("Launching NLSolverInstance::solve" << endl);  

    // --- --- --- 1) Check options
    // --- --- --- 2) Prepare for the translation
    // --- --- --- 3) Testing of the AST

    // Note: copy the structure of the pretty printer

    // Analyse the variable declarations
    for (VarDeclIterator it = _fzn->begin_vardecls(); it != _fzn->end_vardecls(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }

    // Analyse the contraints
    for (ConstraintIterator it = _fzn->begin_constraints(); it != _fzn->end_constraints(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }


    // --- --- --- Prepare the files
    std::ofstream outfile(file_nl);
    outfile.precision(numeric_limits<double>::digits10 + 2);

    // Use to talk back to minizinc
    auto* out = getSolns2Out();

    // Manage status
    int exitStatus = -1;

    // All the NL iperations in one try/catch
    try {

      // Analyse the goal
      analyse(_fzn->solveItem());

      // Phase 2
      nl_file.phase_2();

      // Print to the files
      nl_file.print_on(outfile);

      // --- --- --- Call the solver
      NLSolns2Out s2o = NLSolns2Out(out, nl_file);
      vector<string> cmd_line;
      if (opt.nl_solver.empty()) {
        throw InternalError("No NL solver specified");
      }
      cmd_line.push_back(opt.nl_solver);
      cmd_line.push_back(file_nl);
      cmd_line.push_back("-AMPL");
      Process<NLSolns2Out> proc(cmd_line, &s2o, 0, true);
      exitStatus = proc.run();

      // Parse the result
      s2o.parse_sol(file_sol);

    } catch (const NLException e){
      out->getLog() << e.what();
      exitStatus = -2;
    }

    outfile.close();


    return exitStatus == 0 ? out->status : Status::ERROR;
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
        should_not_happen("include \"" << i->cast<IncludeI>()->f() << "\")");
      } break;

      // Case of the variable declaration.
      // Because it is a variable declaration, the expression associated to the item is necessary a VarDecl.
      // From the VarDecl, we can obtain the type and the RHS expression. Use this to analyse further.
      case Item::II_VD: {
        DEBUG_MSG("II_VD: Variable Declaration. [");

        const VarDecl& vd = *i->cast<VarDeclI>()->e();
        const TypeInst &ti        = *vd.ti()->cast<TypeInst>();
        const Expression &rhs     = *vd.e();
        nl_file.add_vdecl(vd, ti, rhs);

        DEBUG_MSG("]OK." << endl);
      } break;

      case Item::II_ASN:{
        should_not_happen("item II_ASN should not be present in flatzinc.");
      } break;

      // Case of the constraint.
      // Constraint are expressed through builtin calls.
      // Hence, the expression associated to the item must be a E_CALL.
      case Item::II_CON: {
        DEBUG_MSG("II_CON: Constraint. [");
        Expression* e = i->cast<ConstraintI>()->e();
        if(e->eid() == Expression::E_CALL){
          const Call& c = *e->cast<Call>();
          DEBUG_MSG(c.id() << " ");
          nl_file.analyse_constraint(c);
        } else {
          DEBUG_MSG("Contraint is not a builtin call." << endl);
          assert(false);
        }
        DEBUG_MSG("]OK." << endl);
      } break;

      // Case of the 'solve' directive
      case Item::II_SOL: {
        const SolveI& si = *i->cast<SolveI>();
        nl_file.add_solve(si.st(), si.e());
      } break;

      case Item::II_OUT: {
        should_not_happen("Item II_OUT should not be present in flatzinc.");
      } break;

      case Item::II_FUN: {
        // TODO
        DEBUG_MSG("'FUN' item are ignored." << endl);
      } break;
    }// END OF SWITCH
  }

}
