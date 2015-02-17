/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/search.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/prettyprinter.hh> // for DEBUG only

namespace MiniZinc {
  
  SolverInstance::Status 
  interpretCombinator(Expression* comb, Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: Interpreting combinator: " << *comb << std::endl;
    
    if(Call* call = comb->dyn_cast<Call>()) {      
      if(call->id().str() == constants().combinators.and_.str()) {
        std::cout << "DEBUG: AND combinator: " << (*call) << std::endl;        
        if(call->args().size() != 1) {
          std::stringstream ssm;
          ssm << "AND-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
          throw TypeError(call->loc(), ssm.str());
        }
        if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
          assert(al->dims() == 1);
          for(unsigned int i=0; i<al->length(); i++) {
            SolverInstance::Status status = interpretCombinator(al->v()[i],env,solver);
            if(status != SolverInstance::SAT)
              return status;
          }
        } else {
          std::stringstream ssm;
          ssm << "AND-combinator takes an array as argument";
          throw TypeError(call->loc(), ssm.str());
        }        
      } 
      else if(call->id().str() == constants().combinators.or_.str()) {
        std::cout << "DEBUG: OR combinator: " << (*call) << std::endl;        
        if(call->args().size() != 1) {
          std::stringstream ssm;
          ssm << "OR-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
          throw TypeError(call->loc(), ssm.str());
        }
        SolverInstance::Status status = SolverInstance::UNKNOWN;
        bool oneIsFeasible = false;
        if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
          assert(al->dims() == 1);
          for(unsigned int i=0; i<al->length(); i++) {
            status = interpretCombinator(al->v()[i],env,solver);
            if(status == SolverInstance::SAT)
              oneIsFeasible = true;
          }
          return oneIsFeasible ? SolverInstance::SAT : status;
        } else {
          std::stringstream ssm;
          ssm << "OR-combinator takes an array as argument";
          throw TypeError(call->loc(), ssm.str());
        }                
      }
      else if(call->id().str() == constants().combinators.post.str()) {
        std::cout << "DEBUG: POST combinator: " << *call << std::endl;
        // TODO: post constraint in solver        
        return SolverInstance::UNKNOWN;
      }
      else if(call->id().str() == constants().combinators.repeat.str()) {
        std::cout << "DEBUG: REPEAT combinator: " << *call << std::endl;
        if(call->args().size() == 1) {
          SolverInstance::Status status;
          do {
            status = interpretCombinator(call->args()[0],env,solver);
          } while(status == SolverInstance::SAT);
          return status;
        }
        else if(call->args().size() == 2) { // repeat restriction: TODO: check the format-> maybe it is a comprehension
          // TODO: implement
        }
        else {
          std::stringstream ssm;
          ssm << "REPEAT-combinator only takes 1 or 2 arguments instead of " << call->args().size() << " in: " << *call;
          throw TypeError(call->loc(), ssm.str());
        }        
      }
      else if(call->id().str() == constants().combinators.scope.str()) {
        std::cout << "DEBUG: SCOPE combinator: " << *call << std::endl;       
        if(call->args().size() != 1) {
          std::stringstream ssm;
          ssm << "SCOPE-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
          throw TypeError(call->loc(), ssm.str());
        }
        // TODO: create new scope in solver: solver->newScope();
        return interpretCombinator(call->args()[0], env, solver);
      }
      else std::cout << "DEBUG: the combinator call id is: " << call->id() << ", and not: " << constants().combinators.scope << std::endl;
    }
    else if(Id* id = comb->dyn_cast<Id>()) {
      if(id->str().str() == constants().combinators.next.str()) {
        std::cout << "DEBUG: NEXT combinator: " << *id << std::endl;
        return solver->next();        
      }
    }
    // TODO: just for now...
    return SolverInstance::UNKNOWN;
  }
  
}