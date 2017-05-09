/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jip J. Dekker <jip@dekker.li>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */
//

#ifndef __MINIZINC_PRESOLVE_UTILS_HH__
#define __MINIZINC_PRESOLVE_UTILS_HH__

#include <minizinc/presolve.hh>
#include <minizinc/model.hh>
#include <minizinc/solns2out.hh>

namespace MiniZinc {

  /// Returns true if the function contains a paramter argument.
  bool paramArgument(FunctionI* i);

  /// Registers function and all dependencies to model
  void recursiveRegisterFns(Model*, EnvI&, FunctionI*);

  /// Computes decision variable domain for a given variable expression
  Expression* computeDomainExpr(EnvI& env, Expression* exp);

  /// Computes the array ranges for an given array expression and add these ranges to the given vector
  void computeRanges(EnvI& env, CopyMap& cm, Expression* exp, std::vector<TypeInst*>& ranges);

  /// Generates the flatzinc model given the environment and given flags
  void generateFlatZinc(Env& env, bool rangeDomains, bool optimizeFZN, bool newFZN);

  /// Extension to Solns2Out that stores all results in a vector to be for a different environment
  class Presolver::Solns2Vector : public Solns2Out {
  protected:
    EnvI& copyEnv;

//    TODO: using ASTStringMap don't work, ASTStrings don't compare correctly
    std::vector< UNORDERED_NAMESPACE::unordered_map<std::string, Expression*>* > solutions;

    std::vector<KeepAlive> GCProhibitors;
  public:
    /// Default constructor
    Solns2Vector(Env* e, EnvI& forEnv) : copyEnv(forEnv) { this->initFromEnv(e); }
    /// Destructor
    virtual ~Solns2Vector() { for (int i = 0; i < solutions.size(); ++i) delete solutions[i]; }
    /// Getter for all gathered solutions
    const std::vector< UNORDERED_NAMESPACE::unordered_map<std::string, Expression*>* >& getSolutions() const { return solutions; }

  protected:
    /// Override of evalOutput, which stores a result in the solutions vector
    virtual bool evalOutput();
    virtual bool evalStatus(SolverInstance::Status status) {return true;}
  };

  /// Helper class to construct a table constraint
  class Presolver::TableBuilder {
  protected:
    bool boolTable = false;
    long long int rows;

    Expression* variables = nullptr;
    Call* dataCall = nullptr;

    std::vector<Expression*> vVariables;
    std::vector<Expression*> data;

    EnvI& env;
    Model* m;
    Flattener* flattener;
  public:
    /// Default constructor
    TableBuilder(EnvI& env, Model* m, Flattener* flattener, bool boolTable)
            : env(env), m(m), flattener(flattener), boolTable(boolTable) { };
    /// Constructs full table constraint from function item, solution, and optionally given table variable arguments
    void buildFromSolver(FunctionI* f, Solns2Vector* solns, ASTExprVec<Expression> variables = ASTExprVec<Expression>());
    /// Adds variable to the table constraint
    void addVariable(Expression* var);
    /// Reset variables
    void resetVariables() { variables = nullptr; vVariables.clear(); }
    /// Adds a data point to the table constraint
    /// (this function must be called alternating the variables)
    void addData(Expression* dat);
    void resetData() { dataCall = nullptr; data.clear(); }
    /// Returns a call to the generated table constraint
    Call* getExpression();
    /// Sets the number of number of points of data for every variable
    void setRows(long long int rows) { rows = rows; }

  protected:
    /// Adds all currently gathered variable to the variables expression
    void storeVars();
    /// Registers the table predicate in the model
    void registerTableConstraint();
  };

}

#endif //__MINIZINC_PRESOLVE_UTILS_HH__
