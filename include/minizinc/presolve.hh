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
#include <minizinc/solns2out.h>

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

//  TODO: Group Flattener options and use those, to avoid copying.
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
  public:
    class Solns2Vector;
    class TableExpressionBuilder;
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
    Model* m = nullptr;
    Env* e = nullptr;
    // Solver used to solve constructed model.
    SolverInstanceBase* si = nullptr; //TODO: More resonable default when using one of the specific solvers
    Solns2Vector* solns = nullptr;

    enum Constraint { BoolTable, IntTable, Element };

  public:

    Subproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save=false);
    virtual ~Subproblem();

    void addCall(Call* c) { calls.push_back(c); }

    FunctionI* getPredicate() const { return predicate; }

    virtual void solve();

    static void registerFns(Model*, EnvI&, FunctionI*);

  protected:
    virtual void constructModel() = 0;

    virtual void solveModel();

    virtual void replaceUsage() = 0;

    Expression* computeBounds(Expression* exp);


  };

  class Presolver::GlobalSubproblem : public Presolver::Subproblem {
  public:
    GlobalSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : Subproblem(
            origin, origin_env, predicate, options, save) { }

  protected:
    virtual void constructModel();

    virtual void replaceUsage();
  };

  class Presolver::ModelSubproblem : public Presolver::GlobalSubproblem {
  public:
    ModelSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : GlobalSubproblem(
            origin, origin_env, predicate, options, save) { }

  protected:
    virtual void constructModel();
  };

  class Presolver::CallsSubproblem : public Presolver::Subproblem {
  protected:
    Call* currentCall = nullptr;
    std::vector<VarDecl*> modelArgs;

  public:
    CallsSubproblem(Model* origin, EnvI& origin_env, FunctionI* predicate, Options& options, bool save) : Subproblem(
            origin, origin_env, predicate, options, save) { }

    virtual void solve();

  protected:
    virtual void constructModel();

    virtual void replaceUsage();


  };

  class Presolver::Subproblem::Solns2Vector : public Solns2Out {
  protected:
    EnvI& copyEnv;

//    TODO: using ASTStringMap don't work, ASTStrings don't compare correctly
    std::vector< std::unordered_map<std::string, Expression*>* > solutions;

    std::vector<KeepAlive> GCProhibitors;
  public:
    Solns2Vector(Env* e, EnvI& forEnv) : copyEnv(forEnv) { this->initFromEnv(e); }

    virtual ~Solns2Vector() { for (int i = 0; i < solutions.size(); ++i) delete solutions[i]; }

    const std::vector< std::unordered_map<std::string, Expression*>* >& getSolutions() const { return solutions; }

  protected:
    virtual bool evalOutput();
    virtual bool evalStatus(SolverInstance::Status status) {return true;}
  };

  class Presolver::Subproblem::TableExpressionBuilder {
  protected:
    bool boolTable = false;
    long long int rows;

    Expression* variables = nullptr;

    std::vector<Expression*> vVariables;
    std::vector<Expression*> data;

    EnvI& env;
    Model* m;
    Options& options;
  public:
    TableExpressionBuilder(EnvI& env, Model* m, Options& options, bool boolTable)
            : env(env), m(m), options(options), boolTable(boolTable) { };

    void buildFromSolver(FunctionI* f, Solns2Vector* solns, ASTExprVec<Expression> variables = ASTExprVec<Expression>());

    void addVariable(Expression* var);

    void addData(Expression* dat);

    Call* getExpression();

    void setRows(long long int rows) { rows = rows; }

  protected:
    void storeVars();

    void registerTableConstraint();
  };
}

#endif
