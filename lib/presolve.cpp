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
#include <minizinc/options.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>

namespace MiniZinc {

  Presolver::~Presolver() {
    for (auto it = subproblems.begin(); it != subproblems.end(); ++it)
      delete (*it);
  }

  void Presolver::presolve() {
    findPresolveAnnotations();
    if (subproblems.size() < 1){

      return;
    }

    findPresolvedCalls();

//    TODO: Deal with circular presolving.
//    TODO: Handle errors of individual solving here.
    for (auto it = subproblems.begin(); it != subproblems.end(); ++it) {
      (*it)->solve();
    }
  }

  void Presolver::findPresolveAnnotations() {
    class PresolveVisitor : public ItemVisitor {
    public:
      std::vector<Subproblem*>& subproblems;
      EnvI& env;
      Model* model;
      Options& options;
      PresolveVisitor(vector<Subproblem*>& subproblems, EnvI& env, Model* model, Options& options) : subproblems(
              subproblems), env(env), model(model), options(options) { }
      void vFunctionI(FunctionI* i) {
        Expression* ann = getAnnotation(i->ann(),constants().presolve.presolve);
        if (ann) {
          if (! i->e()->type().isvarbool())
            throw TypeError(env, i->loc(), "Presolve annotation on non-predicate `" + i->id().str() + "'");
          if(ann->eid() == Expression::E_CALL) {
            ASTExprVec<Expression> args = ann->cast<Call>()->args();
            Id* s_id = args[0]->cast<Id>();
            bool save = args[1]->cast<BoolLit>()->v();

            if ( s_id->v() == constants().presolve.calls->v() )
              subproblems.push_back( new CallsSubproblem(model, env, i, options, save) );
            else if ( s_id->v() == constants().presolve.model->v() )
              subproblems.push_back( new ModelSubproblem(model, env, i, options, save) );
            else if ( s_id->v() == constants().presolve.global->v() )
              subproblems.push_back( new GlobalSubproblem(model, env, i, options, save) );
            else
              throw TypeError(env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");
          } else {
            subproblems.push_back( new ModelSubproblem(model, env, i, options, false) );
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

//    TODO: Replace with an ItemVisitor to support split models
    for (ConstraintIterator it = model->begin_constraints(); it != model->end_constraints(); ++it) {
      cf_it.run(it->e());
    }
  }

  Presolver::Subproblem::Subproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options,
                                    bool save)
          : origin(origin), origin_env(origin_env), predicate(predicate), options(options), save(save) {
    GCLock lock;
    m = new Model();
    e = new Env(m);

    origin->mergeStdLib(e->envi(), m);
    registerBuiltins(*e, m);
  }

  Presolver::Subproblem::~Subproblem() {
    if(m) delete m;
    if(e) delete e;
    if(si) delete si;
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
    if (si == nullptr) {
      MiniZinc::Options ops = MiniZinc::Options();
      ops.setBoolParam(constants().opts.solver.allSols.str(), true);
      ops.setBoolParam(constants().opts.statistics.str(), false);

      si = new FZNSolverInstance(*e, ops);
      solns = new Solns2Vector(e, origin_env);
      si->setSolns2Out(solns);
    }

    auto status = si->solve();

    if (status != SolverInstance::OPT && status != SolverInstance::SAT )
      throw InternalError("Unable to solve subproblem for the `" + predicate->id().str() + "' predicate");
  }

  void Presolver::GlobalSubproblem::constructModel() {
    GCLock lock;
    CopyMap cm;

//  TODO: make sure this actually works for everything that's called in the predicate.
    FunctionI* pred = copy(e->envi(), cm, predicate, false, true)->cast<FunctionI>();
    m->addItem(pred);
    recursiveRegisterFns(m, e->envi(), pred);
    std::vector<Expression*> args;
    for (auto it = pred->params().begin(); it != pred->params().end(); ++it) {
      // TODO: Deal with non-variable parameters
      VarDecl* vd = new VarDecl(Location(), (*it)->ti(), (*it)->id(), NULL);
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

    FlatteningOptions fopts;
    fopts.onlyRangeDomains = options.onlyRangeDomains;
    flatten(*e, fopts);

    if(options.optimize)
      optimize(*e);

    if (!options.newfzn) {
      oldflatzinc(*e);
    } else {
      e->flat()->compact();
      e->output()->compact();
    }

//    Printer p = Printer(std::cout);
//    std::cerr << std::endl << std::endl;
//    p.print(e->model());
//    std::cerr << std::endl;
  }

  void Presolver::GlobalSubproblem::replaceUsage() {
    GCLock lock;

    Constraint constraint = BoolTable;
    for (auto it = predicate->params().begin(); it != predicate->params().end(); ++it) {
      if (constraint == BoolTable && (*it)->type().bt() == Type::BT_INT )
        constraint = IntTable;
      else if (constraint != Element && ((*it)->type().is_set()))
        constraint = Element;
    }

//    TODO: Add set support
    if(constraint == Element)
      throw EvalError(origin_env, Location(), "Set types are unsupported for predicate presolving");

    auto builder = TableBuilder(origin_env, origin, options, constraint == BoolTable);
    builder.buildFromSolver(predicate, solns);
    Expression* tableCall = builder.getExpression();

    predicate->e(tableCall);

//    Printer p = Printer(std::cout,0);
//    std::cerr << std::endl << std::endl;
//    p.print(origin_env.orig);
//    std::cerr << std::endl;
  }

  void Presolver::ModelSubproblem::constructModel() {
    assert(calls.size() > 0);
    GCLock lock;

    std::vector<Expression*> domains;
    for (auto it = calls[0]->args().begin(); it != calls[0]->args().end(); ++it) {
      domains.push_back( computeDomainExpr(origin_env, *it) );
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
      predicate->params()[i]->ti()->domain(domains[i]);
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

    bool construction = m->size() == 0;
    if (construction) {
      FunctionI* pred = copy(e->envi(), predicate, false, true)->cast<FunctionI>();
      m->addItem(pred);
      m->registerFn(e->envi(), pred);
      std::vector<Expression*> args;
      for (auto it = pred->params().begin(); it != pred->params().end(); ++it) {
        // TODO: Deal with non-variable parameters
        VarDecl* vd = new VarDecl(Location(), (*it)->ti(), (*it)->id(), NULL);
        m->addItem(new VarDeclI(Location(), vd));

        modelArgs.push_back(vd);

        Id* arg = new Id(Location(), vd->id()->str().str(), vd);
        arg->type(vd->type());
        args.push_back(arg);
      }
      Call* pred_call = new Call(Location(), pred->id().str(), args, pred);
      pred_call->type(Type::varbool());
      ConstraintI* constraint = new ConstraintI(Location(), pred_call);
      m->addItem(constraint);
      m->addItem(SolveI::sat(Location()));
    } else {
//      TODO: Consider just making a new model, this turns out to be rather slow.
      delete si;
      delete solns;
      si = nullptr;
      solns = nullptr;
      e->envi().flat_removeItem( e->flat()->solveItem() );
    }

    assert( modelArgs.size() == currentCall->args().size() );
    for (int i = 0; i < modelArgs.size(); ++i) {
      Expression* dom = computeDomainExpr(origin_env, currentCall->args()[i]);
      modelArgs[i]->ti()->domain(dom);
      if (!construction)
        modelArgs[i]->flat()->ti()->domain(dom);
    }

    Printer p = Printer(std::cout);
    std::cerr << std::endl << std::endl;
    p.print(e->model());
    std::cerr << std::endl;

    FlatteningOptions fopts;
    fopts.onlyRangeDomains = options.onlyRangeDomains;
    flatten(*e, fopts);

    if (construction) {
      if (options.optimize)
        optimize(*e);

      if (!options.newfzn) {
        oldflatzinc(*e);
      } else {
        e->flat()->compact();
        e->output()->compact();
      }
    }
  }

  void Presolver::CallsSubproblem::replaceUsage() {
    GCLock lock;

    Constraint constraint = BoolTable;
    for (auto it = predicate->params().begin(); it != predicate->params().end(); ++it) {
      if (constraint == BoolTable && (*it)->type().bt() == Type::BT_INT )
        constraint = IntTable;
      else if (constraint != Element && ((*it)->type().is_set()))
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

//    Printer p = Printer(std::cout,0);
//    std::cerr << std::endl << std::endl;
//    p.print(origin_env.orig);
//    std::cerr << std::endl;

  }

}
