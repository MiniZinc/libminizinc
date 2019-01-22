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
#include <fstream>

// #include <minizinc/timer.hh>
// #include <minizinc/prettyprinter.hh>
// #include <minizinc/pathfileprinter.hh>
// #include <minizinc/parser.hh>
// #include <minizinc/typecheck.hh>
// #include <minizinc/builtins.hh>
// #include <minizinc/eval_par.hh>
// #include <minizinc/process.hh>

#ifdef _WIN32
#undef ERROR
#endif

using namespace std;

namespace MiniZinc {
  
  NL_SolverFactory::NL_SolverFactory(void) {
    SolverConfig sc("org.minizinc.mzn-nl",MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
    sc.name("Generic Non Linear driver");
    sc.mznlibVersion(1);
    sc.description("MiniZinc generic Non Linear solver plugin");
    sc.requiredFlags({"--nl-cmd"});
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

  string NL_SolverFactory::getId()
  {
    return "org.minizinc.mzn-nl";
  }
  
  void NL_SolverFactory::printHelp(ostream& os)
  {
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

  bool NL_SolverFactory::processOption(SolverInstanceBase::Options* opt, int& i, std::vector<std::string>& argv)
  {
    cerr << "NL_SolverFactory::processOption TODO: does not process any option for now" << endl;
    return true;
  }  


  NLSolverInstance::NLSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

  NLSolverInstance::~NLSolverInstance(void) {}

  SolverInstance::Status
  NLSolverInstance::solve(void) {
    cerr << "Launching NLSolverInstance::solve" << endl;
    // --- --- --- 1) Check options
    // --- --- --- 2) Prepare for the translation
    NLFile file; // = new NLFile();
    file.print_on(cout);
    
    // --- --- --- 3) Testing of the AST
    cerr << "AST tests" << endl;

    for (VarDeclIterator it = _fzn->begin_vardecls(); it != _fzn->end_vardecls(); ++it) {
      if(!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    } // END FOR

    cerr << "End of NLSolverInstance::solve" << endl;
    return SolverInstance::NONE;
  }

/*  FileUtils::TmpFile fznFile(".fzn");
    std::ofstream os(fznFile.name());
    Printer p(os, 0, true);
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
    cmd_line.push_back(fznFile.name());

    FileUtils::TmpFile* pathsFile = NULL;
    if(opt.fzn_needs_paths) {
      pathsFile = new FileUtils::TmpFile(".paths");
      std::ofstream ofs(pathsFile->name());
      PathFilePrinter pfp(ofs, _env.envi());
      pfp.print(_fzn);

      cmd_line.push_back("--paths");
      cmd_line.push_back(pathsFile->name());
    }

    if(!opt.fzn_output_passthrough) {
      Process<Solns2Out> proc(cmd_line, getSolns2Out(), timelimit, sigint);
      int exitStatus = proc.run();
      delete pathsFile;
      return exitStatus == 0 ? getSolns2Out()->status : SolverInstance::ERROR;
    } else {
      Solns2Log s2l(getSolns2Out()->getOutput(), _log);
      Process<Solns2Log> proc(cmd_line, &s2l, timelimit, sigint);
      int exitStatus = proc.run();
      delete pathsFile;
      return exitStatus==0 ? SolverInstance::NONE : SolverInstance::ERROR;
    }
  }
  */

  void NLSolverInstance::analyse(const Item* i) {
    // Guard
    if (i==NULL) return;

    // Switch on the id of item
    switch (i->iid()) {
      case Item::II_INC: {
        cerr << "Inclusion not implemented. (include \"" << i->cast<IncludeI>()->f() << "\")" << endl;
        assert(false);
      } break;

      case Item::II_VD: {
        Expression* e = i->cast<VarDeclI>()->e();
        analyse(e);
      } break;

      case Item::II_ASN:{
        cerr << "Assignement not implemented." << endl;
        assert(false);
      } break;

      case Item::II_CON: {
        cerr << "Constraint not implemented." << endl;
        assert(false);
      } break;

      case Item::II_SOL: {
        cerr << "Solve not implemented." << endl;
        assert(false);
      } break;

      case Item::II_OUT: {
        cerr << "Output not implemented." << endl;
        assert(false);
      } break;

      case Item::II_FUN: {
        cerr << "Function/predicate/test not implemented." << endl;
        assert(false);        
      } break;
    }// END OF SWITCH
  }


  void NLSolverInstance::analyse(const Expression* e) {
    // Guard
    if (e==NULL) return;

    // Dispatch on expression type
    switch (e->eid()) {

      // --- --- --- Literals
      case Expression::E_INTLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);
      } break;

      case Expression::E_FLOATLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);
      } break;

      case Expression::E_SETLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_BOOLLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_STRINGLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      /// --- --- --- Expressions

      case Expression::E_ID: { // Identifier
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

      case Expression::E_TIID: { // Type-inst identifier
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

      case Expression::E_ANON: { // Annotation
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

      case Expression::E_ARRAYLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_ARRAYACCESS: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_COMP:{ // Comprehension
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

      case Expression::E_ITE:{ // If-then-else expression
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

      case Expression::E_BINOP: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;
      
      case Expression::E_UNOP: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_CALL: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;


      case Expression::E_VARDECL: {
        // --- --- ---
        const VarDecl &vd = *e->cast<VarDecl>();
        const TypeInst &ti = *vd.ti()->cast<TypeInst>();
        const Expression &rhs = *vd.e();

        // --- --- --- Get the name
        stringstream os; 
        if (vd.id()->idn() != -1) {
          os << " X_INTRODUCED_" << vd.id()->idn() << "_";
        } else if (vd.id()->v().size() != 0){
          os << " " << vd.id()->v();
        } 
        string name = os.str();     

        // --- --- --- Switch accoring to the type/kind of declaration
        if (ti.isEnum()){
          nl_file->add_vdecl_enum();
          cerr << "vdecl enum not implemented" << endl;
          assert(false);
        /*} else if(env) {
          nl_file->add_vdecl_tystr();
          cerr << "vdecl tystr not implemented" << endl;
          assert(false);*/
        } else {
          if(ti.isarray()){
            // Array
            nl_file->add_vdecl_array(name, ti.ranges(), ti.type(), ti.domain() );
            cerr << "vedcl array not implemented" << endl;
            assert(false);
          } else {
            // "Normal" var
            nl_file->add_vdecl(name, ti.type(), ti.domain() );
            cerr << "vedcl array not implemented" << endl;
            assert(false);
          }
        } 

        
/*  // VDECL
                
          p(vd.ti());
          if (!vd.ti()->isEnum()) {
            os << ":";
          }

          if (vd.id()->idn() != -1) {
            os << " X_INTRODUCED_" << vd.id()->idn() << "_";
          } else if (vd.id()->v().size() != 0)
            os << " " << vd.id()->v();
          if (vd.introduced()) {
            os << " ::var_is_introduced ";
          }
          p(vd.ann());
          if (vd.e()) {
            os << " = ";
            p(vd.e());
          }
          */

     
      } break;


      case Expression::E_LET: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);        
      } break;

      case Expression::E_TI: {  // TypeInst
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);      
      } break;

    } // END OF SWITCH
  } // END OF FUN






  void NLSolverInstance::processFlatZinc(void) {}

  void NLSolverInstance::resetSolver(void) {}

  Expression*
  NLSolverInstance::getSolutionValue(Id* id) {
    assert(false);
    return NULL;
  }
}
