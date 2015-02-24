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
    return status; 
  }
  
  bool 
  SearchHandler::postConstraints(Expression* cts, Env& env, SolverInstanceBase* solver) {
    bool success = true;
    //std::cout << "DEBUG: BEGIN posting constraint " << *cts << std::endl;
    Expression* cts_eval = eval_par(env.envi(),cts);      
    //std::cout << "\n\nDEBUG: Flattened model before flattening:" << std::endl;
    //debugprint(env.flat());
    int nbCtsBefore = 0;
    for(ConstraintIterator it=env.flat()->begin_constraints(); it!=env.flat()->end_constraints(); ++it)
      nbCtsBefore++;
    int nbVarsBefore = 0;
    for(VarDeclIterator it=env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it)
      nbVarsBefore++;        
    
    // store the domains of each variable in an IdMap to later check changes in the domain (after flattening)
    IdMap<Expression*> domains;
    for(VarDeclIterator it = env.flat()->begin_vardecls(); it!= env.flat()->end_vardecls(); ++it) {
      Id* id = it->e()->id();
      Expression* domain = copy(it->e()->ti()->domain());
      domains.insert(id,domain);         
    }
    
    
    // flatten the expression
    EE ee = flat_exp(env.envi(), Ctx(), cts_eval, constants().var_true, constants().var_true);
    //std::cout << "\n\nDEBUG: Flattened model AFTER flattening: " << std::endl;   
    //debugprint(env.flat());    
    //std::cout<< "\n" << std::endl;
    
    int nbVarsAfter = 0;
    for(VarDeclIterator it=env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it)
      nbVarsAfter++;
    if(nbVarsBefore < nbVarsAfter) {
      std::vector<VarDecl*> vars;
      unsigned int i=0;
      for(VarDeclIterator it= env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it) {        
        if(i<nbVarsBefore) i++;
        else {
          vars.push_back(it->e());
        }
      }
      for(unsigned int i=0; i<vars.size(); i++)
        std::cout << "DEBUG: adding new variable to solver:" << *vars[i] << std::endl;
      success = success && solver->addVariables(vars);
    }      
    
    oldflatzinc(env); // TODO: make sure oldflatzinc preserves order of constraints!!
    //std::cout << "\n\nDEBUG: Flattened model AFTER calling oldflatzinc: " << std::endl;   
    //debugprint(env.flat());  
    
    int nbCtsAfter = 0;
    for(ConstraintIterator it=env.flat()->begin_constraints(); it!=env.flat()->end_constraints(); ++it)
      nbCtsAfter++;
    
        
    if(nbCtsBefore < nbCtsAfter) {       
      std::vector<Call*> flat_cts;
      int i = 0;
      for(ConstraintIterator it=env.flat()->begin_constraints(); it!=env.flat()->end_constraints(); ++it) {
        if(i<nbCtsBefore) i++;
        else {
          flat_cts.push_back(it->e()->cast<Call>());
        }
      }
      for(unsigned int i=0; i<flat_cts.size(); i++)
        std::cout << "DEBUG: adding new (flat) constraint to solver:" << *flat_cts[i] << std::endl;      
      success = success && solver->postConstraints(flat_cts);      
    }
    
    // check for variable domain updates
    for(VarDeclIterator it = env.flat()->begin_vardecls(); it!= env.flat()->end_vardecls(); ++it) {
      Id* id = it->e()->id();
      Expression* domain = it->e()->ti()->domain();          
      IdMap<Expression*>::iterator iter = domains.find(id);      
      if(iter != domains.end()) {
        Expression* oldDomain = iter->second; 
        if(oldDomain) {          
          if(SetLit* sl_old = oldDomain->dyn_cast<SetLit>()) {            
            if(SetLit* sl_new = domain->dyn_cast<SetLit>()) {                         
              int lb_old = sl_old->isv()->min().toInt();
              int lb_new = sl_new->isv()->min().toInt();
              int ub_old = sl_old->isv()->max().toInt();
              int ub_new = sl_new->isv()->max().toInt();
              bool updateBounds = (lb_old != lb_new || ub_old != ub_new);
              if(updateBounds) {                
                success = success && solver->updateIntBounds(id->decl(),lb_new,ub_new);
                std::cout << "DEBUG: updated int bounds of " << *id << " in solver" << std::endl;
              }             
            }
          }
          else {
          // TODO: check for boolean and floating point bounds
          }
        }
      }
    }
      
    return success; // TODO: change as soon as it works in the solvers
  }
  
}