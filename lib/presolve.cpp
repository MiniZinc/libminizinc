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

void MiniZinc::presolve(Env& env, Model* m) {
  std::vector<SubModel> submodels;

  find_presolve_annotations(env, m, submodels);
  if (submodels.size() < 1) return;

  find_presolved_calls(env, m, submodels);

  m->compact();
}

void MiniZinc::find_presolve_annotations(Env &env, Model *m, std::vector<SubModel> &submodels) {
  class GoalSeeker : public ItemVisitor {
  public:
    EnvI& env;
    std::vector<SubModel>& submodels;
    GoalSeeker(EnvI& env0, std::vector<SubModel>& submodels0) : env(env0), submodels(submodels0) {};
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
          throw TypeError(this->env, s_id->loc(), "Invalid presolve strategy `" + s_id->str().str() + "'");

        submodels.push_back(SubModel(i, strategy, save));
        i->remove();
      }
    }
  } goals(env.envi(),submodels);
  iterItems(goals,m);
}

void MiniZinc::find_presolved_calls(Env& env, Model* m, std::vector<SubModel>& submodels) {
  CallProcessor cf = CallProcessor(env.envi(), m, submodels);
  TopDownIterator<CallProcessor> cf_it(cf);

  class CallSeeker : public ItemVisitor {
  public:
    EnvI& env;
    TopDownIterator<CallProcessor>& cf;
    CallSeeker(EnvI& env0, TopDownIterator<CallProcessor>& cf0) : env(env0), cf(cf0) {};
    void vConstraintI(ConstraintI* i) {
      cf.run(i->e());
    }
  } cs(env.envi(), cf_it);
  iterItems(cs, m);
}

void MiniZinc::CallProcessor::vCall(Call &call) {
  for (size_t i = 0; i < submodels.size(); ++i) {
    if (submodels[i].p() == m->matchFn(env, &call)) {
      submodels[i].addCall(&call);
    }
  }
}
