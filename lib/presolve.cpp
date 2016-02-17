/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
#include <minizinc/presolve.hh>
#include <minizinc/astexception.hh>
#include <minizinc/astiterator.hh>
#include <minizinc/flatten.hh>
#include <minizinc/optimize.hh>
#include <minizinc/solver.hh>
#include <minizinc/solvers/fzn_presolverinstance.hh>

namespace MiniZinc {

  void Presolver::presolve() {
    find_presolve_annotations();
    if (submodels.size() < 1) return;

    find_presolved_calls();

    for (auto it = submodels.begin(); it != submodels.end(); ++it) {
      if (it->calls.empty())
        continue;
      if (flag_verbose)
        std::cerr << "\tPresolving `" + it->predicate->id().str() + "'" << std::endl;
      switch (it->strategy) {
        case SubModel::GLOBAL:
          presolve_predicate_global(*it);
          break;
        default:
          throw EvalError(env.envi(), Location(), "Presolve strategy not supported yet.");
      }
    }
  }

  void Presolver::find_presolve_annotations() {
    class PresolveVisitor : public ItemVisitor {
    public:
      std::vector<SubModel>& submodels;
      EnvI& env;
      FunctionI*& table_constraint;
      PresolveVisitor(EnvI& env, std::vector<SubModel>& submodels, FunctionI*& table_constraint)
              : env(env), submodels(submodels), table_constraint(table_constraint) { };
      void vFunctionI(FunctionI* i) {
//        TODO: Check function type.
        Expression* ann = getAnnotation(i->ann(),constants().presolve.presolve);
        if (ann) {
          ASTExprVec<Expression> args = ann->cast<Call>()->args();
          Id* s_id = args[0]->cast<Id>();
          bool save = args[1]->cast<BoolLit>()->v();

          SubModel::Strategy strategy;
          if (s_id->v() == constants().presolve.calls->v())
            strategy = SubModel::CALLS;
          else if (s_id->v() == constants().presolve.model->v())
            strategy = SubModel::MODEL;
          else if (s_id->v() == constants().presolve.global->v())
            strategy = SubModel::GLOBAL;
          else
            throw TypeError(env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");

          submodels.push_back(SubModel(i, strategy, save));
        }
        if (i->id().str() == "table") {
          table_constraint = i;
        }
      }
    } pv(env.envi(), submodels, table_constraint);
    iterItems(pv, model);
  }

  void Presolver::find_presolved_calls() {
    class CallSeeker : public ExprVisitor {
    public:
      std::vector<SubModel>& submodels;
      EnvI& env;
      Model* m;

      CallSeeker(std::vector<SubModel>& submodels, EnvI& env, Model* m) : submodels(submodels), env(env), m(m) { }
      virtual void vCall(Call &call) {
        for (size_t i = 0; i < submodels.size(); ++i) {
          if (submodels[i].predicate == m->matchFn(env, &call)) {
            submodels[i].addCall(&call);
          }
        }
      }
    } cf(submodels, env.envi(), model);
    TopDownIterator<CallSeeker> cf_it(cf);

    for (ConstraintIterator it = model->begin_constraints(); it != model->end_constraints(); ++it) {
      cf_it.run(it->e());
    }
  }

  void Presolver::presolve_predicate_global(SubModel& submodel) {
    assert(submodel.strategy == SubModel::Strategy::GLOBAL);

    submodel.predicate->ann().clear();

    GCLock lock;
    Model m;
    Env e(&m);
    CopyMap cm;

    FunctionI* pred = copy(e.envi(), cm, submodel.predicate, false, true)->cast<FunctionI>();
    m.addItem(pred);
    m.registerFn(e.envi(), pred);
    std::vector<Expression*> args;
    for (auto it = pred->params().begin(); it != pred->params().end(); ++it) {
      //TODO: Deal with non-variable parameters
      VarDecl* vd = new VarDecl(Location(), (*it)->ti(), (*it)->id(), NULL);
      m.addItem(new VarDeclI(Location(), vd));
      Id* arg = new Id(Location(), vd->id()->str().str(), vd);
      arg->type(vd->type());
      args.push_back(arg);
    }
    Call* pred_call = new Call(Location(), pred->id().str(), args, pred);
    pred_call->type(Type::varbool());
    ConstraintI* constraint = new ConstraintI(Location(), pred_call);
    m.addItem(constraint);
    m.addItem(SolveI::sat(Location()));

//    TODO: Merg STD or RegisterBuiltins?
    model->mergeStdLib(e.envi(), &m);

    FlatteningOptions fops = FlatteningOptions();
    fops.onlyRangeDomains = flag_only_range_domains;
    flatten(e, fops);

    if(flag_optimize)
      optimize(e);

    e.flat()->compact();
    e.output()->compact();

//    Printer p = Printer(std::cerr);
//    std::cerr << std::endl << std::endl;
//    p.print(e.flat());
//    std::cerr << std::endl;

    Options ops = Options();
    ops.setBoolParam(constants().opts.solver.allSols.str(), true);
    ops.setBoolParam(constants().opts.statistics.str(), false);

    FZNPreSolverInstance si(e, ops);

    auto status = si.solve();

    if (status != SolverInstance::OPT && status != SolverInstance::SAT )
      throw InternalError("Unable to solve submodel for the `" + submodel.predicate->id().str() + "' predicate");

    vector<Expression*> tableVars;
    for (auto it = submodel.predicate->params().begin(); it != submodel.predicate->params().end(); ++it) {
      tableVars.push_back((*it)->id());
    }

    vector< vector<Expression*> > tableData;
    do {
      vector<Expression*> data;
      for (auto it = tableVars.begin(); it != tableVars.end(); ++it){
        Expression* e = copy( env.envi(), si.getSolution().at( (*it)->cast<Id>()->str() ).first->e() );
        e->type( (*it)->type() );
        data.push_back(e);
      }
      tableData.push_back(data);
    } while (si.next() != SolverInstance::ERROR);


    ArrayLit* x = new ArrayLit(Location(), tableVars);
    ArrayLit* t = new ArrayLit(Location(), tableData);
//    x->type(Type::varint(1));
//    t->type(Type::parint(2));
    vector<Expression*> table_args;
    table_args.push_back(x);
    table_args.push_back(t);


//    TODO: Include Table constraint definition.
    Call* table_call = new Call(Location(), "table", table_args, table_constraint);
    table_call->type(Type::varbool());

    submodel.predicate->e(table_call);
  }
}
