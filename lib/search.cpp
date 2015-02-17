/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <typeinfo> // just for debugging

#include <minizinc/search.hh>
#include <minizinc/solver_instance_base.hh>
#include <minizinc/prettyprinter.hh> // for DEBUG only
#include <minizinc/eval_par.hh>

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
          return SolverInstance::SAT;
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
        std::cout << "DEBUG: POST combinator, returning SAT: " << *call << std::endl;
        // TODO: post constraint in solver        
        return SolverInstance::SAT;
        //return SolverInstance::UNKNOWN;
      }
      else if(call->id().str() == constants().combinators.repeat.str()) {
        std::cout << "DEBUG: REPEAT combinator: " << *call << std::endl;
        if(call->args().size() == 1) {          
          if(Comprehension* compr = call->args()[0]->dyn_cast<Comprehension>()) {            
            if(compr->n_generators() != 1) {
              std::stringstream ssm;
              ssm << "REPEAT-combinator only takes 1 generator instead of " << compr->n_generators() << " in:" << *compr;
              throw TypeError(compr->loc(), ssm.str());
            }
            else {
              Expression* in = compr->in(0);
              int nbIterations = 0;              
              if(!in->type().ispar()) {
                std::stringstream ssm;
                ssm << "The generator expression \"" << *in << "\" has to be par";
                throw TypeError(in->loc(), ssm.str());
              }              
              if(BinOp* bo = in->dyn_cast<BinOp>()) {                
                int lb = eval_int(bo->lhs()).toInt();
                int ub = eval_int(bo->rhs()).toInt();
                nbIterations = ub - lb + 1;
              } 
              else {
                std::stringstream ssm;
                ssm << "Expected set literal of the form \"(lb..ub)\" instead of \"" << *in << "\"";
                throw TypeError(in->loc(), ssm.str());
              }
              SolverInstance::Status status = SolverInstance::UNKNOWN;
              // repeat the argument a limited number of times
              for(unsigned int i = 0; i<nbIterations; i++) {
                std::cout << "DEBUG: repeating combinator " << *(compr->e()) << " for " << i << "/" << (nbIterations-1) << " times" << std::endl;
                status = interpretCombinator(compr->e(),env,solver);
                if(status != SolverInstance::SAT)                  
                  return status;                
              }
            }            
          }          
          else { // repeat is only restricted by satisfiability
            SolverInstance::Status status = SolverInstance::UNKNOWN;
            do {
              status = interpretCombinator(call->args()[0],env,solver);
            } while(status == SolverInstance::SAT);
            return status;
          }
        }
        else {
          std::stringstream ssm;
          ssm << "REPEAT-combinator only takes 1 array as argument instead of " << call->args().size() << " arguments in: " << *call;
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
        SolverInstance::Status status = solver->next();
        std::cout << "DEBUG: status from next: " << status << ", SAT = " << SolverInstance::SAT << std::endl;
        return status; //solver->next();   
      }
    }
    // TODO: just for now...
    return SolverInstance::UNKNOWN;
  }
  
}