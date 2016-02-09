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
#include <minizinc/copy.hh>

namespace MiniZinc {

  class Presolver {
  public:
    class SubModel {
    public:
      enum Strategy {CALLS, MODEL, GLOBAL};
      // predicate around which the submodel revolves
      FunctionI* predicate;
      // calls to the predicate in the model
      std::vector<Call*> calls;
      // submodel presolve strategy
      Strategy strategy;
      // save/load result from file?
      bool save;

      SubModel(FunctionI* pred, Strategy strategy, bool save) : predicate(pred), save(save), strategy(strategy) {};

      void addCall(Call* c) { calls.push_back(c); }
    };

  protected:
    Env& env;
    Model* model;
    std::vector<SubModel> submodels;

    void find_presolve_annotations();

    void find_presolved_calls();

    void presolve_predicate_global(SubModel& submodel);

  public:
    Presolver(Env& env, Model* m) : env(env), model(m) {}

    void presolve();
  };

//  TODO: Move to where this makes more sense
  class ExprVisitor {
  public:
    /// Check annotations when expression is finished
    virtual void exit(Expression* e) {}
    virtual bool enter(Expression*) { return true; }
    /// Visit integer literal
    virtual void vIntLit(const IntLit&) {}
    /// Visit floating point literal
    virtual void vFloatLit(const FloatLit&) {}
    /// Visit Boolean literal
    virtual void vBoolLit(const BoolLit&) {}
    /// Visit set literal
    virtual void vSetLit(SetLit& sl) {}
    /// Visit string literal
    virtual void vStringLit(const StringLit&) {}
    /// Visit identifier
    virtual void vId(Id& id) {}
    /// Visit anonymous variable
    virtual void vAnonVar(const AnonVar&) {}
    /// Visit array literal
    virtual void vArrayLit(ArrayLit& al) {}
    /// Visit array access
    virtual void vArrayAccess(ArrayAccess& aa) {}
    /// Visit array comprehension
    virtual void vComprehension(Comprehension& c) {}
    /// Visit array comprehension generator
    virtual void vComprehensionGenerator(Comprehension& c, int gen_i) {}
    /// Visit if-then-else
    virtual void vITE(ITE& ite) {}
    /// Visit binary operator
    virtual void vBinOp(BinOp& bop) {}
    /// Visit unary operator
    virtual void vUnOp(UnOp& uop) {}
    /// Visit call
    virtual void vCall(Call& call) {}
    /// Visit let
    virtual void vLet(Let& let) {}
    /// Visit variable declaration
    virtual void vVarDecl(VarDecl& vd) {}
    /// Visit type inst
    virtual void vTypeInst(TypeInst& ti) {}
    virtual void vTIId(TIId& id) {}
  };
}
#endif
