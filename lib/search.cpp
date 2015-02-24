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
#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>

namespace MiniZinc {
  
  SolverInstance::Status 
  SearchHandler::interpretCombinator(Expression* comb, Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: Interpreting combinator: " << *comb << std::endl;
    
    if(Call* call = comb->dyn_cast<Call>()) {      
      if(call->id().str() == constants().combinators.and_.str()) {
        return interpretAndCombinator(call,env,solver);    
      } 
      else if(call->id().str() == constants().combinators.or_.str()) {
        return interpretOrCombinator(call,env,solver); 
      }
      else if(call->id().str() == constants().combinators.post.str()) {
        return interpretPostCombinator(call,env,solver);
      }
      else if(call->id().str() == constants().combinators.repeat.str()) {
        return interpretRepeatCombinator(call,env,solver);   
      }
      else if(call->id().str() == constants().combinators.scope.str()) {
        return interpretScopeCombinator(call,env,solver);
      }
      else {
        std::stringstream ssm; 
        ssm << "unknown combinator call: " << call->id();
        throw TypeError(call->loc(), ssm.str());
      }
    }
    else if(Id* id = comb->dyn_cast<Id>()) {
      if(id->str().str() == constants().combinators.next.str()) {
        return interpretNextCombinator(env,solver);
      } else {
        std::stringstream ssm; 
        ssm << "unknown combinator id: " << id->str();
        throw TypeError(id->loc(), ssm.str());
      }
    }
    else {
      std::stringstream ssm; 
      ssm << "unknown combinator expression: " << *comb;
      throw TypeError(comb->loc(), ssm.str());
    }    
  }
  
  
  SolverInstance::Status 
  SearchHandler::interpretAndCombinator(Call* call, Env& env, SolverInstanceBase* solver) {    
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
  
  SolverInstance::Status
  SearchHandler::interpretOrCombinator(Call* call, Env& env, SolverInstanceBase* solver) {
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
  
  SolverInstance::Status
  SearchHandler::interpretPostCombinator(Call* call, Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: POST combinator: " << *call << std::endl;
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "POST combinator takes only 1 argument instead of " << call->args().size() << " in " << *call ;
      throw TypeError(call->loc(), ssm.str());
    }
    if(!postConstraints(call->args()[0], env, solver)) {
      std::stringstream ssm;
      ssm << "could not post constraints: " << *(call->args()[0]) ;
      throw TypeError(call->args()[0]->loc(), ssm.str());
    }
    return SolverInstance::SAT; // well, it means that posting went well, not that there is a solution..
  }
  
  SolverInstance::Status
  SearchHandler::interpretRepeatCombinator(Call* call, Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: REPEAT combinator: " << *call << std::endl;
    if(call->args().size() == 1) {  
      // repeat is restricted by comprehension (e.g. repeat (i in 1..10) (comb) )
      if(Comprehension* compr = call->args()[0]->dyn_cast<Comprehension>()) {            
        if(compr->n_generators() != 1) {
          std::stringstream ssm;
          ssm << "REPEAT-combinator currently only supports 1 generator instead of " << compr->n_generators() << " in: " << *compr;
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
            int lb = eval_int(env.envi(), bo->lhs()).toInt();
            int ub = eval_int(env.envi(), bo->rhs()).toInt();
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
  
  SolverInstance::Status
  SearchHandler::interpretScopeCombinator(Call* call, Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: SCOPE combinator: " << *call << std::endl;       
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "SCOPE-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(call->loc(), ssm.str());
    }
    // TODO: create new scope in solver: solver->newScope();
    return interpretCombinator(call->args()[0], env, solver);
  }
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(Env& env, SolverInstanceBase* solver) {
    std::cout << "DEBUG: NEXT combinator" << std::endl;
    SolverInstance::Status status = solver->next();
    std::cout << "DEBUG: status from next: " << status << ", SAT = " << SolverInstance::SAT << std::endl;
    for(VarDeclIterator it = env.output()->begin_vardecls(); it != env.output()->end_vardecls(); ++it) {
      std::cout << "DEBUG: solution:\n" << *it << std::endl;
    }
    return status; 
  }
  
  bool 
  SearchHandler::postConstraints(Expression* cts, Env& env, SolverInstanceBase* solver) { 
    std::cout << "DEBUG: posting constraint " << *cts << std::endl;
    Expression* cts_eval = eval_par(env.envi(),cts);
    std::cout << "DEBUG: posting evaluated (par) constraint " << *cts_eval << std::endl;
    std::cout << "\n\nDEBUG: Original model before flattening:" << std::endl;
    debugprint(env.model());    
    std::cout << "\n\nDEBUG: Flattened model before flattening:" << std::endl;
    debugprint(env.flat());    
    EE ee = flat_exp(env.envi(), Ctx(), cts_eval, constants().var_true, constants().var_true);
    Expression* flat = ee.r(); // it's not really the flat expression
    std::cout << "\n\nDEBUG: Original model AFTER flattening:" << std::endl;
    debugprint(env.model());
    std::cout << "\n\nDEBUG: Flattened model AFTER flattening: " << *cts_eval << std::endl;   
    debugprint(env.flat());   
    std::cout << std::endl;
    // TODO: post flat constraint in solver (incremental or non-incremental)    
    return false;
  }
  
}