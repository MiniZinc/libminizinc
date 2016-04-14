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
#include <minizinc/solver_instance_base.hh>

namespace MiniZinc {
  class Presolver {
  public:
    struct Options;
    class Solns2Vector; // presolve_utils.hh
    class TableBuilder; // presolve_utils.hh
  protected:
    class Subproblem;
    class GlobalSubproblem;
    class ModelSubproblem;
    class CallsSubproblem;

    /// Model & Env in which the presolver is active
    Env& env;
    Model* model;
    /// Contains problems to be solved to presolve marked predicates
    std::vector<Subproblem*> subproblems;
    /// Option flags for flatzinc compilation
    Options& options;

    /// Searches the model's predicate for presolve annotations
    void findPresolveAnnotations();
    /// Searches the model's expressions for expressions that call a predicate to be presolved
    void findPresolvedCalls();

  public:
    /// Constructor
    Presolver(Env& env, Model* m, Options& options)
            : env(env), model(m), options(options) {}
    /// Destructor
    virtual ~Presolver();
    /// Presolves all marked predicates in the model
    void presolve();
  };

//  TODO: Group Flattener options and use those, to avoid copying.
  /// Options provides the flags from the Flattener to the Presolver
  struct Presolver::Options {
    std::vector<std::string> includePaths;
    std::string stdLibDir;
    std::string globalsDir;
    std::string modelOutput;

    bool verbose = false;
    bool newfzn = false;
    bool optimize = true;
    bool onlyRangeDomains = false;
  };


  class Presolver::Subproblem{
  protected:
    /// Model & Env in which the presolve statement occured
    Model* origin;
    EnvI& origin_env;
    /// predicate around which the submodel revolves
    FunctionI* predicate;
    /// calls to the predicate in the model
    std::vector<Call*> calls;
    /// Flattener for compiling flag access.
    Options& options;
    /// FZN solver to be used.
    std::string solver;

    /// Constructed Model & Env to solve subproblem.
    Model* m = nullptr;
    Env* e = nullptr;
    /// Solver used to solve constructed model.
    SolverInstanceBase* si = nullptr;
    Solns2Vector* solns = nullptr;

    enum Constraint { BoolTable, IntTable, Element };

  public:
    /// Constructor
    Subproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, std::string solver="");
    /// Destructor
    virtual ~Subproblem();
    /// Adds a reference to a call for the predicate to be presolved
    void addCall(Call* c) { calls.push_back(c); }
    /// Getter for the function item of the predicate to be presolved
    FunctionI* getPredicate() const { return predicate; }
    /// Solves subproblem
    virtual void solve();

  protected:
    /// Constructs MiniZinc model from subproblem
    virtual void constructModel() = 0;
    /// Solves MiniZinc model for subproblem
    virtual void solveModel();
    /// Replaces the usage of the predicate by solutions to the subproblem
    virtual void replaceUsage() = 0;
  };

  class Presolver::GlobalSubproblem : public Presolver::Subproblem {
  public:
    /// Constructor
    GlobalSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, std::string solver="") : Subproblem(
            origin, origin_env, predicate, options, solver) { }

  protected:
    /// Implements Subproblem pure virtual
    virtual void constructModel();
    /// Implements Subproblem pure virtual
    virtual void replaceUsage();
  };

  class Presolver::ModelSubproblem : public Presolver::GlobalSubproblem {
  public:
    /// Constructor
    ModelSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, std::string solver="") : GlobalSubproblem(
            origin, origin_env, predicate, options, solver) { }

  protected:
    /// Implements Subproblem pure virtual
    virtual void constructModel();
  };

  class Presolver::CallsSubproblem : public Presolver::Subproblem {
  protected:
    Call* currentCall = nullptr;

  public:
    /// Constructor
    CallsSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, std::string solver="") : Subproblem(
            origin, origin_env, predicate, options, solver) { }
    /// Override Subproblem solve
    virtual void solve();

  protected:
    /// Implements Subproblem pure virtual
    virtual void constructModel();
    /// Implements Subproblem pure virtual
    virtual void replaceUsage();
  };

}

#endif
