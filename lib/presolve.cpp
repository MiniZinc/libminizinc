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
          ASTExprVec<Expression> args = ann->cast<Call>()->args();
          Id* s_id = args[0]->cast<Id>();
          bool save = args[1]->cast<BoolLit>()->v();

          if (s_id->v() == constants().presolve.calls->v())
            subproblems.push_back(new CallsSubproblem(model, env, i, options, save));
          else if (s_id->v() == constants().presolve.model->v())
            subproblems.push_back(new ModelSubproblem(model, env, i, options, save));
          else if (s_id->v() == constants().presolve.global->v())
            subproblems.push_back(new GlobalSubproblem(model, env, i, options, save));
          else
            throw TypeError(env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");
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

  void Presolver::Subproblem::registerTableConstraint() {
    GCLock lock;

    //  TODO: Make sure of the location of table.mzn
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
    } rt(origin_env, origin);
    iterItems(rt, table_model);
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

    //TODO: Split into TableConstraintBuilder/ElementConstraintBuilder.
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

    Expression* x = nullptr;
    std::vector< Expression* > tableVars;
    for (auto it = predicate->params().begin(); it != predicate->params().end(); ++it) {
      Expression* id = new Id(Location(), (*it)->id()->str(), (*it));
      id->type((*it)->type());
      if (id->type().isvarbool() && constraint == IntTable) {
        Call* c = new Call( Location(), "bool2int", std::vector< Expression* >(1, id) );
        c->decl( origin->matchFn(origin_env, c) );
        c->type( Type::varint() );
        tableVars.push_back(c);
      } else if( id->type().dim() > 0 ) {

        if (x == nullptr) {
//          TODO: This results in empty arrayliteral when empty. Might want to change that.
          x = new ArrayLit(Location(), tableVars);
          x->type( constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );
        } else {
          Expression* arr = new ArrayLit(Location(), tableVars);
          arr->type(constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );
          x = new BinOp(Location(), x, BOT_PLUSPLUS, arr);
          x->type( constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );
        }
        tableVars.clear();

        if ((*it)->type().dim() > 1) {
          Call* c = new Call( Location(), "array1d", std::vector<Expression*>(1, id) );
          c->type( id->type().bt() == Type::BT_BOOL ? Type::varbool(1) : Type::varint(1) );
          c->decl( origin->matchFn(origin_env, c) );
          id = c;
        }
        if (id->type().bt() == Type::BT_BOOL && constraint == IntTable) {
          Call* c = new Call( Location(), "bool2int", std::vector< Expression* >(1, id) );
          c->type( Type::varint(1) );
          c->decl( origin->matchFn(origin_env, c) );
          x = new BinOp(Location(), x, BOT_PLUSPLUS, c);
        } else {
          x = new BinOp(Location(), x, BOT_PLUSPLUS, id);
        }
        x->type( constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );

      } else {
        tableVars.push_back(id);
      }
    }

    Expression* t = nullptr;
    std::vector< Expression* > tableData;
    do {
      auto sol = si->getSolution();
      for (auto it = predicate->params().begin(); it != predicate->params().end(); ++it){
        Expression* exp = copy(origin_env, sol[ (*it)->id()->str() ], false, false, true);;
        if ((*it)->type().dim() > 0){

          if (t == nullptr) {
            t = new ArrayLit(Location(), tableData);
            t->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );
          } else {
            Expression* arr = new ArrayLit(Location(), tableData);
            arr->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );
            t = new BinOp(Location(), t, BOT_PLUSPLUS, arr);
            t->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );
          }
          tableData.clear();


          exp->type( exp->type().bt() == Type::BT_BOOL ? Type::parbool( (*it)->type().dim() ) : Type::parint( (*it)->type().dim() ) );
          if ((*it)->type().dim() > 1) {
            Call* c = new Call( Location(), "array1d", std::vector<Expression*>(1, exp) );
            c->type( exp->type().bt() == Type::BT_BOOL ? Type::parbool(1) : Type::parint(1) );
            c->decl( origin->matchFn(origin_env, c) );
            exp = c;
          }
          t = new BinOp(Location(), t, BOT_PLUSPLUS, exp);
          t->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );

        } else if ( (*it)->type().isint() ){
          exp->type( Type::parint() );
          tableData.push_back(exp);
        } else if ( (*it)->type().isbool()) {
          exp->type( Type::parbool() );
          tableData.push_back(exp);
        }
      }
    } while (si->next() != SolverInstance::ERROR);

    if (x == nullptr)
      x = new ArrayLit(Location(), tableVars);
    else if ( !tableVars.empty() ) {
      Expression* arr = new ArrayLit(Location(), tableVars);
      arr->type(constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );
      x = new BinOp(Location(), x, BOT_PLUSPLUS, arr);
    }
    x->type( constraint == BoolTable ? Type::varbool(1) : Type::varint(1) );

    if(t == nullptr)
      t = new ArrayLit(Location(), tableData);
    else if( !tableData.empty() ) {
      Expression* arr = new ArrayLit(Location(), tableData);
      arr->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );
      t = new BinOp(Location(), t, BOT_PLUSPLUS, arr);
    }
    t->type( constraint == BoolTable ? Type::parbool(1) : Type::parint(1) );
    std::vector<Expression*> dataArgs;
    SetLit* dataIndex1 = new SetLit(Location(),
               IntSetVal::a(
                       std::vector<IntSetVal::Range>( 1, IntSetVal::Range(IntVal(1), IntVal(si->getNr_solutions()) ))
               ) );
    dataIndex1->type( Type::parsetint() );
    dataArgs.push_back(dataIndex1);
    Call* dataIndex2 = new Call(Location(), "index_set", std::vector<Expression*>(1, x));
    dataIndex2->type(Type::parsetint());
    dataIndex2->decl( origin->matchFn(origin_env, dataIndex2) );
    dataArgs.push_back(dataIndex2);
    dataArgs.push_back(t);
    t = new Call(Location(), "array2d", dataArgs);
    t->type( constraint == BoolTable ? Type::parbool(2) : Type::parint(2) );
    t->cast<Call>()->decl( origin->matchFn(origin_env, t->cast<Call>()) );

    std::vector< Expression* > table_args;
    table_args.push_back(x);
    table_args.push_back(t);

    Call* table_call = new Call(Location(), "table", table_args);
    FunctionI* table_decl = origin->matchFn(origin_env, table_call);
    if(table_decl == nullptr) {
      registerTableConstraint();
      table_decl = origin->matchFn(origin_env, table_call);
      assert(table_decl != nullptr);
    }
    table_call->decl(table_decl);
    table_call->type(Type::varbool());

    predicate->e(table_call);

    predicate->e()->type(Type::varbool());

//    Printer p = Printer(std::cout,0);
//    std::cerr << std::endl << std::endl;
//    p.print(origin_env.orig);
//    std::cerr << std::endl;
  }
}
