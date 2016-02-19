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
#include <minizinc/solvers/fzn_presolverinstance.hh>

namespace MiniZinc {
  class Presolver {
  public:
    struct Options;
  protected:
    class Subproblem;
    class GlobalSubproblem;
    class ModelSubproblem;
    class CallsSubproblem;

    Env& env;
    Model* model;
    std::vector<Subproblem*> subproblems;

    Options& options;

    void findPresolveAnnotations();

    void findPresolvedCalls();

  public:
    Presolver(Env& env, Model* m, Options& options)
            : env(env), model(m), options(options) {}

    virtual ~Presolver();

    void presolve();
  };

  struct Presolver::Options {
    vector<string> includePaths;
    string stdLibDir;
    string globalsDir;

    bool verbose = false;
    bool newfzn = false;
    bool optimize = true;
    bool onlyRangeDomains = false;
  };


  class Presolver::Subproblem{
  protected:
    Model* origin;
    EnvI& origin_env;
    // predicate around which the submodel revolves
    FunctionI* predicate;
    // calls to the predicate in the model
    std::vector<Call*> calls;
    // Flattener for compiling flag access.
    Options& options;
    // save/load result from file?
    bool save;

    // Constructed Model & Env to solve subproblem.
    Model* m;
    Env* e;
    //
    FZNPreSolverInstance* si;
  public:

    Subproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save=true);
    virtual ~Subproblem();

    void addCall(Call* c) { calls.push_back(c); }

    FunctionI* getPredicate() const { return predicate; }

    virtual void solve();

  protected:
    virtual void registerTableConstraint();

    virtual void constructModel() = 0;

    virtual void solveModel();

    virtual void replaceUsage() = 0;
  };

  class Presolver::GlobalSubproblem : public Presolver::Subproblem {
  public:
    GlobalSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : Subproblem(
            origin, origin_env, predicate, options, save) { }

  protected:
    virtual void constructModel();

    virtual void replaceUsage();
  };

  class Presolver::ModelSubproblem : public Presolver::Subproblem {
  public:
    ModelSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : Subproblem(
            origin, origin_env, predicate, options, save) { }

  protected:
    virtual void constructModel(){
      throw EvalError(origin_env, Location(), "Presolve strategy not supported yet.");
    };

    virtual void replaceUsage(){
      throw EvalError(origin_env, Location(), "Presolve strategy not supported yet.");
    };
  };

  class Presolver::CallsSubproblem : public Presolver::Subproblem {
  public:
    CallsSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : Subproblem(
            origin, origin_env, predicate, options, save) { }

  protected:
    virtual void constructModel(){
      throw EvalError(origin_env, Location(), "Presolve strategy not supported yet.");
    };

    virtual void replaceUsage(){
      throw EvalError(origin_env, Location(), "Presolve strategy not supported yet.");
    };
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
