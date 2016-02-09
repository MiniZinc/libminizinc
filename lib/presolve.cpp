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

namespace MiniZinc {

  void Presolver::presolve() {
    find_presolve_annotations();
    if (submodels.size() < 1) return;

    find_presolved_calls();

    for (auto it = submodels.begin(); it != submodels.end(); ++it) {
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

    GCLock lock;
    Model* m = new Model();
    CopyMap cm;
    m->addItem( copy(env.envi(), cm, submodel.predicate, true, true) );

    std::vector<Expression*> args;
    for (auto it = submodel.predicate->params().begin(); it != submodel.predicate->params().end(); ++it) {
      //TODO: Deal with non-variable parameters
      VarDecl* vd = copy( env.envi(), cm, (Expression*) *it, false, false, false )->cast<VarDecl>();
      m->addItem( new VarDeclI(Location(), vd) );
      args.push_back( new Id(Location(), vd->id()->v(),vd) );
    }
    ConstraintI* constraint = new ConstraintI(Location(),
                                              new Call(Location(), submodel.predicate->id(), args)
    );
    m->addItem(constraint);
    m->addItem(SolveI::sat(Location()));

    delete m;
  }
}
