/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ctime>

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
      else if(call->id() == constants().combinators.next) {
        return interpretNextCombinator(call,solver,verbose);
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
      else if(call->id() == constants().combinators.skip) {
        return SolverInstance::SUCCESS;
      }
      else if(call->id() == constants().combinators.fail) {
        // TODO: set constraint scope to COMPLETE
        return SolverInstance::FAILURE;
      }
      else if(call->id() == constants().combinators.prune) {
        return SolverInstance::FAILURE;
      }      
      else if(call->id() == constants().combinators.limit_time) {
        return interpretTimeLimitAdvancedCombinator(call, solver, verbose);
      }
      else if(call->id() == constants().combinators.comb_assign) {
        return interpretAssignCombinator(call, solver, verbose);
      }
      else if(call->id() == constants().combinators.commit) {
        return interpretCommitCombinator(call, solver, verbose);
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
      else if(ident && ident->idn()==-1 && ident->v() == constants().combinators.skip) {
        return SolverInstance::SUCCESS;
      }
      else if(ident && ident->idn()==-1 && ident->v() == constants().combinators.prune) {
        return SolverInstance::FAILURE;
      }
      else if(ident && ident->idn()==-1 && ident->v() == constants().combinators.fail) {
        // TODO: set constraint scope to COMPLETE
        return SolverInstance::FAILURE;
      }
      else {
        std::stringstream ssm; 
        ssm << "unknown combinator id: " << *ident;
        throw TypeError(env.envi(), ident->loc(), ssm.str());
      }
    }
    else if(Let* let = comb->dyn_cast<Let>()) {      
      return interpretLetCombinator(let, solver, verbose);      
    } 
    else if(BinOp* bo = comb->dyn_cast<BinOp>()) {
      return interpretBinOpCombinator(bo,solver,verbose);
    }
    else {
      std::stringstream ssm; 
      ssm << "unknown combinator: " << *comb << " of type: " << comb->eid();
      throw TypeError(env.envi(), comb->loc(), ssm.str());
    }    
  }
  
  SolverInstance::Status
  SearchHandler::interpretBinOpCombinator(BinOp* bo, SolverInstanceBase* solver, bool verbose)  {
    BinOpType bot = bo->op();
    if(bot != BinOpType::BOT_AND && bot != BinOpType::BOT_OR) {
      std::stringstream ssm;
      ssm << "unknown bin-op combinator: " << *bo << std::endl;
      throw TypeError(solver->env().envi(),bo->loc(),ssm.str());
    }
    std::vector<Expression*> args;
    Expression* lhs = bo->lhs();
    args.push_back(bo->rhs());
    while(BinOp* lhs_bo = lhs->dyn_cast<BinOp>()) {
      if(lhs_bo->op() != bot) {
        args.push_back(lhs_bo);
        break;
      }      
      args.push_back(lhs_bo->rhs());     
      lhs = lhs_bo->lhs();
    }
    args.push_back(lhs);
    //reverse the arguments
    std::vector<Expression*> and_args;    
    for(int i=args.size()-1; i>=0; i--) {     
      and_args.push_back(args[i]);
    }
    args.clear();
    ArrayLit* al = new ArrayLit(bo->loc(),and_args);
    args.push_back(al);
    ASTString call_id; 
    if(bot == BinOpType::BOT_AND) 
      call_id = constants().combinators.and_;
    else call_id = constants().combinators.or_;
    Call* call = new Call(bo->loc(),
                          call_id,
                          args);
    KeepAlive ka(call);
    if(verbose)    
      std::cerr << "DEBUG: Transformed binops into call: " << *call << " "<< std::endl;
    if(bot == BinOpType::BOT_AND)
      return interpretAndCombinator(call, solver, verbose);
    else return interpretOrCombinator(call, solver, verbose);    
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
        if(status == SolverInstance::FAILURE)
          return status;            
      }
      return SolverInstance::SUCCESS;
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
  SearchHandler::interpretConditionalCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    if(call->args().size() != 3) {
      std::stringstream ssm;
      ssm << call->id() << "-combinator takes 3 arguments instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }    
    Expression* condition = call->args()[0];
    SolverInstance::Status status = interpretCombinator(condition, solver, verbose);
    if(status == SolverInstance::SUCCESS) {
      return interpretCombinator(call->args()[1], solver, verbose);
    }
    else {
      return interpretCombinator(call->args()[2], solver, verbose);
    }
  }
  
  
  SolverInstance::Status
  SearchHandler::interpretOrCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    //std::cout << "DEBUG: OR combinator: " << (*call) << std::endl;        
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "OR-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(), call->loc(), ssm.str());
    }
    SolverInstance::Status status = SolverInstance::FAILURE; 
    if(ArrayLit* al = call->args()[0]->dyn_cast<ArrayLit>()) {
      assert(al->dims() == 1);
      for(unsigned int i=0; i<al->length(); i++) {
        status = interpretCombinator(al->v()[i],solver,verbose);
        if(status == SolverInstance::SUCCESS) // stop at success
          return status;
      }
      return status;
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
    //std::cerr << "DEBUG: Flat model after interpreting POST combinator:\n" << std::endl;
    //debugprint(solver->env().flat());
    return SolverInstance::SUCCESS;
  }
  
  SolverInstance::Status
  SearchHandler::interpretRepeatCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    Env& env = solver->env();
    // check for timeout!    
    if(isTimeLimitViolated()) {
      setTimeoutIndex(getViolatedTimeLimitIndex());
      return SolverInstance::FAILURE;
    }
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
          int lb;
          if(BinOp* bo = in->dyn_cast<BinOp>()) {                
            lb = eval_int(env.envi(), bo->lhs()).toInt();
            int ub = eval_int(env.envi(), bo->rhs()).toInt();
            nbIterations = ub - lb + 1;
          } 
          else {
            std::stringstream ssm;
            ssm << "Expected set literal of the form \"(lb..ub)\" instead of \"" << *in << "\"";
            throw TypeError(solver->env().envi(),in->loc(), ssm.str());
          }
          SolverInstance::Status status = SolverInstance::FAILURE;
          // repeat the argument a limited number of times
          Expression* oldValue = compr->decl(0, 0)->e();
          for(unsigned int i = 0; i<nbIterations; i++) {
            if(isTimeLimitViolated()) { // we have reached a timeout; set timeout index and stop
              setTimeoutIndex(getViolatedTimeLimitIndex());           
              compr->decl(0, 0)->e(oldValue);             
              return status;
            }
            compr->decl(0, 0)->e(IntLit::a(lb+i));
            if(verbose)
              std::cout << "DEBUG: repeating combinator " << *(compr->e()) << " for " << (i+1) << "/" << (nbIterations) << " times" << std::endl;            
            status = interpretCombinator(compr->e(),solver,verbose);
            if(false) { // TODO: check if constraint scope is COMPLETE in which case we should stop           
              compr->decl(0, 0)->e(oldValue);
              return status;
            }
          }
          compr->decl(0, 0)->e(oldValue);
          return status;
        }            
      }          
      else { // repeat is only restricted by satisfiability
        SolverInstance::Status status = SolverInstance::FAILURE;
        bool timeout = isTimeLimitViolated();
        do {
          status = interpretCombinator(call->args()[0], solver,verbose);
          timeout = isTimeLimitViolated();
        } while(!timeout); // TODO: check if constraint scope is COMPLETE, in which case we should stop
        if(timeout) setTimeoutIndex(getViolatedTimeLimitIndex());
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
    if(isTimeLimitViolated()) {
      setTimeoutIndex(getViolatedTimeLimitIndex());     
      return SolverInstance::FAILURE;
    }
    
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
  
  void 
  SearchHandler::addNewVariableToModel(ASTExprVec<Expression> decls, SolverInstanceBase* solver, bool verbose) {
    for(unsigned int i=0; i<decls.size(); i++) {
      if(VarDecl* vd = decls[i]->dyn_cast<VarDecl>()) {
        if (vd->type().isvar()) {
          // flatten and add the variable to the flat model
          EE ee = flat_exp(solver->env().envi(),Ctx(),vd,NULL,constants().var_true);
          VarDecl* nvd = ee.r()->cast<Id>()->decl();
          
          // add output annotation to the flat variable declaration
          if (nvd->type().dim() == 0) {
            nvd->addAnnotation(constants().ann.output_var);
          } else {
            // TODO: see flatten.cpp:4517
          }
          
          // Create new output variable
          Type t = nvd->type();
          t.ti(Type::TI_PAR); // make par
          VarDecl* output_vd = copy(solver->env().envi(), nvd)->cast<VarDecl>(); 
          output_vd->ti()->domain(NULL);
          output_vd->flat(nvd);
          output_vd->ann().clear();
          output_vd->introduced(false);
          output_vd->ti()->type(t);
          output_vd->type(t);
          output_vd->e(NULL);
          solver->env().output()->addItem(new VarDeclI(Location(), output_vd));
        }
      }
      else { // this is a constraint
        std::cerr << "WARNING: Specify constraints using POST. Ignoring constraint in LET: " << *decls[i]  << std::endl;
        continue;
      }
    }
  }
  
  SolverInstance::Status
  SearchHandler::interpretLetCombinator(Let* let, SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: LET combinator" << std::endl;   
    ASTExprVec<Expression> decls = let->let();
    addNewVariableToModel(decls, solver, verbose); 
       
    let->pushbindings(); 
    SolverInstance::Status status = interpretCombinator(let->in(), solver, verbose);   
    let->popbindings();    
    return status;
  }
  
  SolverInstance::Status 
  SearchHandler::interpretTimeLimitAdvancedCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {    
    if(call->args().size() != 2) {
      std::stringstream ssm;
      ssm << "TIME-LIMIT-combinator takes 2 arguments instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(),call->loc(), ssm.str());
    }
    Expression* time = call->args()[0];
    if(!time->type().ispar()) {
      std::stringstream ssm;
      ssm << "Expected par expression instead of " << *time << " in: " << *call;
      throw TypeError(solver->env().envi(),time->loc(), ssm.str());
    }
 
    int ms = eval_int(solver->env().envi(), time).toInt();
    clock_t t = getTimeout(ms);
    _timeouts.push_back(t);
    if(isTimeLimitViolated()) {
      int timeoutIdx = getViolatedTimeLimitIndex();
      if(timeoutIdx == _timeouts.size()-1) // if our own time-out was already reached
        resetTimeoutIndex();
      _timeouts.pop_back(); // remove this time-limit     
      return SolverInstance::FAILURE; // this is a higher level timeout, so stop
    }
 
    // execute argument if there is enough time
    SolverInstance::Status status = interpretCombinator(call->args()[1], solver, verbose);   
    if(_timeoutIndex == _timeouts.size()-1) { // if this timeout has been reached
      resetTimeoutIndex();
    }    
    _timeouts.pop_back(); // remove the time limit
    return status;
  } 
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(SolverInstanceBase* solver, bool verbose) {
   // std::cerr << "DEBUG: NEXT combinator" << std::endl;       
    if(isTimeLimitViolated()) {    
      setTimeoutIndex(getViolatedTimeLimitIndex(verbose));
      return SolverInstance::FAILURE;
    }
    setCurrentTimeout(solver);
    SolverInstance::Status status = solver->next();
      if(status == SolverInstance::SUCCESS) {
//      solver->env().envi().hasSolution(true);
//      // set/update the solutions in all higher scopes
//      for(unsigned int i = 0; i <_scopes.size(); i++) {
//        _scopes[i]->env().envi().hasSolution(true);
//        updateSolution(solver->env().output(), _scopes[i]->env().output());
//      }
      delete _solutionScopes.back();
      _solutionScopes.back() = copy(solver->env().envi(), solver->env().output());
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
    if(isTimeLimitViolated(verbose)) {
      setTimeoutIndex(getViolatedTimeLimitIndex());
      return SolverInstance::FAILURE;
    }
    if(args.size() > 0)
      interpretLimitCombinator(args[0],solver,verbose);
    setCurrentTimeout(solver); // timeout via time_limit(ms,ann) combinator
    
    // get next solution
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SUCCESS) {
//      solver->env().envi().hasSolution(true);
//      // set/update the solutions in all higher scopes
//      for(unsigned int i = 0; i <_scopes.size(); i++) {
//        _scopes[i]->env().envi().hasSolution(true);
//        updateSolution(solver->env().output(), _scopes[i]->env().output());
//      }
      delete _solutionScopes.back();
      _solutionScopes.back() = copy(solver->env().envi(), solver->env().output());
    }
    //std::cerr << "DEBUG: solver returned status " << status << " (SAT = " << SolverInstance::SAT << ")" << std::endl;
    return status; 
  }
  
  SolverInstance::Status
  SearchHandler::interpretAssignCombinator(Call *assignComb, SolverInstanceBase *solver, bool verbose) {
    VarDecl* decl = follow_id_to_decl(assignComb->args()[0])->cast<VarDecl>();
    if (decl->type().isann()) {
      Call* rhs = assignComb->args()[1]->cast<Call>();
      std::vector<Expression*> previousParameters(rhs->decl()->params().size());
      for (unsigned int i=rhs->decl()->params().size(); i--;) {
        VarDecl* vd = rhs->decl()->params()[i];
        previousParameters[i] = vd->e();
        vd->flat(vd);
        vd->e(eval_par(solver->env().envi(), rhs->args()[i]));
      }
      _solutionScopes.push_back(NULL);
      
      SolverInstance::Status ret;
      
      if(rhs->decl()->e()) {
        if(verbose)
          std::cerr << "DEBUG: interpreting combinator " << *rhs << " according to its defined body." << std::endl;
        ret = interpretCombinator(rhs->decl()->e(), solver,verbose);
      } else if(rhs->id() == constants().combinators.next) {
        ret = interpretNextCombinator(rhs, solver,verbose);
      }
      for (unsigned int i=rhs->decl()->params().size(); i--;) {
        VarDecl* vd = rhs->decl()->params()[i];
        vd->e(previousParameters[i]);
        vd->flat(vd->e() ? vd : NULL);
      }
      decl->e(new ModelExp(Location(),_solutionScopes.back()));
      _solutionScopes.pop_back();
      return ret;

    } else {
      decl->e(eval_par(solver->env().envi(), assignComb->args()[1]));
    }
    return SolverInstance::SUCCESS;
  }
  
  SolverInstance::Status
  SearchHandler::interpretCommitCombinator(Call *commitComb, SolverInstanceBase* solver, bool verbose) {
    if(commitComb->args().size() != 1) {
      std::stringstream ssm; 
      ssm << "commit takes 1 argument" << std::endl;
      throw TypeError(solver->env().envi(), commitComb->loc(),ssm.str());
    } 
    ModelExp* me = Expression::cast<ModelExp>(follow_id(commitComb->args()[0])); // TODO: get the solution from current scope
    if (me) {
      delete _solutionScopes.back();
      _solutionScopes.back() = copy(solver->env().envi(),me->m());
      return SolverInstance::SUCCESS;
    } else {
      return SolverInstance::FAILURE;
    }
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
      return SolverInstance::SUCCESS;
    }
    else {
      if(verbose)
        std::cerr << "No solution found to be printed by PRINT-combinator" << std::endl;      
     return SolverInstance::FAILURE;
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
  }
  
   Expression* 
   SearchHandler::removeRedundantScopeCombinator(Expression* combinator, SolverInstanceBase* solver, bool verbose) {
     if(Call* c = combinator->dyn_cast<Call>()) {
       if(c->id() == constants().combinators.scope) {
         return c->args()[0];
       }
     }
     else if(Let* let = combinator->dyn_cast<Let>()) {      
        ASTExprVec<Expression> decls = let->let();
        addNewVariableToModel(decls, solver, verbose);  
        return let->in();     
     }
     return combinator;
   }
   
   bool 
   SearchHandler::isTimeLimitViolated(bool verbose) {
     clock_t time_now = std::clock();
     for(unsigned int i=0; i<_timeouts.size(); i++) {
       clock_t timeout = _timeouts[i];
       if(time_now >= timeout) {
         if(verbose)
           std::cerr << "timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has been reached." << std::endl;
         //std::cerr << "WARNING: timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has been reached." << std::endl;
         return true;
       }
       else {
         //if(verbose)
          // std::cerr << "Currently at time " << (((float)time_now)/CLOCKS_PER_SEC)<< ". timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has not yet been reached." << std::endl;
       }
     }
     return false;
   }
   
   int 
   SearchHandler::getViolatedTimeLimitIndex(bool verbose) {
     clock_t time_now = std::clock();
     for(unsigned int i=0; i<_timeouts.size(); i++) {
       clock_t timeout = _timeouts[i];
       if(time_now >= timeout) {
         if(verbose)
           std::cerr << "timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has been reached." << std::endl;
         //std::cerr << "WARNING: timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has been reached." << std::endl;
         return i;
       }
       else {
         //if(verbose)
          // std::cerr << "Currently at time " << (((float)time_now)/CLOCKS_PER_SEC)<< ". timeout: " << (((float)timeout)/CLOCKS_PER_SEC) << "secs has not yet been reached." << std::endl;
       }
     }
     return -1;
   }   
   
   void 
   SearchHandler::setTimeoutIndex(int index) {
     // if there already is a timeout that has fired
     if(_timeoutIndex >= 0) {
       if(index > _timeoutIndex) // do overwrite a higher time-limit
         return;
     }    
    _timeoutIndex = index;
   }   
   
   void 
   SearchHandler::resetTimeoutIndex(void) {
     _timeoutIndex = -1;
   }
   
   clock_t 
   SearchHandler::getTimeout(int ms) {
    clock_t t = std::clock();
    t = t + (ms/1000)*(CLOCKS_PER_SEC);
    //std::cerr << "DEBUG: setting time-out to: " << (((float)t)/CLOCKS_PER_SEC) << "secs" << std::endl;
    return t;
   }
   
   void 
   SearchHandler::setCurrentTimeout(SolverInstanceBase* solver) {
     if(_timeouts.size() == 0)
       return;
     clock_t smallest_timeout = _timeouts[0];
     for(unsigned int i=1; i<_timeouts.size(); i++) {
       clock_t timeout = _timeouts[i];
       if(timeout < smallest_timeout)
         smallest_timeout = timeout;
     }
     clock_t now = std::clock();
     int timeout_ms = (int) ((smallest_timeout - now)/CLOCKS_PER_SEC)*1000;
     if(timeout_ms > 0) {
       Options& opt = solver->getOptions();
       if(opt.hasParam(constants().solver_options.time_limit_ms.str())) {
         int old_timeout = opt.getIntParam(constants().solver_options.time_limit_ms.str());
         if(old_timeout <timeout_ms) // if the timeout that is already set is even smaller, keep it
           return;
       }
       opt.setIntParam(constants().solver_options.time_limit_ms.str(),timeout_ms);
       //std::cerr << "DEBUG: setting solver time-out to: " << timeout_ms << "ms" << std::endl;
     }     
   }
  
}