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
#include <minizinc/flattener.h>

namespace MiniZinc {
  class Flattener;

  class Presolver {
  protected:
    class Subproblem;

    Env& env;
    Model* model;
    std::vector<Subproblem> subproblems;

    Flattener* options;

    void find_presolve_annotations();

    void find_presolved_calls();

    void presolve_predicate_global(Subproblem& subproblem);

  public:
    Presolver(Env& env, Model* m, Flattener* options)
            : env(env), model(m), options(options) {}

    void presolve();
  };


  class Presolver::Subproblem{
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

    Subproblem(FunctionI* pred, Strategy strategy, bool save) : predicate(pred), save(save), strategy(strategy) {};

    void addCall(Call* c) { calls.push_back(c); }
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
