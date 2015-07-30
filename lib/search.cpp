/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Andrea Rendl <andrea.rendl@nicta.com.au>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <ctime>
#include <sys/time.h>

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
        return interpretPrintCombinator(call,solver,verbose);
      }
      else if(call->id() == constants().combinators.skip) {
        return SolverInstance::SUCCESS;
      }
      else if(call->id() == constants().combinators.fail) {        
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
          GCLock lock;
          vd->e(eval_par(env.envi(), call->args()[i]));
        }
        
        SolverInstance::Status ret;
        env.envi().resetCommitted();
        env.envi().pushSolution(env.envi().getCurrentSolution());
        if(call->decl()->e()) {      
          if(verbose) 
            std::cerr << "DEBUG: interpreting combinator " << *call << " according to its defined body." << std::endl;
          (void) interpretCombinator(call->decl()->e(), solver,verbose);
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
              
            (void) interpretBestCombinator(call, solver, call->id() == constants().combinators.best_min, verbose, print);
          }
          else {          
            std::stringstream ssm; 
            ssm << "No body for combinator: " << *call;
            throw TypeError(env.envi(), call->loc(), ssm.str());
          }
        }
        env.envi().popSolution();
        if (env.envi().isCommitted()) {
          ret = SolverInstance::SUCCESS;
        } else if (env.envi().nbSolutionScopes() > 1) {
          ret = SolverInstance::FAILURE;
        }

        //solver->env().envi().setCurSolution(_solutionScopes.back());
        //if(verbose) {
        //  std::cerr << "DEBUG: Setting current solution to: " << std::endl;
        //  debugprint(solver->env().envi().getCurSolution());
        //}

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
      else if(ident && ident->idn()==-1 && ident->v() == "break") {
        return interpretBreakCombinator(ident, solver, verbose);
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
    else if(ITE* ite = comb->dyn_cast<ITE>()) {
      return interpretConditionalCombinator(ite,solver,verbose);
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
    Call* call;
    KeepAlive ka;
    {
      GCLock lock;
      ArrayLit* al = new ArrayLit(bo->loc(),and_args);
      args.push_back(al);
      ASTString call_id;
      if(bot == BinOpType::BOT_AND)
        call_id = constants().combinators.and_;
      else call_id = constants().combinators.or_;
      call = new Call(bo->loc(),
                      call_id,
                      args);
      ka = call;
    }
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
    if(verbose)
      std::cerr << "DEBUG: Interpreting BEST combinator\n" ;
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
          std::cerr << "TODO: array access" << std::endl;
          exit(EXIT_FAILURE);
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
  SearchHandler::interpretConditionalCombinator(Expression* e, SolverInstanceBase* solver, bool verbose) {
    if (Call* call = e->dyn_cast<Call>()) {
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
    } else {
      ITE* ite = e->cast<ITE>();      
      for (unsigned int i=0; i<ite->size(); i++) {
        Expression* condition = ite->e_if(i);
        SolverInstance::Status status;        
        if(condition->type().isann()) {         
          status = interpretCombinator(condition, solver, verbose);          
        }
        else {
          GCLock lock;
          status = eval_bool(solver->env().envi(), condition) ? SolverInstance::SUCCESS : SolverInstance::FAILURE;
        }
        if(status == SolverInstance::SUCCESS) {
          return interpretCombinator(ite->e_then(i), solver, verbose);
        }        
      }
      return interpretCombinator(ite->e_else(), solver, verbose);
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
          if(Id* id = in->dyn_cast<Id>()) {
            if(id->decl()->e()) {
              in = eval_par(env.envi(), id->decl()->e());
            }            
          }
          if(BinOp* bo = in->dyn_cast<BinOp>()) {
            GCLock lock;
            lb = eval_int(env.envi(), bo->lhs()).toInt();
            int ub = eval_int(env.envi(), bo->rhs()).toInt();
            nbIterations = ub - lb + 1;
          }
          else if(SetLit* sl = in->dyn_cast<SetLit>()) {
            IntSetVal* isv = sl->isv();
            lb = isv->min().toInt();
            int ub = isv->max().toInt();
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
          _repeat_break.push_back(false);
          for(unsigned int i = 0; i<nbIterations; i++) {
            GCLock lock;
            if(isTimeLimitViolated()) { // we have reached a timeout; set timeout index and stop
              setTimeoutIndex(getViolatedTimeLimitIndex());           
              compr->decl(0, 0)->e(oldValue);             
              return status;
            }
            if (_repeat_break.back()) {
              _repeat_break.pop_back();
              return SolverInstance::FAILURE;
            }
            compr->decl(0, 0)->e(IntLit::a(lb+i));
            if(verbose)
              std::cout << "DEBUG: repeating combinator " << *(compr->e()) << " for " << (i+1) << "/" << (nbIterations) << " times" << std::endl;            
            status = interpretCombinator(compr->e(),solver,verbose);     
          }
          _repeat_break.pop_back();
          compr->decl(0, 0)->e(oldValue);
          //std::cout << "REPEAT returning status: " << status << std::endl;
          return status;
        }            
      }          
      else { // repeat is only restricted by satisfiability
        SolverInstance::Status status = SolverInstance::FAILURE;
        bool timeout = isTimeLimitViolated();
        _repeat_break.push_back(false);
        do {
          status = interpretCombinator(call->args()[0], solver,verbose);
          timeout = isTimeLimitViolated();
          //std::cerr << "REPEAT BREAK status after finishing iteration of REPEAT: " << _repeat_break.back() << ", with size: " << _repeat_break.size() << std::endl;
        } while(!timeout && !_repeat_break.back());
        bool hadBreak = _repeat_break.back();
        _repeat_break.pop_back();
        if (hadBreak)
          return SolverInstance::FAILURE;
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
    if (verbose)
      std::cerr << "DEBUG: SCOPE combinator" << std::endl;   
    if(call->args().size() != 1) {
      std::stringstream ssm;
      ssm << "SCOPE-combinator only takes 1 argument instead of " << call->args().size() << " in: " << *call;
      throw TypeError(solver->env().envi(),call->loc(), ssm.str());
    }   
    //std::cerr << "DEBUG: Opening new nested scope" << std::endl;
    solver->env().combinator = call->args()[0];
    CopyMap cmap;
    for(unsigned int i=0; i<_localVars.size(); i++) {
      for(unsigned int j=0; j<_localVars[i].size(); j++) {
        cmap.insert(_localVars[i][j],_localVars[i][j]);
      }
    }
    SolverInstanceBase* solver_copy;
    {
      GCLock lock;
      solver_copy = solver->copy(cmap);
    }
    if (solver->env().envi().nbSolutionScopes() > 1) {
      solver_copy->env().envi().pushSolution(solver->env().envi().getSolution(solver->env().envi().nbSolutionScopes()-2));
    }
    solver_copy->env().envi().pushSolution(solver->env().envi().getCurrentSolution());
    //std::cerr << "DEBUG: Copied solver instance" << std::endl;
    pushScope(solver_copy);
    SolverInstance::Status status = interpretCombinator(solver_copy->env().combinator, solver_copy, verbose);
    if (solver->env().envi().nbSolutionScopes() > 1) {
      if (solver_copy->env().envi().getSolution(0) != NULL) {
        solver->env().envi().setSolution(solver->env().envi().nbSolutionScopes()-2,
                                         solver_copy->env().envi().getSolution(0));
      }
    }
    if (solver_copy->env().envi().getCurrentSolution() != NULL) {
      solver->env().envi().setSolution(solver->env().envi().nbSolutionScopes()-1,
                                       solver_copy->env().envi().getCurrentSolution());
    }
    Env* copy_env = &solver_copy->env();
    delete copy_env->model();
    delete copy_env;
    popScope();
    //std::cerr << "REPEAT BREAK status after closing scope: " << _repeat_break.back() << ", with size: " << _repeat_break.size() << std::endl;
    if (verbose)
      std::cout << "DEBUG: Returning SCOPE status: " << status << std::endl;
    return status;
  }
  
  void 
  SearchHandler::addNewVariableToModel(ASTExprVec<Expression> decls, SolverInstanceBase* solver, bool verbose) {
    std::vector<VarDecl*> par_vars;
    for(unsigned int i=0; i<decls.size(); i++) {
      if(VarDecl* vd = decls[i]->dyn_cast<VarDecl>()) {     
        if (vd->type().isvar()) {  
           // vd must not have a rhs!
          if(vd->e()) {
            std::stringstream ssm;
            ssm << "Local variable declaration may not have a right-hand-side: " << *vd << std::endl;
            throw TypeError(solver->env().envi(),vd->loc(),ssm.str());            
          }          
          // flatten and add the variable to the flat model
          EE ee = flatten(solver->env().envi(),vd->id(),NULL,constants().var_true,solver->env().envi().fopt);
          VarDecl* nvd = ee.r()->cast<Id>()->decl();         
          int nbVars = _localVarsToAdd.back();          
          _localVarsToAdd[_localVarsToAdd.size()-1] = nbVars+1;
          //std::cerr << "DEBUG: setting locally added var to: " << _localVarsToAdd[_localVarsToAdd.size()-1] << std::endl;                    
          
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
          if(nvd->type().dim() == 0)
            output_vd->e(NULL);
          
          VarDecl* output_vd_orig = new VarDecl(vd->loc(), output_vd->ti(), vd->id(), output_vd->id());
          solver->env().output()->addItem(new VarDeclI(Location(), output_vd)); 
          solver->env().output()->addItem(new VarDeclI(Location(), output_vd_orig)); 
          
          // add output annotation to the flat variable declaration
          if (nvd->type().dim() == 0) {
            nvd->addAnnotation(constants().ann.output_var);           
          } else {           
            ArrayLit* al= output_vd->e()->cast<ArrayLit>();           
            for(unsigned int i =0;i<al->length(); i++) {
              Id* id = al->v()[i]->cast<Id>();
              id->decl()->addAnnotation(constants().ann.output_var);
              VarDeclI* vdi = new VarDeclI(Location(), id->decl());
              solver->env().output()->addItem(vdi);
            }
            // do the same for the vardecl in the flat model
            ArrayLit* aln= nvd->e()->cast<ArrayLit>();           
            for(unsigned int i =0;i<aln->length(); i++) {
              Id* id = aln->v()[i]->cast<Id>();
              id->decl()->addAnnotation(constants().ann.output_var);
              VarDeclI* vdi = new VarDeclI(Location(), id->decl());
              solver->env().output()->addItem(vdi);              
            }
            // add the number of local variables according to length of array
            int nbVars = _localVarsToAdd.back(); 
            _localVarsToAdd[_localVarsToAdd.size()-1] = nbVars + al->length();
          }
        } else { // we have a parameter -> add it to the model by flattening
          par_vars.push_back(vd);          
        }
      }
      else { // this is a constraint
        std::cerr << "WARNING: Specify constraints using POST. Ignoring constraint in LET: " << *decls[i]  << std::endl;
        continue;
      }
    }
    _localVars.push_back(par_vars);
  }
  
  SolverInstance::Status
  SearchHandler::interpretLetCombinator(Let* let, SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: LET combinator" << std::endl;   
    ASTExprVec<Expression> decls = let->let();
    addNewVariableToModel(decls, solver, verbose); 
          
    let->pushbindings(); 
    SolverInstance::Status status = interpretCombinator(let->in(), solver, verbose);   
    let->popbindings(); 
    _localVars.pop_back();
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
 
    int ms;
    {
      GCLock lock;
      ms = eval_int(solver->env().envi(), time).toInt();
    }
    long long int t = getTimeout(ms);
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
    //std::cerr << "DEBUG: NEXT combinator" << std::endl;       
    if(isTimeLimitViolated()) {    
      setTimeoutIndex(getViolatedTimeLimitIndex(verbose));
      return SolverInstance::FAILURE;
    }
    setCurrentTimeout(solver);
    //if(_scopes.size() ==1) {    
     // std::cerr << "DEBUG: flat model before next():\n" << std::endl;
     //debugprint(solver->env().flat());
    //}
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SUCCESS) {
      //std::cerr << "DEBUG: output model after next():" << std::endl;
      //debugprint(solver->env().output());
      GCLock lock;
      solver->env().envi().updateCurrentSolution(copy(solver->env().envi(), solver->env().output()));
      // TODO: check if this makes sense; update solutions in all upper scopes?
      //for(unsigned int i=_scopes.size()-1; i>= 0; i--) {
       // _scopes[i]->env().envi().updateCurrentSolution(copy(_scopes[i]->env().envi(), _scopes[i]->env().output()));
      //}
    }
    //std::cerr << "DEBUG: solver returned status " << status << " (SUCCESS = " << SolverInstance::SUCCESS << ")" << std::endl;
    return status; 
  }
  
  SolverInstance::Status
  SearchHandler::interpretNextCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: NEXT combinator" << std::endl; 
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
    
     //if(_scopes.size() ==1) {   
     // std::cerr << "DEBUG: flat model before next():\n" << std::endl;
     // debugprint(solver->env().flat());
     //}
     
    // get next solution
    GCLock lock;
    SolverInstance::Status status = solver->next();
    if(status == SolverInstance::SUCCESS) { 
      //std::cerr << "DEBUG: output model after next():" << std::endl;
      //debugprint(solver->env().output());
      solver->env().envi().updateCurrentSolution(copy(solver->env().envi(), solver->env().output()));
      // TODO: check if this makes sense; update solutions in all upper scopes?
     // for(int i=_scopes.size()-1; i>= 0; i--) {
     //   std::cerr << "Scopes at position " << i << std::endl;
     //   _scopes[i]->env().envi().updateCurrentSolution(copy(_scopes[i]->env().envi(), _scopes[i]->env().output()));
     // }
    }    
    return status; 
  }
  
  SolverInstance::Status
  SearchHandler::interpretAssignCombinator(Call *assignComb, SolverInstanceBase *solver, bool verbose) {  
    VarDecl* decl = NULL;    
    if(ArrayAccess* aa = assignComb->args()[0]->dyn_cast<ArrayAccess>()) {      
      Expression* e = aa->v();
      if(Id* id = e->dyn_cast<Id>()) {
        decl = follow_id_to_decl(e)->cast<VarDecl>();
       // while (decl->flat() && decl->flat() != decl)
       //   decl = decl->flat();
        GCLock lock;
        ArrayLit* al = decl->e()->dyn_cast<ArrayLit>();
        if (al==NULL) {
          al = eval_array_lit(solver->env().envi(), decl->e());
          decl->e(al);
        }
        int idx = eval_int(solver->env().envi(), aa->idx()[0]).toInt();
        al->v()[idx-1] = eval_par(solver->env().envi(), assignComb->args()[1]);       
        return SolverInstance::SUCCESS;
        // TODO: assign value to array element!
      } else {
        std::stringstream ssm;
        ssm << "Expected Id instead of : " << *e << " for assigning: " <<*decl;
        throw TypeError(solver->env().envi(),decl->loc(),ssm.str());
      }
    }
    else { // otherwise it must be an Id according to the parser
      decl = follow_id_to_decl(assignComb->args()[0])->cast<VarDecl>();
    }
    if (decl->type().isann()) {  
      std::stringstream ssm;
      ssm << "Cannot assign value to an annotation: " << *decl;
      throw TypeError(solver->env().envi(),decl->loc(),ssm.str()); 
    } else {
      GCLock lock;
      decl->e(eval_par(solver->env().envi(), assignComb->args()[1]));
    }
    return SolverInstance::SUCCESS;
  }
  
  SolverInstance::Status
  SearchHandler::interpretCommitCombinator(Call *commitComb, SolverInstanceBase* solver, bool verbose) {
    if(commitComb->args().size() != 0) {
      std::stringstream ssm; 
      ssm << "commit takes 0 arguments" << std::endl;
      throw TypeError(solver->env().envi(), commitComb->loc(),ssm.str());
    }
    EnvI& envi = solver->env().envi();
    if (envi.nbSolutionScopes()==1) {
      throw EvalError(solver->env().envi(), commitComb->loc(), "Cannot commit outside of function scope");
    }
/*    if (verbose) {
      if (envi.nbSolutionScopes()==2 ||envi.getSolution(envi.nbSolutionScopes()-3) != envi.getSolution(envi.nbSolutionScopes()-2))
        std::cerr << "COMMIT delete solution and ";
      std::cerr << "COMMIT solution into scope " << envi.nbSolutionScopes()-2 << "\n";
    } */
    //if (_solutionScopes.size()==2 || _solutionScopes[_solutionScopes.size()-3]!=_solutionScopes[_solutionScopes.size()-2]) {      
    // delete _solutionScopes[_solutionScopes.size()-2];
    //}
    //_solutionScopes[_solutionScopes.size()-2]=_solutionScopes.back();
    envi.commitLastSolution();
    return SolverInstance::SUCCESS;
  }
  
  SolverInstance::Status
  SearchHandler::interpretBreakCombinator(Id* c, SolverInstanceBase* solver, bool verbose) {
    if (_repeat_break.size()==0) {
      throw EvalError(solver->env().envi(), c->loc(), "break outside of repeat");
    }
    _repeat_break.back() = true;
    //std::cerr << "REPEAT BREAK status after interpreting BREAK: " << _repeat_break.back() << ", with size: " << _repeat_break.size() << std::endl;
    return SolverInstance::FAILURE;
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
    GCLock lock;
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
    GCLock lock;
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
    GCLock lock;
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
    GCLock lock;
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
  SearchHandler::interpretPrintCombinator(Call* call, SolverInstanceBase* solver, bool verbose) {
    //std::cerr << "DEBUG: PRINT combinator: " << *call << std::endl;
    if (call->args().size()==0) {
      
      if(solver->env().envi().getCurrentSolution() != NULL) {
        GCLock lock;
        solver->env().evalOutput(std::cout);
        std::cout << constants().solver_output.solution_delimiter << std::endl;
        return SolverInstance::SUCCESS;
      }
      else {
        if(verbose)
          std::cerr << "No solution found to be printed by PRINT-combinator" << std::endl;
        return SolverInstance::FAILURE;
      }
    } else {
      //std::cerr << "DEBUG: printing flat model before printing PRINT(""):" << std::endl;      
      //debugprint(solver->env().flat());
      //std::cerr << "DEBUG: trying to print: " << *(call->args()[0]) << "\n";
      GCLock lock;
      std::cout << eval_string(solver->env().envi(), call->args()[0]);
      return SolverInstance::SUCCESS;
    }
  }
  
  SolverInstance::Status
  SearchHandler::interpretPrintCombinator(SolverInstanceBase* solver, bool verbose) {  
    if(solver->env().envi().getCurrentSolution() != NULL) {
      GCLock lock;
      solver->env().evalOutput(std::cout);      
      std::cout << constants().solver_output.solution_delimiter << std::endl;
      std::cout.flush(); // flush so we can read it from within Python
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
    if(BoolLit* bl = cts->dyn_cast<BoolLit>()) {
      if(bl->v()) return true;
    }    
    Env& env = solver->env();
    bool success = true;
    if(verbose)
      std::cout << "DEBUG: BEGIN posting constraint: " << *cts << std::endl;    
  
    int nbCtsBefore = 0;
    for(ConstraintIterator it=env.flat()->begin_constraints(); it!=env.flat()->end_constraints(); ++it)
      nbCtsBefore++;
    int nbVarsBefore = 0;
    for(VarDeclIterator it=env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it) {      
      nbVarsBefore++;        
    }
    nbVarsBefore = nbVarsBefore - _localVarsToAdd[_localVarsToAdd.size()-1];    
    
    // store the domains of each variable in an IdMap to later check changes in the domain (after flattening)
    GCLock lock;
    IdMap<Expression*> domains;
    for(VarDeclIterator it = env.flat()->begin_vardecls(); it!= env.flat()->end_vardecls(); ++it) {
      Id* id = it->e()->id();
      Expression* domain = copy(env.envi(),it->e()->ti()->domain());
      domains.insert(id,domain);         
    }    
   
    // flatten the expression
    //std::cerr << "DEBUG: ozn model before flattening:\n..........................\n";
    //debugprint(env.output());
    //std::cerr << "............................................\n";
    (void) flatten(env.envi(), cts, constants().var_true, constants().var_true, env.envi().fopt);
    //std::cerr << "DEBUG: ozn model aFTER flattening:\n..........................\n";
    //debugprint(env.output());
    //std::cerr << "............................................\n";
    
    int nbVarsAfter = 0;
    for(VarDeclIterator it=env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it)
      nbVarsAfter++;
    if(nbVarsBefore < nbVarsAfter) {
      //std::cerr << "before:" << nbVarsBefore << ", after: " << nbVarsAfter << std::endl;
      std::vector<VarDecl*> vars;
      unsigned int i=0;
      for(VarDeclIterator it= env.flat()->begin_vardecls(); it!=env.flat()->end_vardecls(); ++it) {        
        if(i<nbVarsBefore) { 
          //std::cerr << "skipping var: " << i << ": " << *(it) << std::endl;
          i++;
        }
        else {
          //std::cerr << "adding var: " << i << ": " << *(it)  << std::endl;
          vars.push_back(it->e());
          i++;
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
      if(id->type().ispar()) continue; // skip constants that might have been added 
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
      if(verbose)
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
     long long int time_now = getTimeout(0);
     for(unsigned int i=0; i<_timeouts.size(); i++) {
       long long int timeout = _timeouts[i];
       if(time_now >= timeout) {
         if(verbose)
           std::cerr << "timeout: " << (timeout/1000.0) << "secs has been reached." << std::endl;
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
     long long int time_now = getTimeout(0);
     for(unsigned int i=0; i<_timeouts.size(); i++) {
       long long int timeout = _timeouts[i];
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
   
   long long int
   SearchHandler::getTimeout(int ms) {

     timeval now;
     gettimeofday(&now, NULL);

     long long int to = now.tv_sec*1000+(now.tv_usec/1000)+ms;
     return to;
   }
   
   void 
   SearchHandler::setCurrentTimeout(SolverInstanceBase* solver) {
     if(_timeouts.size() == 0)
       return;
     long long int smallest_timeout = _timeouts[0];
     for(unsigned int i=1; i<_timeouts.size(); i++) {
       long long int timeout = _timeouts[i];
       if(timeout < smallest_timeout)
         smallest_timeout = timeout;
     }
     long long int now = getTimeout(0);
     long long int timeout_ms = smallest_timeout-now;
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