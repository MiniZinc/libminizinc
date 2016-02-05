/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/presolve.hh>

void MiniZinc::presolve(Env& env, Model* m) {
  class GoalSeeker : public ItemVisitor {
  public:
    void vFunctionI(FunctionI* i) {
      Expression* ann = getAnnotation(i->ann(),constants().presolve.presolve);
      if (ann) {
        ASTExprVec<Expression> args = ann->cast<Call>()->args();
        Id* strategy = args[0]->cast<Id>();
        bool save = args[1]->cast<BoolLit>()->v();
      }
    }
  } goals;
  iterItems(goals,m);
}
