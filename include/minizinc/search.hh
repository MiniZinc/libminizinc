/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_SEARCH_HH__
#define __MINIZINC_SEARCH_HH__

#include <stack>

#include <minizinc/flatten.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
  
  class SearchHandler { 
  private:
    // the index of the current scope
    unsigned int _currentScope;
  protected:
    // the stack of scopes where the topmost scope is the scope of the next level
    std::vector<SolverInstanceBase*> _scopes;
    void initCurrentScopeIndex(void) { _currentScope = 0; } 
  public: 
    /// perform search on the flat model in the environement using the specified solver
    template<class SolverInstanceBase>
    void search(Env& env, MiniZinc::Options& opt) {   
      SolverInstanceBase* solver = new SolverInstanceBase(env,opt);     
      solver->processFlatZinc();
      
      SolverInstance::Status status;    
      Expression* combinator = NULL;
      if(env.flat()->solveItem()->combinator_lite()) {
        Annotation& ann = env.flat()->solveItem()->ann(); 
        for(ExpressionSetIter it = ann.begin(); it!=ann.end(); it++) {
          if(Call* c = (*it)->dyn_cast<Call>()) {
            if(c->id() == constants().ann.combinator) {
              combinator = c->args()[0];
              break;
            }
          }  
        }
        initCurrentScopeIndex();
        status = interpretCombinator(combinator, solver);
      }
      else { // solve using normal solve call
        status = solver->solve();
      }    
      // process status and solution      
      if (status==SolverInstance::SAT && !env.envi().hasSolution() ||
        status==SolverInstance::OPT) {
        env.evalOutput(std::cout);
      }
      std::cout << "----------\n";
      switch(status) {
        case SolverInstance::SAT:
          break;
        case SolverInstance::OPT:
          std::cout << "==========\n";
          break;
        case SolverInstance::UNKNOWN:
          std::cout << "=====UNKNOWN=====";
          break;
        case SolverInstance::ERROR:
          std::cout << "=====ERROR=====";
          break;
        case SolverInstance::UNSAT:
          std::cout << "=====UNSAT=====";
          break;        
      }
      std::cout << std::endl;
    }
    
  private:
    /// interpret and execute the given combinator  
  SolverInstance::Status interpretCombinator(Expression* comb, SolverInstanceBase* solver);
  /// interpret and execute an AND combinator
  SolverInstance::Status interpretAndCombinator(Call* andComb, SolverInstanceBase* solver);
   /// interpret and execute an OR combinator
  SolverInstance::Status interpretOrCombinator(Call* orComb, SolverInstanceBase* solver);
   /// interpret and execute a POST combinator
  SolverInstance::Status interpretPostCombinator(Call* postComb, SolverInstanceBase* solver);
  /// interpret and execute a REPEAT combinator
  SolverInstance::Status interpretRepeatCombinator(Call* repeatComb, SolverInstanceBase* solver);
    /// interpret and execute a SCOPE combinator
  SolverInstance::Status interpretScopeCombinator(Call* scopeComb, SolverInstanceBase* solver);
   /// interpret and execute a NEXT combinator
  SolverInstance::Status interpretNextCombinator(SolverInstanceBase* solver);
  /// interpret and execute a PRINT combinator
  SolverInstance::Status interpretPrintCombinator(SolverInstanceBase* solver);
  /// post the list of (unflattened) constraints (the argument of the POST combinator) in the solver
  bool postConstraints(Expression* cts, SolverInstanceBase* solver);
  /// overwrite the solution in \a outputToUpdate with the solution in \a output
  void updateSolution(Model* output, Model* outputToUpdate);
  
  void pushScope(SolverInstanceBase* new_scope) {
    _scopes.push_back(new_scope);
    _currentScope++;
  }
  void popScope() {
    _scopes.pop_back();
    _currentScope--;
  }
  };
 
  
}

#endif