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
  SearchHandler::interpretCombinator(Expression* comb, SolverInstanceBase* solver, bool verbose) {
    Env& env = solver->env();  
    //std::cout << "DEBUG: Interpreting combinator: " << *comb << std::endl;
    
    if(Call* call = comb->dyn_cast<Call>()) {      
      if(call->id() == constants().combinators.and_) {
        return interpretAndCombinator(call,solver,verbose);    
      } 
      else if(call->id() == constants().combinators.or_) {
        return interpretOrCombinator(call,solver,verbose); 
      }
      else if(call->id() == constants().combinators.post) {
        return interpretPostCombinator(call,solver,verbose);
      }
      else if(call->id() == constants().combinators.repeat) {
        return interpretRepeatCombinator(call,solver,verbose);   
      }
      else if(call->id() == constants().combinators.scope) {
        return interpretScopeCombinator(call,solver,verbose);
      }
      else if(call->id() == constants().combinators.print) {
        if(verbose)
          std::cout << "DEBUG: PRINT combinator in " << call->loc() << std::endl;
        return interpretPrintCombinator(solver,verbose);
      }
      else if(call->id() == constants().combinators.next) {
        return interpretNextCombinator(call, solver,verbose);
      }
      else {
        std::vector<Expression*> previousParameters(call->decl()->params().size());
        for (unsigned int i=call->decl()->params().size(); i--;) {
          VarDecl* vd = call->decl()->params()[i];
          previousParameters[i] = vd->e();
          vd->flat(vd);
          vd->e(eval_par(env.envi(), call->args()[i]));
        }
        
        SolverInstance::Status ret;
        
        if(call->decl()->e()) {      
          if(verbose) 
            std::cerr << "DEBUG: interpreting combinator " << *call << " according to its defined body." << std::endl;
          ret = interpretCombinator(call->decl()->e(), solver,verbose);
        } else { 
          if(verbose) 
            std::cerr << "DEBUG: interpreting combinator " << *call << " according to its solver implementation." << std::endl;
          if(call->id() == constants().combinators.best_max || 
            call->id() == constants().combinators.best_min) {
            bool print = false;
            if(call->args().size() > 1)
              if(Id* id = call->args()[1]->dyn_cast<Id>()) 
                if(id->str() == constants().combinators.print)
                  print = true;
              
            ret = interpretBestCombinator(call, solver, call->id() == constants().combinators.best_min, verbose, print);            
          }
          else {          
            std::stringstream ssm; 
            ssm << "No body for combinator: " << *call;
            throw TypeError(env.envi(), call->loc(), ssm.str());
          }
        }

        for (unsigned int i=call->decl()->params().size(); i--;) {
          VarDecl* vd = call->decl()->params()[i];
          vd->e(previousParameters[i]);
          vd->flat(vd->e() ? vd : NULL);
        }
        return ret;
      }
    }
    else if(Id* id = comb->dyn_cast<Id>()) {
      Expression* id_e = follow_id_to_value(id);
      Id* ident = id_e->dyn_cast<Id>();
      if(ident && ident->idn()==-1 && ident->v() == constants().combinators.next) {
        return interpretNextCombinator(solver,verbose);
      } 
      else if (ident && ident->idn()==-1 && ident->v() == constants().combinators.print) {
        return interpretPrintCombinator(solver,verbose);
      }
      else {
        std::stringstream ssm; 
        ssm << "unknown combinator id: " << *ident;
        throw TypeError(env.envi(), ident->loc(), ssm.str());
      }
    }
    else {
      std::stringstream ssm; 
      ssm << "unknown combinator: " << *comb;
      throw TypeError(env.envi(), comb->loc(), ssm.str());
    }    
  }
  
  
  SolverInstance::Status 
  SearchHandler::interpretAndCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {    
    //std::cout << "DEBUG: AND combinator: " << (*call) << std::endl;    
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "AND-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }
    if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
      assert(al->dims() == 1);
      for(unsigned int i=0; i<al->length(); i++) {
        SolverInstance::Status status = interpretCombinator(al->v()[i],solver,verbose);
        if(status != SolverInstance::SAT)               
          return status;            
      }
      return SolverInstance::SAT;
    } else {
      std::stringstream ssm;
      ssm << "AND-combinator takes an array as argument";
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }     
  }
  
  SolverInstance::Status 
  SearchHandler::interpretBestCombinator(Call* call, SolverInstanceBase* solver, bool minimize, bool print, bool verbose) {
    if(call->args().size() == 0 || call->args().size() > 2) {
      std::stringstream ssm;
      ssm << call->id() << "-combinator takes at least 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }
    if(call->args().size() == 2) {
      interpretLimitCombinator(call->args()[1],solver,verbose);
    }
     
    VarDecl* decl; // TODO: use flat variable decl?
    if(Id* id = call->args()[0]->dyn_cast<Id>()) {
      decl = id->decl();
      if(decl->e()) { // if there is a right hand side
        if(Id* id_rhs = decl->e()->dyn_cast<Id>()) {
          if(id_rhs->decl())
            decl = id_rhs->decl();
        }
        else { // TODO : ArrayAccess??
        }
      }
    }
    else {
      std::stringstream ssm;
      ssm << "Expected identifier instead of " << *(call->args()[0]) << " in " << *call;
      throw TypeError(solver->env().envi(), call->args()[0]->loc(), ssm.str());
    }    
    
    return solver->best(decl,minimize,print); 
  }
  
  SolverInstance::Status
  SearchHandler::interpretOrCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    //std::cout << "DEBUG: OR combinator: " << (*call) << std::endl;        
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "OR-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }
    SolverInstance::Status status = SolverInstance::UNKNOWN;
    bool oneIsFeasible = false;
    if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
      assert(al->dims() == 1);
      for(unsigned int i=0; i<al->length(); i++) {
        status = interpretCombinator(al->v()[i],solver,verbose);
        if(status == SolverInstance::SAT)
          oneIsFeasible = true;
      }
      return oneIsFeasible ? SolverInstance::SAT : status;
    } else {
      std::stringstream ssm;
      ssm << "OR-combinator takes an array as argument";
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }          
  }
  
  SolverInstance::Status
  SearchHandler::interpretPostCombinator(Call* call, SolverInstanceBase* solver,bool verbose) {
    //std::cout << "DEBUG: POST combinator: " << *call << std::endl;
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "POST combinator takes only 1 argument instead of " << call->args().size() << " in " << *call ;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }
    if(!postConstraints(call->args()[0], solver,verbose)) {
      std::stringstream ssm;
      ssm << "could not post constraints: " << *(call->args()[0]) ;
      throw TypeError(solver->env().envi(),call->args()[0]->loc(), ssm.str());
    }
    return SolverInstance::SAT; // well, it means that posting went well, not that there is a solution..
  }
  
  SolverInstance::Status
  SearchHandler::interpretRepeatCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    Env& env = solver->env();
    //std::cout << "DEBUG: REPEAT combinator: " << *call << std::endl;
    if(call->args().size() == 1) {  
      // repeat is restricted by comprehension (e.g. repeat (i in 1..10) (comb) )
      if(Comprehension* compr = call->args()[0]->dyn_cast<Comprehension>()) {            
        if(compr->n_generators() != 1) {
          std::stringstream ssm;
          ssm << "REPEAT-combinator currently only supports 1 generator instead of " << compr->n_generators() << " in: " << *compr;
          throw TypeError(solver->env().envi(),compr->loc(), ssm.str());
        }
        else {
          Expression* in = compr->in(0);
          int nbIterations = 0;              
          if(!in->type().ispar()) {
            std::stringstream ssm;
            ssm << "The generator expression \"" << *in << "\" has to be par";
            throw TypeError(solver->env().envi(),in->loc(), ssm.str());
          }              
          if(BinOp* bo = in->dyn_cast<BinOp>()) {                
            int lb = eval_int(env.envi(), bo->lhs()).toInt();
            int ub = eval_int(env.envi(), bo->rhs()).toInt();
            nbIterations = ub - lb + 1;
          } 
          else {
            std::stringstream ssm;
            ssm << "Expected set literal of the form \"(lb..ub)\" instead of \"" << *in << "\"";
            throw TypeError(solver->env().envi(),in->loc(), ssm.str());
          }
          SolverInstance::Status status = SolverInstance::UNKNOWN;
          // repeat the argument a limited number of times
          for(unsigned int i = 0; i<nbIterations; i++) {
            if(verbose)
              std::cout << "DEBUG: repeating combinator " << *(compr->e()) << " for " << (i+1) << "/" << (nbIterations) << " times" << std::endl;            
            status = interpretCombinator(compr->e(),solver,verbose);
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
          status = interpretCombinator(call->args()[0], solver,verbose);
        } while(status == SolverInstance::SAT);
        return status;
      }
    }
    else {
      std::stringstream ssm;
      ssm << "REPEAT-combinator only takes 1 array as argument instead of " << call->args().size() << " arguments in: " << *call;
      throw TypeError(solver->env().envi(),call->loc(), ssm.str());
    }            
  }
  
  SolverInstance::Status
  SearchHandler::interpretScopeCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: SCOPE combinator" << std::endl;   
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "SCOPE-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(),call->loc(), ssm.str());
    }   
    //std::cerr << "DEBUG: Opening new nested scope" << std::endl;
    solver->env().combinator = call->args()[0];
    SolverInstanceBase* solver_copy = solver->copy();
    //std::cerr << "DEBUG: Copied solver instance" << std::endl;
    pushScope(solver_copy);
    SolverInstance::Status status = interpretCombinator(solver_copy->env().combinator, solver_copy, verbose);
    popScope();
    //std::cerr << "DEBUG: Closed nested scope" << std::endl;
    return status;
  }
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(SolverInstanceBase* solver, bool verbose) {
   // std::cerr << "DEBUG: NEXT combinator" << std::endl;   
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SAT) {      
      solver->env().envi().hasSolution(true);
      // set/update the solutions in all higher scopes
      for(unsigned int i = 0; i <_scopes.size(); i++) {
        _scopes[i]->env().envi().hasSolution(true);
        updateSolution(solver->env().output(), _scopes[i]->env().output());
      }
    }
    //std::cerr << "DEBUG: solver returned status " << status << " (SAT = " << SolverInstance::SAT << ")" << std::endl;
    return status; 
  }
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {   
    // interpret NEXT arguments    
    ASTExprVec<Expression> args = call->args();
    if(args.size()>1) {
      std::stringstream ssm;
      ssm << "NEXT-combinator takes at most 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(),call->loc(), ssm.str());      
    } 
    if(args.size() > 0)
      interpretLimitCombinator(args[0],solver,verbose);
      
    // get next solution
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SAT) {      
      solver->env().envi().hasSolution(true);
      // set/update the solutions in all higher scopes
      for(unsigned int i = 0; i <_scopes.size(); i++) {
        _scopes[i]->env().envi().hasSolution(true);
        updateSolution(solver->env().output(), _scopes[i]->env().output());
      }
    }
    //std::cerr << "DEBUG: solver returned status " << status << " (SAT = " << SolverInstance::SAT << ")" << std::endl;
    return status; 
  }
  
  void 
  SearchHandler::interpretLimitCombinator(Expression* e, SolverInstanceBase* solver, bool verbose) {
    if(Call* c = e->dyn_cast<Call>()) {
      if(c->id() == constants().combinators.limit_time) 
        interpretTimeLimitCombinator(c,solver,verbose);
      else if(c->id() == constants().combinators.limit_nodes)
        interpretNodeLimitCombinator(c,solver,verbose);
      else if(c->id() == constants().combinators.limit_fails)
        interpretFailLimitCombinator(c,solver,verbose);   
//      else if(c->id() == constants().combinators.limit_solutions)
//        interpretSolutionLimitCombinator(c,solver,verbose);
      else 
        std::cerr << "WARNING: Ignoring unknown argument to next:" << *c << std::endl;
    }
    else if(ArrayLit* al = e->dyn_cast<ArrayLit>()) {
      ASTExprVec<Expression> elems = al->v();
      for(unsigned int j=0; j<elems.size(); j++) {
        if(Call* c = elems[j]->dyn_cast<Call>()) {
          if(c->id() == constants().combinators.limit_time) 
            interpretTimeLimitCombinator(c,solver,verbose);
          else if(c->id() == constants().combinators.limit_nodes)
            interpretNodeLimitCombinator(c,solver,verbose);
          else if(c->id() == constants().combinators.limit_fails) 
            interpretFailLimitCombinator(c,solver,verbose);  
//          else if(c->id() == constants().combinators.limit_solutions)
//            interpretSolutionLimitCombinator(c,solver,verbose);
          else 
            std::cerr << "WARNING: Ignoring unknown argument to next:" << *c << std::endl;
        }
      }
    }
    else {
      std::cerr << "WARNING: Ignoring unknown argument to next:" << *e << std::endl;
    }
    
  }
  
  void
  SearchHandler::interpretFailLimitCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {    
    ASTExprVec<Expression> args = call->args();
    if(args.size() != 1) {
      std::stringstream ssm; 
      ssm << "Expecting 1 argument in call: " << *call;
      throw EvalError(solver->env().envi(),call->loc(), ssm.str());
    }     
    args[0] = eval_par(solver->env().envi(),args[0]);
    Options& opt = solver->getOptions();
    if(IntLit* il = args[0]->dyn_cast<IntLit>()) {      
      KeepAlive ka(il);
      opt.setIntParam(constants().solver_options.fail_limit.str(),ka);
    }   
    else {
      std::stringstream ssm; 
      ssm << "Cannot process argument. Expecting integer value instead of: " << *args[0];
      throw EvalError(solver->env().envi(),args[0]->loc(), ssm.str());
    }    
  }
  
  void
  SearchHandler::interpretNodeLimitCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {    
    ASTExprVec<Expression> args = call->args();
    if(args.size() != 1) {
      std::stringstream ssm; 
      ssm << "Expecting 1 argument in call: " << *call;
      throw EvalError(solver->env().envi(),call->loc(), ssm.str());
    }     
    args[0] = eval_par(solver->env().envi(),args[0]);
    Options& opt = solver->getOptions();
    if(IntLit* il = args[0]->dyn_cast<IntLit>()) {      
      KeepAlive ka(il);
      opt.setIntParam(constants().solver_options.node_limit.str(),ka);
    }   
    else {
      std::stringstream ssm; 
      ssm << "Cannot process argument. Expecting integer value instead of: " << *args[0];
      throw EvalError(solver->env().envi(),args[0]->loc(), ssm.str());
    }    
  }
  
  
  void 
  SearchHandler::interpretSolutionLimitCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    ASTExprVec<Expression> args = call->args();
    if(args.size() != 1) {
      std::stringstream ssm; 
      ssm << "Expecting 1 argument in call: " << *call;
      throw EvalError(solver->env().envi(),call->loc(), ssm.str());
    }     
    args[0] = eval_par(solver->env().envi(),args[0]);
    Options& opt = solver->getOptions();
    if(IntLit* il = args[0]->dyn_cast<IntLit>()) {      
      KeepAlive ka(il);      
      opt.setIntParam(constants().solver_options.solution_limit.str(),ka);
    }   
    else {
      std::stringstream ssm; 
      ssm << "Cannot process argument. Expecting integer value instead of: " << *args[0];
      throw EvalError(solver->env().envi(),args[0]->loc(), ssm.str());
    }        
  }
  
  void
  SearchHandler::interpretTimeLimitCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    ASTExprVec<Expression> args = call->args();
    if(args.size() != 1) {
      std::stringstream ssm; 
      ssm << "Expecting 1 argument in call: " << *call;
      throw EvalError(solver->env().envi(),call->loc(), ssm.str());
    }     
    args[0] = eval_par(solver->env().envi(),args[0]);
    Options& opt = solver->getOptions();
    if(IntLit* il = args[0]->dyn_cast<IntLit>()) {
      int time = il->v().toInt();
      if(verbose)
        std::cerr << "DEBUG: setting time limit: " << time << "ms" << std::endl;
      opt.setIntParam(constants().solver_options.time_limit_ms.str(),time);
    }
    else {
      std::stringstream ssm; 
      ssm << "Cannot process argument. Expecting integer value instead of: " << *args[0];
      throw EvalError(solver->env().envi(),args[0]->loc(), ssm.str());
    }    
  }
  
  SolverInstance::Status
  SearchHandler::interpretPrintCombinator(SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: PRINT combinator" << std::endl;   
    if(solver->env().envi().hasSolution()) {      
      solver->env().evalOutput(std::cout);      
      std::cout << constants().solver_output.solution_delimiter << std::endl;     
      return SolverInstance::SAT;
    }
    else {
      if(verbose)
        std::cerr << "No solution found to be printed by PRINT-combinator" << std::endl;      
     return SolverInstance::UNSAT;
    }
  }  
  
  bool 
  SearchHandler::postConstraints(Expression* cts, SolverInstanceBase* solver, bool verbose) {
    Env& env = solver->env();
    bool success = true;
    if(verbose)
      std::cout << "DEBUG: BEGIN posting constraint: " << *cts << std::endl;    
  
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
      Expression* domain = copy(env.envi(),it->e()->ti()->domain());
      domains.insert(id,domain);         
    }    
    if(verbose) {
      //std::cerr << "\n\nDEBUG: Flattened model BEFORE flattening: " << std::endl;   
      //debugprint(env.flat());  
    }
    // flatten the expression
    EE ee = flat_exp(env.envi(), Ctx(), cts, constants().var_true, constants().var_true);  
    if(verbose) {
      //std::cerr << "\n\nDEBUG: Flattened model AFTER flattening: " << std::endl;   
      //debugprint(env.flat());    
    
    //std::cout<< "\n" << std::endl;
    //std::cerr << "\n\nDEBUG: Flattened model on higher scope: ******************: " << std::endl;   
    //debugprint(_scopes[0]->env().flat());
    }
    
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
      if(verbose)
        for(unsigned int i=0; i<vars.size(); i++) {        
          std::cout << "DEBUG: adding new variable to solver:" << *vars[i] << std::endl;
        }
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
      if(verbose)
        for(unsigned int i=0; i<flat_cts.size(); i++)
          std::cout << "DEBUG: adding new (flat) constraint to solver:" << *flat_cts[i] << std::endl;      
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
                if(verbose)
                  std::cout << "DEBUG: updating intbounds of \"" << *(id->decl()) << "\" to new bounds: (" << lb_new << ", " << ub_new << ")"  << std::endl;
                success = success && solver->updateIntBounds(id->decl(),lb_new,ub_new);               
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
  
   Expression* 
   SearchHandler::removeRedundantScopeCombinator(Expression* combinator) {
     if(Call* c = combinator->dyn_cast<Call>()) {
       if(c->id() == constants().combinators.scope) {
         return c->args()[0];
       }
     }
     return combinator;
   }
  
}