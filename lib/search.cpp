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
  SearchHandler::interpretCombinator(Expression* comb0, SolverInstanceBase* solver) {
    Env& env = solver->env();
    Expression* comb = eval_par(env.envi(), comb0);
    //std::cout << "DEBUG: Interpreting combinator: " << *comb << std::endl;
    
    if(Call* call = comb->dyn_cast<Call>()) {      
      if(call->id().str() == constants().combinators.and_.str()) {
        return interpretAndCombinator(call,solver);    
      } 
      else if(call->id().str() == constants().combinators.or_.str()) {
        return interpretOrCombinator(call,solver); 
      }
      else if(call->id().str() == constants().combinators.post.str()) {
        return interpretPostCombinator(call,solver);
      }
      else if(call->id().str() == constants().combinators.repeat.str()) {
        return interpretRepeatCombinator(call,solver);   
      }
      else if(call->id().str() == constants().combinators.scope.str()) {
        return interpretScopeCombinator(call,solver);
      }
      else if(call->id().str() == constants().combinators.print.str()) {
        return interpretPrintCombinator(solver);
      }
      else if(call->id().str() == constants().combinators.next.str()) {
        return interpretNextCombinator(solver);
      }
      else {
        std::stringstream ssm; 
        ssm << "unknown combinator call: " << call->id();
        throw TypeError(call->loc(), ssm.str());
      }
    }
    else if(Id* id = comb->dyn_cast<Id>()) {
      if(id->str().str() == constants().combinators.next.str()) {
        return interpretNextCombinator(solver);
      } 
      else if (id->str().str() == constants().combinators.print.str()) {
        return interpretPrintCombinator(solver);
      }
      else {
        std::stringstream ssm; 
        ssm << "unknown combinator id: " << id->str();
        throw TypeError(id->loc(), ssm.str());
      }
    }
    else {
      std::stringstream ssm; 
      ssm << "unknown combinator: " << *comb;
      throw TypeError(comb->loc(), ssm.str());
    }    
  }
  
  
  SolverInstance::Status 
  SearchHandler::interpretAndCombinator(Call* call, SolverInstanceBase* solver) {    
    //std::cout << "DEBUG: AND combinator: " << (*call) << std::endl;    
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "AND-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(call->loc(), ssm.str());
    }
    if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
      assert(al->dims() == 1);
      for(unsigned int i=0; i<al->length(); i++) {
        SolverInstance::Status status = interpretCombinator(al->v()[i],solver);
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
  SearchHandler::interpretOrCombinator(Call* call, SolverInstanceBase* solver) {
    //std::cout << "DEBUG: OR combinator: " << (*call) << std::endl;        
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
        status = interpretCombinator(al->v()[i],solver);
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
  SearchHandler::interpretPostCombinator(Call* call, SolverInstanceBase* solver) {   
    //std::cout << "DEBUG: POST combinator: " << *call << std::endl;
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "POST combinator takes only 1 argument instead of " << call->args().size() << " in " << *call ;
      throw TypeError(call->loc(), ssm.str());
    }
    if(!postConstraints(call->args()[0], solver)) {
      std::stringstream ssm;
      ssm << "could not post constraints: " << *(call->args()[0]) ;
      throw TypeError(call->args()[0]->loc(), ssm.str());
    }
    return SolverInstance::SAT; // well, it means that posting went well, not that there is a solution..
  }
  
  SolverInstance::Status
  SearchHandler::interpretRepeatCombinator(Call* call, SolverInstanceBase* solver) {
    Env& env = solver->env();
    //std::cout << "DEBUG: REPEAT combinator: " << *call << std::endl;
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
            //std::cout << "DEBUG: repeating combinator " << *(compr->e()) << " for " << (i+1) << "/" << (nbIterations) << " times" << std::endl;            
            status = interpretCombinator(compr->e(),solver);
            if(status != SolverInstance::SAT) {             
              return status;
            }
          }
          return status;
        }            
      }          
      else { // repeat is only restricted by satisfiability
        SolverInstance::Status status = SolverInstance::UNKNOWN;
        do {
          status = interpretCombinator(call->args()[0], solver);
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
  SearchHandler::interpretScopeCombinator(Call* call, SolverInstanceBase* solver) {
    //std::cerr << "DEBUG: SCOPE combinator" << std::endl;   
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "SCOPE-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(call->loc(), ssm.str());
    }
    // if this is a nested scope
    if(!_scopes.empty()) {
      //std::cerr << "DEBUG: Opening new nested scope" << std::endl;
      solver->env().combinator = call->args()[0];
      SolverInstanceBase* solver_copy = solver->copy();
      //std::cerr << "DEBUG: Copied solver instance" << std::endl;
      pushScope(solver_copy);
      SolverInstance::Status status = interpretCombinator(solver_copy->env().combinator, solver_copy);
      popScope();
      //std::cerr << "DEBUG: Closed nested scope" << std::endl;
      return status;
    } // this is not a nested scope
    else {
     //std::cerr << "DEBUG: Opening high-level scope" << std::endl;
     pushScope(solver);
     SolverInstance::Status status = interpretCombinator(call->args()[0], solver);
     popScope();
     return status;
    }    
  }
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(SolverInstanceBase* solver) {
    //std::cerr << "DEBUG: NEXT combinator" << std::endl;   
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SAT) {      
      solver->env().envi().hasSolution(true);
      // set/update the solutions in all higher scopes
      for(unsigned int i = 0; i <_scopes.size(); i++) {
        _scopes[i]->env().envi().hasSolution(true);
        updateSolution(solver->env().output(), _scopes[i]->env().output());
      }
    }    
    return status; 
  }
  
  SolverInstance::Status
  SearchHandler::interpretPrintCombinator(SolverInstanceBase* solver) {
    //std::cerr << "DEBUG: PRINT combinator" << std::endl;   
    if(solver->env().envi().hasSolution()) {      
      solver->env().evalOutput(std::cout);      
      std::cout << constants().solver_output.solution_delimiter << std::endl;     
      return SolverInstance::SAT;
    }
    else {      
      throw EvalError(Location(), "No solution found to be printed by PRINT-combinator");      
    }
  }  
  
  bool 
  SearchHandler::postConstraints(Expression* cts, SolverInstanceBase* solver) {
    Env& env = solver->env();
    bool success = true;
    //std::cout << "DEBUG: BEGIN posting constraint: " << *cts << std::endl;    
  
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
    EE ee = flat_exp(env.envi(), Ctx(), cts, constants().var_true, constants().var_true);  
    //std::cout << "\n\nDEBUG: Flattened model AFTER flattening: " << std::endl;   
    //debugprint(env.flat());    
    //std::cout<< "\n" << std::endl;
    //std::cout << "\n\nDEBUG: Flattened model on higher scope: ******************: " << std::endl;   
    //debugprint(_scopes[0]->env().flat());
    
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
      //for(unsigned int i=0; i<vars.size(); i++) {
       // std::cout << "DEBUG: adding new variable to solver:" << *vars[i] << std::endl;
      //}        
      success = success && solver->addVariables(vars);      
    }      
    
    oldflatzinc(env); // TODO: make sure oldflatzinc preserves order of constraints!!
    // std::cout << "\n\nDEBUG: Flattened model AFTER calling oldflatzinc: " << std::endl;   
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
      //for(unsigned int i=0; i<flat_cts.size(); i++)
        //std::cout << "DEBUG: adding new (flat) constraint to solver:" << *flat_cts[i] << std::endl;      
      success = success && solver->postConstraints(flat_cts);      
    }
    
    bool updateBoundsOnce = false;
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
              updateBoundsOnce = updateBounds || updateBoundsOnce;
              if(updateBounds) {
                //std::cout << "DEBUG: updating intbounds of \"" << *(id->decl()) << "\" to new bounds: (" << lb_new << ", " << ub_new << ")"  << std::endl;
                success = success && solver->updateIntBounds(id->decl(),lb_new,ub_new);
                //std::cout << "DEBUG: updated int bounds (" << lb_new << "," << ub_new << " ) of " << *id << " in solver" << std::endl;
              }  
            }
          }
          else {
          // TODO: check for boolean and floating point bounds
          }
        }
      }
    }
     
  
    if(!updateBoundsOnce && nbCtsBefore == nbCtsAfter && nbVarsBefore == nbVarsAfter) {
      std::cerr << "WARNING: flat model did not change after posting constraint: " << *cts << std::endl;
    }
              
     
    return success; 
  }
  
  void
  SearchHandler::updateSolution(Model* output, Model* outputToUpdate) {    
    IdMap<Expression*> solutions;
    for(unsigned int i=0; i<output->size(); i++) {
      if(VarDeclI* vdi = (*output)[i]->dyn_cast<VarDeclI>()) {
        solutions.insert(vdi->e()->id(),vdi->e()->e());
      }
    }
    for(unsigned int i=0; i<outputToUpdate->size(); i++) {
      if(VarDeclI* vdi = (*outputToUpdate)[i]->dyn_cast<VarDeclI>()) {
        // update the solution for the identifiers that exist in the other solution
        Id* id = vdi->e()->id();
        if(solutions.find(id) != solutions.end()) {
          vdi->e()->e(solutions.get(id));
        }
      }
    }    
    //throw InternalError("SearchHandler::updateSolution: Could not update solutions in output models");   
  }
  
}