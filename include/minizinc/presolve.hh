/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PRESOLVE_HH__
#define __MINIZINC_PRESOLVE_HH__

#include <minizinc/model.hh>

namespace MiniZinc {

  class SubModel {
  public:
    enum Strategy {CALLS, MODEL, GLOBAL};
  protected:
    // predicate around which the submodel revolves
    FunctionI * _predicate;
// calls to the predicate in the model
    std::vector<Call*> _calls;
    // submodel presolve strategy
    Strategy _strategy;
    // save/load result from file?
    bool _save;

  public:
    SubModel(FunctionI* pred, Strategy strategy, bool save) : _predicate(pred), _save(save), _strategy(strategy) {};

    FunctionI *p() const { return _predicate; }

    void p(FunctionI *p) { SubModel::_predicate = p; }


    const std::vector<Call *> &c() const { return _calls; }

    void addCall(Call* c) { _calls.push_back(c); }
  };

  void presolve(Env& env, Model* m);

  void find_presolve_annotations(Env& env, Model* m, std::vector<SubModel>& submodels);

  void find_presolved_calls(Env& env, Model* m, std::vector<SubModel>& submodels);

//  TODO: Make Expression visitors more like ItemVisitors, so we don't need all the function definitions.
  class CallProcessor {
  public:
    EnvI& env;
    Model* m;
    std::vector<SubModel>& submodels;
    CallProcessor(EnvI& env0, Model* m0, std::vector<SubModel>& submodels0) : env(env0), m(m0), submodels(submodels0) {};
    /// Check annotations when expression is finished
    void exit(Expression* e) {}
    bool enter(Expression*) { return true; }
    /// Visit integer literal
    void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    void vSetLit(SetLit& sl) {}
    /// Visit string literal
    void vStringLit(const StringLit&) {}
    /// Visit identifier
    void vId(Id& id) {}
    /// Visit anonymous variable
    void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    void vArrayLit(ArrayLit& al) {}
    /// Visit array access
    void vArrayAccess(ArrayAccess& aa) {}
    /// Visit array comprehension
    void vComprehension(Comprehension& c) {}
    /// Visit array comprehension generator
    void vComprehensionGenerator(Comprehension& c, int gen_i) {}
    /// Visit if-then-else
    void vITE(ITE& ite) {}
    /// Visit binary operator
    void vBinOp(BinOp& bop) {}
    /// Visit unary operator
    void vUnOp(UnOp& uop) {}
    /// Visit call
    void vCall(Call& call);
    /// Visit let
    void vLet(Let& let) {}
    /// Visit variable declaration
    void vVarDecl(VarDecl& vd) {}
    /// Visit type inst
    void vTypeInst(TypeInst& ti) {}
    void vTIId(TIId& id) {}
  };
}
#endif
