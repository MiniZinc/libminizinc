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

    cout << nl_file;
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
        cerr << "Should not happen. (include \"" << i->cast<IncludeI>()->f() << "\")" << endl;
        assert(false);
      } break;

      case Item::II_VD: {
        cerr << "II_VD: Variable Declaration. [";
        const VarDecl& vd = *i->cast<VarDeclI>()->e();
        const TypeInst &ti        = *vd.ti()->cast<TypeInst>();
        const Expression &rhs     = *vd.e();
        analyse_vdecl(vd, ti, rhs);
        cerr << "]OK." << endl;
      } break;

      case Item::II_ASN:{
        cerr << "Should not happen." << endl;
        assert(false);
      } break;

      case Item::II_CON: {
        cerr << "II_CON: Constraint. [";
        Expression* e = i->cast<ConstraintI>()->e();
        if(e->eid() == Expression::E_CALL){
          const Call& c = *e->cast<Call>();
          analyse_constraint(c);
        } else {
          cerr << "Contraint is not a builtin call." << endl;
          assert(false);
        }
        cerr << "]OK." << endl;
      } break;

      case Item::II_SOL: {
        cerr << "Should have exactly one." << endl;
        assert(false);
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


  // FOR THE CALL
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
        cerr << "Set literal should not happen (use -Glinear)." << endl;
        assert(false);
      } break;

      case Expression::E_BOOLLIT: {
        cerr << "Bool literal should not happen (use -Glinear)." << endl;
        assert(false);
      } break;

      case Expression::E_STRINGLIT: {
        cerr << "String literal should not happen." << endl;
        assert(false);
      } break;

      /// --- --- --- Expressions

      case Expression::E_ID: { // Identifier
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);
      } break;

      case Expression::E_TIID: { // Type-inst identifier
        cerr << "Type identifier should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ANON: {
        cerr << "Anonymous variable should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ARRAYLIT: {
        cerr << "case " << e->eid() << " not implemented." << endl;
        assert(false);
      } break;

      case Expression::E_ARRAYACCESS: {
        cerr << "Array access should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_COMP:{ // Comprehension
        cerr << "Comprehension should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_ITE:{ // If-then-else expression
        cerr << "If then else should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_BINOP: {
        cerr << "Binary Op should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_UNOP: {
        cerr << "Unary Op should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_CALL: {
        cerr << "Call should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_VARDECL: {
        cerr << "Var Decl should not happen in flatzinc call." << endl;
        assert(false);
      } break;


      case Expression::E_LET: {
        cerr << "Let should not happen in flatzinc call." << endl;
        assert(false);
      } break;

      case Expression::E_TI: {
        cerr << "TI should not happen in flatzinc call." << endl;
        assert(false);
      } break;

    } // END OF SWITCH
  } // END OF FUN



  void NLSolverInstance::analyse_vdecl(const VarDecl &vd, const TypeInst &ti, const Expression &rhs){
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
      nl_file.add_vdecl_enum();
      cerr << "Should not happen" << endl;
      assert(false);
    }
    // TODO: question: what is env ? (see pretty printer line 622)
     /* else if(env) {
      nl_file.add_vdecl_tystr();
      cerr << "vdecl tystr not implemented" << endl;
      assert(false);
    } */ else {
      if(ti.isarray()){
        // In flatzinc, array always have a rhs: they can alway be replaced by their definition.
        // Follows the pointer starting at the ID to do so.
        cerr << "Definition of array " << name << " is not reproduced in nl.";
      } else {
        // Variable declaration
        const Type& type          = ti.type();
        const Expression* domain  = ti.domain();
        // Check the type
        assert(type.isvarint()||type.isvarfloat());
        bool isvarint = type.isvarint();
        // Check the domain
        // TODO: can be null see below
        //assert(domain != NULL);
if(domain == NULL){
  int index = nl_file.variables.size();
     Var v = Var(name, index, true, NLS_BoundItem::make_nobound(index));
        // Update Internal structure & Header
        nl_file.variables[name] = v;
        nl_file.name_vars.push_back(name);
        nl_file.header.nb_vars++;
        return;
}

        //assert(domain->eid() == Expression::E_SETLIT);
        const SetLit& sl = *domain->cast<SetLit>();
        // Integer?
        if(sl.isv()){
          assert(isvarint);
          nl_file.vdecl_integer(name, sl.isv());
        } // Floating Point?
        else if(sl.fsv()){
          assert(!isvarint);
          nl_file.vdecl_fp(name, sl.fsv());
        }// Else is a set
        else {
          cerr << "Should not happen" << endl;
          assert(false);
        }
        // domain:
        // null -> unrestricted: var int, var float...
        // boolean constant: true or false
        // or a setlit
        // 
      }
    }
  }


  void NLSolverInstance::analyse_constraint(const Call& c){
    // Guard
    if(c.decl() == NULL){
      cerr << "Undeclared function " << c.id();
      assert(false);
    }

    auto id = c.id();
    auto consint = constants().ids.int_;
    auto consfp = constants().ids.float_;

    // Integer constants.
    // TODO: question: which are constraint ?
    // TODO: question: how to deal with reif ?
    if(id == consint.lin_eq){  

      // Always array
      Expression* arg0 = c.arg(0);
      Expression* arg1 = c.arg(1);
      long long integer_constant = c.arg(2)->cast<IntLit>()->v().toInt();

      
           cerr << "constraint 'int lin_eq'  not implemented"; assert(false); }
    else if(id == consint.lin_le){  cerr << "constraint 'int lin_le'  not implemented"; assert(false); }
    else if(id == consint.lin_ne){  cerr << "constraint 'int lin_ne'  not implemented"; assert(false); }
    else if(id == consint.plus){    cerr << "Should not happen - Non linear to be implementeed constraint 'int plus'    not implemented"; assert(false); }
    else if(id == consint.minus){   cerr << "Should not happen - Non linear to be implementeed constraint 'int minus'   not implemented"; assert(false); }
    else if(id == consint.times){   cerr << "Non linear to be implementeed constraint 'int times'   not implemented"; assert(false); }
    else if(id == consint.div){     cerr << "Non linear to be implementeed constraint 'int div'     not implemented"; assert(false); }
    else if(id == consint.mod){     cerr << "Non linear to be implemented 'int mod'     not implemented"; assert(false); }
    else if(id == consint.lt){      cerr << "Should not happen 'int lt'"; assert(false); }
    else if(id == consint.le){// 2 args
      Expression* arg0 = c.arg(0);
      Expression* arg1 = c.arg(1);
      // --- --- --- Bound constraints
      if(arg0->type().ispar()){
        IntVal lowerBound = arg0->cast<IntLit>()->v();

         int lb = lowerBound.toInt();
        VarDecl& vd = *(arg1->cast<Id>()->decl());
            stringstream os;
    if (vd.id()->idn() != -1) {
      os << " X_INTRODUCED_" << vd.id()->idn() << "_";
    } else if (vd.id()->v().size() != 0){
      os << " " << vd.id()->v();
    }
    string name = os.str();

      Var v1 = nl_file.variables[name];
      NLS_BoundItem newBound = v1.bound;
      switch(v1.bound.tag){
            case NLS_BoundItem::LB_UB:{
                if(lb>v1.bound.lb){
                  newBound = NLS_BoundItem::make_bounded(lb, v1.bound.ub, v1.index);
                }
                break;
            }
            case  NLS_BoundItem::UB:{
                newBound = NLS_BoundItem::make_bounded(lb, v1.bound.ub, v1.index);
                
                break;
            }
            case  NLS_BoundItem::LB:{
                if(lb>v1.bound.lb){
                  newBound = NLS_BoundItem::make_lb_bounded(lb, v1.index);
                }
                break;
            }
            case  NLS_BoundItem::NONE:{
              newBound = NLS_BoundItem::make_lb_bounded(lb, v1.index);
                
                break;
            }
            case  NLS_BoundItem::EQ:{
                cerr << "Should not happen" << endl;
                assert(false);
            }
      }

      Var new_v = Var(name, v1.index, v1.is_integer, newBound);
      nl_file.variables[name] = new_v;



        
           }
      else if(arg1->type().ispar()){

      } else {}
      // --- --- --- Actual constraint
      
      
    }
    else if(id == consint.gt){      cerr << "constraint 'int gt'      not implemented"; assert(false); }
    else if(id == consint.ge){      cerr << "constraint 'int ge'      not implemented"; assert(false); }
    else if(id == consint.eq){      cerr << "constraint 'int eq'      not implemented"; assert(false); }
    else if(id == consint.ne){      cerr << "constraint 'int ne'      not implemented"; assert(false); }

    // Floating Point constants.
    // TODO: question: which are constraint ?
    // TODO: question: how to deal with reif ?
    else if(id == consfp.lin_eq){   cerr << "constraint 'float lin_eq' not implemented"; assert(false); }
    else if(id == consfp.lin_le){   cerr << "constraint 'float lin_le' not implemented"; assert(false); }
    else if(id == consfp.lin_lt){   cerr << "constraint 'float lin_lt' not implemented"; assert(false); }
    else if(id == consfp.lin_ne){   cerr << "constraint 'float lin_ne' not implemented"; assert(false); }
    else if(id == consfp.plus){     cerr << "constraint 'float plus  ' not implemented"; assert(false); }
    else if(id == consfp.minus){    cerr << "constraint 'float minus ' not implemented"; assert(false); }
    else if(id == consfp.times){    cerr << "constraint 'float times ' not implemented"; assert(false); }
    else if(id == consfp.div){      cerr << "constraint 'float div   ' not implemented"; assert(false); }
    else if(id == consfp.mod){      cerr << "constraint 'float mod   ' not implemented"; assert(false); }
    else if(id == consfp.lt){       cerr << "constraint 'float lt    ' not implemented"; assert(false); }
    else if(id == consfp.le){       cerr << "constraint 'float le    ' not implemented"; assert(false); }
    else if(id == consfp.gt){       cerr << "constraint 'float gt    ' not implemented"; assert(false); }
    else if(id == consfp.ge){       cerr << "constraint 'float ge    ' not implemented"; assert(false); }
    else if(id == consfp.eq){       cerr << "constraint 'float eq    ' not implemented"; assert(false); }
    else if(id == consfp.ne){       cerr << "constraint 'float ne    ' not implemented"; assert(false); }
    else if(id == consfp.in){       cerr << "constraint 'float in    ' not implemented"; assert(false); }
    else if(id == consfp.dom){      cerr << "constraint 'float dom   ' not implemented"; assert(false); }




     else {
      cerr << "Unrecognized builtins " << c.id() << " not implemented";
      assert(false);
    }
  }





  void NLSolverInstance::processFlatZinc(void) {}

  void NLSolverInstance::resetSolver(void) {}

  Expression*
  NLSolverInstance::getSolutionValue(Id* id) {
    assert(false);
    return NULL;
  }
}
