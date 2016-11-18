/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/presolve.hh>
#include <minizinc/presolve_utils.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>
#include <minizinc/solver.hh>
#include <fstream>

namespace MiniZinc {


  Presolver::Presolver(Env& env, Flattener* flattener)
          : env(env), flattener(flattener) {
    findPresolveAnnotations();
  }

  Presolver::~Presolver() {
    for (Subproblem* it : subproblems)
      delete it;
  }

  void Presolver::presolve() {
    if (subproblems.size() < 1){
      return;
    }

    findPresolvedCalls();

//    TODO: More intelligent presolving in case of circular presolving
    for (Subproblem* it : subproblems) {
      try {
        it->solve();
      } catch(std::exception& e) {
        Exception* m = dynamic_cast<Exception*>(&e);
        std::cout << "% warning: Presolving `" << it->getPredicate()->id().str() << "' failed: ";
        if(m) {
          std::cout << m->msg();
        } else {
          std::cout << e.what();
        }
        std::cout << "." << std::endl;
      }
    }

    if (flattener->flag_output_presolved != "") {
      if (flattener->flag_verbose)
        std::cerr << "\tPrinting presolved model to '" << flattener->flag_output_presolved << "' ..." << std::flush;

      std::ofstream os;
      os.open(flattener->flag_output_presolved.c_str(), std::ios::out);
      checkIOStatus (os.good(), " I/O error: cannot open presolved model output file. ");
      os << "include \"table.mzn\";" << std::endl << std::endl;
      Printer p(os);
      for (Subproblem* it : subproblems){
        p.print(it->getPredicate());
        os << std::endl;
      }
      checkIOStatus (os.good(), " I/O error: cannot write presolved model output file. ");
      os.close();

      if (flattener->flag_verbose)
        std::cerr << "done." << std::endl;
    }
  }

  void Presolver::findPresolveAnnotations() {
    class PresolveVisitor : public ItemVisitor {
    public:
      std::vector<Subproblem*>& subproblems;
      EnvI& env;
      Model* model;
      Flattener* flattener;
      PresolveVisitor(std::vector<Subproblem*>& subproblems, EnvI& env, Model* model, Flattener* flattener) : subproblems(
              subproblems), env(env), model(model), flattener(flattener) { }
      void vFunctionI(FunctionI* i) {
        Expression* ann = getAnnotation(i->ann(),constants().presolve.presolve);
        if (ann) {
          if (! i->e()->type().isvarbool())
            throw TypeError(env, i->loc(), "Presolve annotation on non-predicate `" + i->id().str() + "'");
          if(ann->eid() == Expression::E_CALL) {
            ASTExprVec<Expression> args = ann->cast<Call>()->args();
            Id* s_id = args[0]->cast<Id>();

            if ( s_id->v() == constants().presolve.calls->v() )
              subproblems.push_back( new CallsSubproblem(model, env, i, flattener) );
            else if ( s_id->v() == constants().presolve.model->v() )
              subproblems.push_back( new ModelSubproblem(model, env, i, flattener) );
            else if ( s_id->v() == constants().presolve.global->v() )
              subproblems.push_back( new GlobalSubproblem(model, env, i, flattener) );
            else
              throw TypeError(env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");
          } else {
            subproblems.push_back( new ModelSubproblem(model, env, i, flattener) );
          }
        }
      }
    } pv(subproblems, env.envi(), env.model(), flattener);
    iterItems(pv, env.model());
  }

  void Presolver::findPresolvedCalls() {

    for (ConstraintIterator it = env.flat()->begin_constraints(); it != env.flat()->end_constraints(); ++it) {
      if (Call* c = it->e()->dyn_cast<Call>()) {
        for (size_t i = 0; i < subproblems.size(); ++i) {
          if (subproblems[i]->getPredicate() == c->decl()) {
            subproblems[i]->addCall(c, &(*it));
          }
        }
      }
    }
  }

  Presolver::Subproblem::Subproblem(Model* origin, EnvI& origin_env, FunctionI* pred_orig, Flattener* flattener)
          : origin(origin), origin_env(origin_env), pred_orig(pred_orig), flattener(flattener) {
    GCLock lock;
    m = new Model();
    e = new Env(m);

    origin->mergeStdLib(e->envi(), m);
    registerBuiltins(*e, m);

    predicate = copy(e->envi(), cm, pred_orig, false, true)->cast<FunctionI>();
    pred_orig->e(nullptr);
    m->addItem(predicate);
  }

  Presolver::Subproblem::~Subproblem() {
    if(m) delete m;
    if(e) delete e;
    if(si){
      if (flattener->flag_fzn_solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
        getGlobalSolverRegistry()->getSolverFactories().front()->destroySI(si);
      } else {
        delete si;
      }
    }
    if(solns) delete solns;
  }

  void Presolver::Subproblem::solve() {
    if (calls.empty())
      return;
    clock_t startTime = std::clock();
    clock_t lastTime = startTime;

    if (flattener->flag_verbose)
      std::cerr << std::endl << "\tPresolving `" + predicate->id().str() + "' ... " << std::endl;

    if(flattener->flag_verbose)
      std::cerr << "\t\tConstructing predicate model ...";
    constructModel();
    if(flattener->flag_verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if(flattener->flag_verbose)
      std::cerr << "\t\tPresolving problem ...";
    if (flattener->flag_print_presolve) {
      std::cout << "% Presolve model `" << predicate->id().c_str() << "'" << std::endl;
      Printer p(std::cout);
      std::cout << std::endl;
      p.print(e->model());
      std::cout << std::endl;
    }
    solveModel();
    if(flattener->flag_verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if(flattener->flag_verbose)
      std::cerr << "\t\tInserting solutions ...";
    replaceUsage();
    if(flattener->flag_verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if (flattener->flag_verbose)
      std::cerr << "\t done (" << stoptime(startTime) << ")" << std::endl;
  }

  void Presolver::Subproblem::solveModel() {
    GCLock lock;
    if (si != nullptr) {
      if (flattener->flag_fzn_solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
        getGlobalSolverRegistry()->getSolverFactories().front()->destroySI(si);
      } else {
        delete si;
      }
      delete solns;
    }

    MiniZinc::Options ops = MiniZinc::Options();
    ops.setBoolParam(constants().opts.solver.allSols.str(), true);

    if (flattener->flag_fzn_solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
      si = getGlobalSolverRegistry()->getSolverFactories().front()->createSI(*e);
      si->setOptions(ops);
    } else {
      // TODO: Handle when GlobalsDir doesn't work for standard solver.
      if (flattener->flag_fzn_solver != "") {
        ops.setStringParam(constants().opts.solver.fzn_solver.str(), flattener->flag_fzn_solver);
      }
      si = new FZNSolverInstance(*e, ops);
    }
    solns = new Solns2Vector(e, origin_env);
    si->setSolns2Out(solns);

    si->processFlatZinc();
    SolverInstance::Status status = si->solve();

    if (status != SolverInstance::OPT && status != SolverInstance::SAT)
      throw InternalError("Unable to solve subproblem");
    if ( solns->getSolutions().empty() )
//      TODO: This should not happen, but the GecodeSolverInstance doesn't work with -a yet.
      throw InternalError("Solver returned with solved status but did not return any solutions");
  }

  void Presolver::GlobalSubproblem::constructModel() {
    GCLock lock;

    recursiveRegisterFns(m, e->envi(), predicate);
    std::vector<Expression*> args;
    if (predicate->params().size() < 1) {
      throw EvalError(origin_env, predicate->loc(), "Presolving requires a predicate which includes parameters as targeted "
              "variables for presolving");
    }
    for ( VarDecl* it : predicate->params() ) {
      // TODO: Decide on strategy on parameter arguments
      if (it->type().ti() == Type::TypeInst::TI_PAR) {
        throw EvalError(origin_env, it->loc(), "Presolving is currently only supported for predicates using variable "
                "parameters");
      }
      VarDecl* vd = new VarDecl(Location(), it->ti(), it->id(), NULL);
      m->addItem(new VarDeclI(Location(), vd));
      Id* arg = new Id(Location(), vd->id()->str().str(), vd);
      arg->type(vd->type());
      args.push_back(arg);
    }
    Call* pred_call = new Call(predicate->loc().introduce(), predicate->id().str(), args, predicate);
    pred_call->type(Type::varbool());
    ConstraintI* constraint = new ConstraintI(predicate->loc().introduce(), pred_call);
    m->addItem(constraint);
    m->addItem(SolveI::sat(predicate->loc().introduce()));

    generateFlatZinc(*e, flattener->flag_only_range_domains, flattener->flag_optimize, flattener->flag_newfzn);
  }

  void Presolver::GlobalSubproblem::replaceUsage() {
    GCLock lock;

    Constraint constraint = BoolTable;
    for ( VarDecl* it : predicate->params() ) {
      if (constraint == BoolTable && it->type().bt() == Type::BT_INT)
        constraint = IntTable;
      else if ( constraint != Element && it->type().is_set() )
        constraint = Element;
    }

//    TODO: Add set support
    if(constraint == Element)
      throw EvalError(origin_env, predicate->loc(), "Set types are unsupported for predicate presolving");

    auto builder = TableBuilder(origin_env, origin, flattener, constraint == BoolTable);
    builder.buildFromSolver(predicate, solns);
    Expression* genTable = builder.getExpression();

    pred_orig->e(genTable);

    for (int i = 0; i < calls.size(); ++i) {
      builder.resetVariables();
      for (int j = 0; j < calls[i]->args().size(); ++j) {
        builder.addVariable(calls[i]->args()[j]);
      }
      Expression* tableCall = builder.getExpression();

      origin_env.flat_removeItem(items[i]);
      // TODO: Seems to introduce unused array declarations in FlatZinc.
      (void) flat_exp(origin_env, Ctx(), tableCall, constants().var_true, constants().var_true);
      delete tableCall;
    }
  }

  void Presolver::ModelSubproblem::constructModel() {
    assert(calls.size() > 0);
    GCLock lock;

    std::vector<Expression*> domains;
    for (Expression* it : calls[0]->args()) {
      // TODO: Can this be simplified with FlatZinc Calls?
      domains.push_back( computeDomainExpr(origin_env, it) );
    }

    auto it = ++calls.begin();
    while (it != calls.end()) {
      for (unsigned int i = 0; i < (*it)->args().size(); ++i) {
        if (domains[i] != nullptr) {
          // TODO: Can this be simplified with FlatZinc Calls?
          Expression* type = computeDomainExpr(origin_env, (*it)->args()[i] );
          if (type != nullptr) {
            Expression* expr = new BinOp(predicate->loc().introduce(), domains[i], BOT_UNION, type);
            expr->type( type->type() );
            domains[i] = expr;
          } else {
            domains[i] = nullptr;
          }
        }
      }
      ++it;
    }

    for (unsigned int i = 0; i < predicate->params().size(); ++i) {
      if(predicate->params()[i]->type().dim() > 0) {
//        TODO: Throw error when array ranges do not match between calls
//        TODO: Or even better, split off submodels, grouping them by array ranges
        std::vector<TypeInst*> ranges;
        // TODO: Can this be simplified with FlatZinc Calls?
        computeRanges(origin_env, calls[0]->args()[i], ranges);
        predicate->params()[i]->ti()->setRanges(ranges);
      }
      if(domains[i] != nullptr) {
        predicate->params()[i]->ti()->domain(domains[i]);
      }
    }

    GlobalSubproblem::constructModel();
  }

  void Presolver::CallsSubproblem::solve() {
    if (calls.empty())
      return;
    clock_t startTime = std::clock();
    clock_t lastTime = startTime;

    predicate->ann().clear();

    if (flattener->flag_verbose)
      std::cerr << std::endl << "\tPresolving `" + predicate->id().str() + "' ... " << std::endl;

    for (int i = 0; i < calls.size(); ++i) {
      currentCall = i;
      if(flattener->flag_verbose)
        std::cerr << "\t\tConstructing model for call " << i+1 << " ...";
      constructModel();
      if(flattener->flag_verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

//      TODO: check if the current range has already been solved.
      if(flattener->flag_verbose)
        std::cerr << "\t\tPresolving call " << i+1 << "  ...";
      if (flattener->flag_print_presolve) {
        std::cout << "% Presolve model `" << predicate->id().c_str() << "' " << i+1 << std::endl;
        Printer p(std::cout);
        std::cout << std::endl;
        p.print(e->model());
        std::cout << std::endl;
      }
      solveModel();
      if(flattener->flag_verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

      if(flattener->flag_verbose)
        std::cerr << "\t\tInserting solutions ...";
      replaceUsage();
      if(flattener->flag_verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;
    }

    origin_env.flat()->compact();
    if (flattener->flag_verbose)
      std::cerr << "\t done (" << stoptime(startTime) << ")" << std::endl;
  }

  void Presolver::CallsSubproblem::constructModel() {
    assert(currentCall >= 0 && currentCall <= calls.size());
    GCLock lock;

    if(m->size() != 0) {
      delete m;
      delete e;
      m = new Model();
      e = new Env(m);
      origin->mergeStdLib(e->envi(), m);
      registerBuiltins(*e, m);
      m->addItem(predicate);
    }

    recursiveRegisterFns(m, e->envi(), predicate);
    std::vector<Expression*> args;
    if (predicate->params().size() < 1) {
      // TODO: Make parameter included call-based.
      throw EvalError(origin_env, predicate->loc(), "Presolving requires a predicate which includes parameters as targeted "
              "variables for presolving");
    }
    std::vector<VarDecl*> decls;
    for (VarDecl* it : predicate->params()) {
      // TODO: Decide on strategy on parameter arguments
      if (it->type().ti() == Type::TypeInst::TI_PAR) {
        throw EvalError(origin_env, it->loc(), "Presolving is currently unsupported for predicates using parameter"
                "arguments");
      }
      VarDecl* vd = new VarDecl(predicate->loc().introduce(), it->ti(), it->id(), NULL);
      m->addItem(new VarDeclI(predicate->loc().introduce(), vd));

      decls.push_back(vd);

      Id* arg = new Id(predicate->loc().introduce(), vd->id()->str().str(), vd);
      arg->type(vd->type());
      args.push_back(arg);
    }
    Call* pred_call = new Call(predicate->loc().introduce(), predicate->id().str(), args, predicate);
    pred_call->type(Type::varbool());
    ConstraintI* constraint = new ConstraintI(predicate->loc().introduce(), pred_call);
    m->addItem(constraint);
    m->addItem(SolveI::sat(Location()));


    assert( decls.size() == calls[currentCall]->args().size() );
    for (int i = 0; i < decls.size(); ++i) {
      // TODO: Can this be simplified with FlatZinc Calls?
      Expression* dom = computeDomainExpr(origin_env, calls[currentCall]->args()[i]);
      decls[i]->ti()->domain(dom);
      if (calls[currentCall]->args()[i]->type().dim() > 0) {
        std::vector<TypeInst*> ranges;
        // TODO: Can this be simplified with FlatZinc Calls?
        computeRanges(origin_env, calls[currentCall]->args()[i], ranges);
        decls[i]->ti()->setRanges(ranges);
      }
    }

    generateFlatZinc(*e, flattener->flag_only_range_domains, flattener->flag_optimize, flattener->flag_newfzn);
  }

  void Presolver::CallsSubproblem::replaceUsage() {
    GCLock lock;

    Constraint constraint = BoolTable;
    for ( VarDecl* it : predicate->params() ) {
      if (constraint == BoolTable && it->type().bt() == Type::BT_INT)
        constraint = IntTable;
      else if ( constraint != Element && it->type().is_set() )
        constraint = Element;
    }

//    TODO: Add set support
    if(constraint == Element)
      throw EvalError(origin_env, pred_orig->loc(), "Set types are unsupported for predicate presolving");

    auto builder = TableBuilder(origin_env, origin, flattener, constraint == BoolTable);
    builder.buildFromSolver(predicate, solns, calls[currentCall]->args());
    Call* tableCall = builder.getExpression();

    (void) flat_exp(origin_env, Ctx(), tableCall, constants().var_true, constants().var_true);

    origin_env.flat_removeItem(items[currentCall]);
  }

}
