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
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>

namespace MiniZinc {

  void Presolver::presolve() {
    find_presolve_annotations();
    if (submodels.size() < 1) return;

    find_presolved_calls();

    for (auto it = submodels.begin(); it != submodels.end(); ++it) {
//      TODO: Only when submodel has calls.
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
      PresolveVisitor(EnvI& env, std::vector<SubModel>& submodels) : env(env), submodels(submodels) { };
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
      }
    } pv(env.envi(), submodels);
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
    Model* m = new Model();
    Env e = Env(m);
    CopyMap cm;

    FunctionI* pred = copy(e.envi(), cm, submodel.predicate, true, true)->cast<FunctionI>();
    m->addItem(pred);
    m->registerFn(e.envi(),pred);
    std::vector<Expression*> args;
    for (auto it = submodel.predicate->params().begin(); it != submodel.predicate->params().end(); ++it) {
      //TODO: Deal with non-variable parameters
      VarDecl* vd = new VarDecl(Location(),
                                copy(e.envi(), cm, (*it)->ti(), true, true, false)->cast<TypeInst>(),
                                (*it)->id()->str().str(), NULL);
      m->addItem(new VarDeclI(Location(), vd));
      Id* arg = new Id(Location(), vd->id()->v(), vd);
      arg->type(vd->type());
      args.push_back(arg);
    }
    Call* pred_call = new Call(Location(), pred->id(), args);
    pred_call->type(Type::varbool());
    pred_call->decl(pred);
    ConstraintI* constraint = new ConstraintI(Location(), pred_call);
    m->addItem(constraint);
    m->addItem(SolveI::sat(Location()));

    model->mergeStdLib(e.envi(),m);

    FlatteningOptions fops = FlatteningOptions();
    // TODO: match main model.
    fops.onlyRangeDomains = false;
    flatten(e, fops);
//    Printer p = Printer(std::cerr);
//    std::cerr << std::endl << std::endl;
//    p.print(e.flat());
//    std::cerr << std::endl;


    delete m;
  }
}
