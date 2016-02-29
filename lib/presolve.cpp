/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <minizinc/presolve.hh>
#include <minizinc/astiterator.hh>

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
      std::cerr << "\t done (" << stoptime(startTime) << ")" << std::endl;;
  }

  void Presolver::Subproblem::solveModel() {
    MiniZinc::Options ops = MiniZinc::Options();
    ops.setBoolParam(constants().opts.solver.allSols.str(), true);
    ops.setBoolParam(constants().opts.statistics.str(), false);

    si = new FZNPreSolverInstance(*e, ops);

    auto status = si->solve();

    if (status != SolverInstance::OPT && status != SolverInstance::SAT )
      throw InternalError("Unable to solve subproblem for the `" + predicate->id().str() + "' predicate");
  }

  Expression* Presolver::Subproblem::computeBounds(Expression* exp) {
    if (exp->eid() == Expression::E_ID )
      return exp->cast<Id>()->decl()->ti()->domain();
//    TODO: How about booleans?
    switch (exp->type().bt()) {
      case Type::BT_INT: {
        IntBounds ib = compute_int_bounds(origin_env, exp);
        if (ib.valid) {
          Expression* type = new SetLit( Location(), IntSetVal::a(ib.l, ib.u) );
          type->type(Type::parsetint());
          return type;
        }
        break;
      }
      case Type::BT_FLOAT: {
        FloatBounds ib = compute_float_bounds(origin_env, exp);
        if (ib.valid) {
          Expression* type = new SetLit(Location(), IntSetVal::a(ib.l, ib.u));
          type->type(Type::parsetfloat());
          return type;
        }
        break;
      }
      default:
        return nullptr;
    }
    return nullptr;
  }

  void Presolver::GlobalSubproblem::constructModel() {
    GCLock lock;
    CopyMap cm;

//  TODO: make sure this actually works for everything that's called in the predicate.
    FunctionI* pred = copy(e->envi(), cm, predicate, false, true)->cast<FunctionI>();
    m->addItem(pred);
    m->registerFn(e->envi(), pred);
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
      if (constraint == BoolTable && ((*it)->type().isint() || (*it)->type().isintarray()))
        constraint = IntTable;
      else if (constraint != Element && ((*it)->type().is_set()))
        constraint = Element;
    }

//    TODO: Add set support
    if(constraint == Element)
      throw EvalError(origin_env, Location(), "Set types are unsupported for predicate presolving");

    auto builder = TableExpressionBuilder(origin_env, origin, options, constraint == BoolTable);
    builder.buildFromSolver(predicate, si);
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
      domains.push_back( computeBounds(*it) );
    }

    auto it = ++calls.begin();
    while (it != calls.end()) {
      for (unsigned int i = 0; i < (*it)->args().size(); ++i) {
        if (domains[i] != nullptr) {
          Expression* type = computeBounds( (*it)->args()[i] );
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

  void Presolver::Subproblem::TableExpressionBuilder::buildFromSolver(FunctionI* f, FZNPreSolverInstance* si) {
    rows = si->getNr_solutions();

    for (auto it = f->params().begin(); it != f->params().end(); ++it) {
      Expression* id = new Id( Location(), (*it)->id()->str(), (*it) );
      id->type((*it)->type());
      addVariable(id);
    }

    do {
      auto sol = si->getSolution();
      for (auto it = f->params().begin(); it != f->params().end(); ++it) {
        Expression* exp = copy(env, sol[ (*it)->id()->str() ], false, false, true);
        exp->type( exp->type().bt() == Type::BT_BOOL ? Type::parbool( (*it)->type().dim() ) : Type::parint( (*it)->type().dim() ) );
        addData(exp);
      }
    } while (si->next() != SolverInstance::ERROR);

  }

  Expression* Presolver::Subproblem::TableExpressionBuilder::getExpression() {
    storeVars();
    ArrayLit* dataExpr = new ArrayLit(Location(), data);
    dataExpr->type( boolTable ? Type::parbool(1) : Type::parint(1) );

    std::vector<Expression*> conversionArgs;
    SetLit* index1 = new SetLit(Location(),
                                IntSetVal::a(
                                            std::vector<IntSetVal::Range>( 1, IntSetVal::Range(IntVal(1), IntVal(rows) ) )
                                ) );
    index1->type(Type::parsetint() );
    conversionArgs.push_back(index1);
    Call* index2 = new Call(Location(), "index_set", std::vector<Expression*>(1, variables));
    index2->type(Type::parsetint());
    index2->decl(m->matchFn(env, index2) );
    conversionArgs.push_back(index2);
    conversionArgs.push_back(dataExpr);
    Call* tableData = new Call(Location(), "array2d", conversionArgs);
    tableData->type(boolTable ? Type::parbool(2) : Type::parint(2) );
    tableData->decl(m->matchFn(env, tableData) );

    std::vector< Expression* > tableArgs;
    tableArgs.push_back(variables);
    tableArgs.push_back(tableData);

    Call* tableCall = new Call(Location(), "table", tableArgs);
    FunctionI* tableDecl = m->matchFn(env, tableCall);
    if(tableDecl == nullptr) {
      registerTableConstraint();
      tableDecl = m->matchFn(env, tableCall);
      assert(tableDecl != nullptr);
    }
    tableCall->decl(tableDecl);
    tableCall->type(Type::varbool());

    return tableCall;
  }

  void Presolver::Subproblem::TableExpressionBuilder::addVariable(Expression* var) {
    if (var->type().dim() > 1) {
      Call* c = new Call( Location(), "array1d", std::vector<Expression*>(1, var) );
      c->type( var->type().bt() == Type::BT_BOOL ? Type::varbool(1) : Type::varint(1) );
      c->decl( m->matchFn(env, c) );
      var = c;
    }

    if (var->type().bt() == Type::BT_BOOL && !boolTable) {
      Call* c = new Call( Location(), "bool2int", std::vector< Expression* >(1, var) );
      c->type( Type::varint( var->type().dim() ) );
      c->decl( m->matchFn(env, c) );
      var = c;
    }

    if (var->type().dim() > 0) {
      storeVars();
      if (variables == nullptr) {
        variables = var;
      } else {
        variables = new BinOp(Location(), variables, BOT_PLUSPLUS, var);
        variables->type( boolTable ? Type::varbool(1) : Type::varint(1) );
      }
    } else {
      vVariables.push_back(var);
    }

  }

  void Presolver::Subproblem::TableExpressionBuilder::addData(Expression* dat) {
    if (dat->type().dim() > 0) {
      ArrayLit* arr = nullptr;
      if (dat->eid() == Expression::E_CALL) {
        Call* c = dat->cast<Call>();
        if ( c->id().str() == "array1d" ) {
          arr = c->args()[0]->cast<ArrayLit>();
        } else {
          arr = c->args()[2]->cast<ArrayLit>();
        }
      } else if(dat->eid() == Expression::E_ARRAYLIT) {
        ArrayLit* arr = dat->cast<ArrayLit>();
      }
      assert(arr != nullptr);

      for (auto it = arr->v().begin(); it != arr->v().end(); ++it) {
        data.push_back(*it);
      }
    } else {
      data.push_back(dat);
    }
  }

  void Presolver::Subproblem::TableExpressionBuilder::storeVars() {
    if ( vVariables.empty() )
      return;

    if (variables == nullptr) {
      variables = new ArrayLit(Location(), vVariables);
    } else {
      Expression* arr = new ArrayLit(Location(), vVariables);
      arr->type( boolTable ? Type::varbool(1) : Type::varint(1) );
      variables = new BinOp(Location(), variables, BOT_PLUSPLUS, arr);
    }
    variables->type( boolTable ? Type::varbool(1) : Type::varint(1) );
    vVariables.clear();
  }

  void Presolver::Subproblem::TableExpressionBuilder::registerTableConstraint() {
    GCLock lock;

    //  TODO: Make sure of the location of table.mzn
    std::string loc = boolTable ? "/std/table_bool.mzn" : "/std/table_int.mzn";
    Model* table_model = parse(std::vector< std::string >(1, options.stdLibDir + "/std/table.mzn"),
                               std::vector< std::string >(), options.includePaths, false, false,
                               false, std::cerr);
    Env table_env(table_model);

    std::vector<TypeError> typeErrors;
    typecheck(table_env, table_model, typeErrors, false);
    assert(typeErrors.size() == 0);

    registerBuiltins(table_env, table_model);

    class RegisterTable : public ItemVisitor {
    public:
      EnvI& env;
      Model* model;
      RegisterTable(EnvI& env, Model* model) :env(env), model(model), cr(env, model), cr_it(cr){}
      class CallRegister : public EVisitor {
      public:
        EnvI& env;
        Model* model;
        CallRegister(EnvI& env, Model* model) : env(env), model(model) { }
        virtual void vCall(Call &call) {
          model->addItem(call.decl());
          model->registerFn(env, call.decl());
        }
      } cr;
      TopDownIterator<CallRegister> cr_it;
      void vFunctionI(FunctionI* i) {
        if (i->id().str() == "table") {
          FunctionI* ci = copy(env, i, false, true, false)->cast<FunctionI>();
          model->addItem(ci);
          model->registerFn(env, ci);
          cr_it.run(ci->e());
        }
      }
    } rt(env, m);
    iterItems(rt, table_model);
  }
}
