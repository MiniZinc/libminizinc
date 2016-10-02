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

  Presolver::~Presolver() {
    for (Subproblem* it : subproblems)
      delete it;
  }

  void Presolver::presolve() {
    findPresolveAnnotations();
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

    if (options.modelOutput != "") {
      if (options.verbose)
        std::cerr << "\tPrinting presolved model to '" << options.modelOutput << "' ..." << std::flush;

      std::ofstream os;
      os.open(options.modelOutput.c_str(), std::ios::out);
      checkIOStatus (os.good(), " I/O error: cannot open presolved model output file. ");
      os << "include \"table.mzn\";" << std::endl << std::endl;
      Printer p(os);
      for (Subproblem* it : subproblems){
        p.print(it->getPredicate());
        os << std::endl;
      }
      checkIOStatus (os.good(), " I/O error: cannot write presolved model output file. ");
      os.close();

      if (options.verbose)
        std::cerr << "done." << std::endl;
    }
  }

  void Presolver::findPresolveAnnotations() {
    class PresolveVisitor : public ItemVisitor {
    public:
      std::vector<Subproblem*>& subproblems;
      EnvI& env;
      Model* model;
      Options& options;
      PresolveVisitor(std::vector<Subproblem*>& subproblems, EnvI& env, Model* model, Options& options) : subproblems(
              subproblems), env(env), model(model), options(options) { }
      void vFunctionI(FunctionI* i) {
        Expression* ann = getAnnotation(i->ann(),constants().presolve.presolve);
        if (ann) {
          if (! i->e()->type().isvarbool())
            throw TypeError(env, i->loc(), "Presolve annotation on non-predicate `" + i->id().str() + "'");
          if(ann->eid() == Expression::E_CALL) {
            ASTExprVec<Expression> args = ann->cast<Call>()->args();
            Id* s_id = args[0]->cast<Id>();
            std::string solver = "";
            bool save;
            if (args.size() > 1){
              solver = args[1]->cast<StringLit>()->v().str();
            }

            if ( s_id->v() == constants().presolve.calls->v() )
              subproblems.push_back( new CallsSubproblem(model, env, i, options, solver) );
            else if ( s_id->v() == constants().presolve.model->v() )
              subproblems.push_back( new ModelSubproblem(model, env, i, options, solver) );
            else if ( s_id->v() == constants().presolve.global->v() )
              subproblems.push_back( new GlobalSubproblem(model, env, i, options, solver) );
            else
              throw TypeError(env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");
          } else {
            subproblems.push_back( new ModelSubproblem(model, env, i, options) );
          }
        }
      }
    } pv(subproblems, env.envi(), model, options);
    iterItems(pv, model);
  }

  void Presolver::findPresolvedCalls() {
    class CallSeeker : public EVisitor {
    public:
      std::vector<Subproblem*>& subproblems;
      EnvI& env;
      Model* m;

      CallSeeker(std::vector<Subproblem*>& subproblems, EnvI& env, Model* m) : subproblems(subproblems), env(env), m(m) { }
      virtual void vCall(Call &call) {
        for (size_t i = 0; i < subproblems.size(); ++i) {
          if (subproblems[i]->getPredicate() == m->matchFn(env, &call)) {
            subproblems[i]->addCall(&call);
          }
        }
      }
    } cf(subproblems, env.envi(), model);
    TopDownIterator<CallSeeker> cf_it(cf);

    class CallItems : public ItemVisitor {
    public:
      TopDownIterator<CallSeeker>& cf_it;
      CallItems(TopDownIterator<CallSeeker>& cf_it) : cf_it(cf_it) { }

      virtual void vVarDeclI(VarDeclI* i) {cf_it.run(i->e());}
      virtual void vAssignI(AssignI* i) {cf_it.run(i->e());}
      virtual void vConstraintI(ConstraintI* i) {cf_it.run(i->e());}
      virtual void vSolveI(SolveI* i) {cf_it.run(i->e());}
      virtual void vOutputI(OutputI* i) {cf_it.run(i->e());}
      virtual void vFunctionI(FunctionI* i) {cf_it.run(i->e());}
    } ci(cf_it);
    iterItems(ci, model);
  }

  Presolver::Subproblem::Subproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options,
                                    std::string solver)
          : origin(origin), origin_env(origin_env), predicate(predicate), options(options), solver(solver) {
    GCLock lock;
    m = new Model();
    e = new Env(m);

    origin->mergeStdLib(e->envi(), m);
    registerBuiltins(*e, m);
  }

  Presolver::Subproblem::~Subproblem() {
    if(m) delete m;
    if(e) delete e;
    if(si){
      if (solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
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

    predicate->ann().clear();

    if (options.verbose)
      std::cerr << std::endl << "\tPresolving `" + predicate->id().str() + "' ... " << std::endl;

    if(options.verbose)
      std::cerr << "\t\tConstructing predicate model ...";
    constructModel();
    if(options.verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if(options.verbose)
      std::cerr << "\t\tPresolving problem ...";
    solveModel();
    if(options.verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if(options.verbose)
      std::cerr << "\t\tInserting solutions ...";
    replaceUsage();
    if(options.verbose)
      std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

    if (options.verbose)
      std::cerr << "\t done (" << stoptime(startTime) << ")" << std::endl;
  }

  void Presolver::Subproblem::solveModel() {
    GCLock lock;
    if (si != nullptr) {
      if (solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
        getGlobalSolverRegistry()->getSolverFactories().front()->destroySI(si);
      } else {
        delete si;
      }
      delete solns;
    }

    MiniZinc::Options ops = MiniZinc::Options();
    ops.setBoolParam(constants().opts.solver.allSols.str(), true);
    ops.setBoolParam(constants().opts.statistics.str(), false);

    if (solver == "" && !getGlobalSolverRegistry()->getSolverFactories().empty()) {
      si = getGlobalSolverRegistry()->getSolverFactories().front()->createSI(*e);
      si->setOptions(ops);
    } else {
      // TODO: Handle when GlobalsDir doesn't work for standard solver.
      if (solver != "") {
        ops.setStringParam(constants().opts.solver.fzn_solver.str(), solver);
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
    CopyMap cm;

    FunctionI* pred = copy(e->envi(), cm, predicate, false, true)->cast<FunctionI>();
    m->addItem(pred);
    recursiveRegisterFns(m, e->envi(), pred);
    std::vector<Expression*> args;
    if (pred->params().size() < 1) {
      throw EvalError(origin_env, pred->loc(), "Presolving requires a predicate which includes parameters as targeted "
              "variables for presolving");
    }
    for ( VarDecl* it : pred->params() ) {
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
    Call* pred_call = new Call(Location(), pred->id().str(), args, pred);
    pred_call->type(Type::varbool());
    ConstraintI* constraint = new ConstraintI(Location(), pred_call);
    m->addItem(constraint);
    m->addItem(SolveI::sat(Location()));

    generateFlatZinc(*e, options.onlyRangeDomains, options.optimize, options.newfzn);
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
      throw EvalError(origin_env, Location(), "Set types are unsupported for predicate presolving");

    auto builder = TableBuilder(origin_env, origin, options, constraint == BoolTable);
    builder.buildFromSolver(predicate, solns);
    Expression* tableCall = builder.getExpression();

    predicate->e(tableCall);
  }

  void Presolver::ModelSubproblem::constructModel() {
    assert(calls.size() > 0);
    GCLock lock;

    std::vector<Expression*> domains;
    for (Expression* it : calls[0]->args()) {
      domains.push_back( computeDomainExpr(origin_env, it) );
    }

    auto it = ++calls.begin();
    while (it != calls.end()) {
      for (unsigned int i = 0; i < (*it)->args().size(); ++i) {
        if (domains[i] != nullptr) {
          Expression* type = computeDomainExpr(origin_env, (*it)->args()[i] );
          if (type != nullptr) {
            Expression* expr = new BinOp(Location(), domains[i], BOT_UNION, type);
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

    if (options.verbose)
      std::cerr << std::endl << "\tPresolving `" + predicate->id().str() + "' ... " << std::endl;

    for (int i = 0; i < calls.size(); ++i) {
      currentCall = calls[i];
      if(options.verbose)
        std::cerr << "\t\tConstructing model for call " << i+1 << " ...";
      constructModel();
      if(options.verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

//      TODO: check if the current range has already been solved.
      if(options.verbose)
        std::cerr << "\t\tPresolving call " << i+1 << "  ...";
      solveModel();
      if(options.verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;

      if(options.verbose)
        std::cerr << "\t\tInserting solutions ...";
      replaceUsage();
      if(options.verbose)
        std::cerr << " done (" << stoptime(lastTime) << ")" << std::endl;
    }

    if (options.verbose)
      std::cerr << "\t done (" << stoptime(startTime) << ")" << std::endl;
  }

  void Presolver::CallsSubproblem::constructModel() {
    assert(currentCall != nullptr);
    GCLock lock;

    if(m->size() != 0) {
      delete m;
      delete e;
      m = new Model();
      e = new Env(m);
      origin->mergeStdLib(e->envi(), m);
      registerBuiltins(*e, m);
    }

    FunctionI* pred = copy(e->envi(), predicate, false, true)->cast<FunctionI>();
    m->addItem(pred);
    recursiveRegisterFns(m, e->envi(), pred);
    std::vector<Expression*> args;
    std::vector<VarDecl*> decls;
    for (VarDecl* it : pred->params()) {
      // TODO: Decide on strategy on parameter arguments
      if (it->type().ti() == Type::TypeInst::TI_PAR) {
        throw EvalError(origin_env, it->loc(), "Presolving is currently unsupported for predicates using parameter"
                "arguments");
      }
      VarDecl* vd = new VarDecl(Location(), it->ti(), it->id(), NULL);
      m->addItem(new VarDeclI(Location(), vd));

      decls.push_back(vd);

      Id* arg = new Id(Location(), vd->id()->str().str(), vd);
      arg->type(vd->type());
      args.push_back(arg);
    }
    Call* pred_call = new Call(Location(), pred->id().str(), args, pred);
    pred_call->type(Type::varbool());
    ConstraintI* constraint = new ConstraintI(Location(), pred_call);
    m->addItem(constraint);
    m->addItem(SolveI::sat(Location()));


    assert( decls.size() == currentCall->args().size() );
    for (int i = 0; i < decls.size(); ++i) {
      Expression* dom = computeDomainExpr(origin_env, currentCall->args()[i]);
      decls[i]->ti()->domain(dom);
      if (currentCall->args()[i]->type().dim() > 0) {
        std::vector<TypeInst*> ranges;
        computeRanges(origin_env, currentCall->args()[i], ranges);
        decls[i]->ti()->setRanges(ranges);
      }
    }

    generateFlatZinc(*e, options.onlyRangeDomains, options.optimize, options.newfzn);
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
      throw EvalError(origin_env, Location(), "Set types are unsupported for predicate presolving");

    auto builder = TableBuilder(origin_env, origin, options, constraint == BoolTable);
    builder.buildFromSolver(predicate, solns, currentCall->args());
    Call* tableCall = builder.getExpression();

    currentCall->id( tableCall->id() );
    currentCall->args( tableCall->args() );
    currentCall->decl( tableCall->decl() );
  }

}
