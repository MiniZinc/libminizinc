/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <unistd.h>
#include <cstdio>
#include <sys/select.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/time.h>
#include <signal.h>
#include <fstream>
#include <stdio.h>
#include <climits>


#include <minizinc/solvers/fzn_solverinstance.hh>

#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/copy.hh>

namespace MiniZinc {
  void translateObj(Env& e);
  namespace {
    class FznProcess {
    protected:
      std::string _fzncmd;
      bool _canPipe;
      Model* _flat;
    public:
      FznProcess(const std::string& fzncmd, bool pipe, Model* flat) : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat) {}
      std::string run(Options& opt) {
        std::stringstream result;
        int pipes[2][2];
        pipe(pipes[0]);
        pipe(pipes[1]);

       
        //std::cerr << "DEBUG:===========================================\n";
        std::string fznFile;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX.fzn";
          int fd = mkstemps(tmpfile, 4);
          close(fd);
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          MiniZinc::Printer p(os,0);
          //MiniZinc::Printer p_debug(std::cerr,0); // uncomment to see flatzinc model for FZN solver
          if (!os.good()) {
            std::string last_error = strerror(errno);
            throw InternalError(std::string("cannot open file ")+tmpfile+" for writing: "+last_error);
          }
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            Item* item = *it;
            if(item->removed()) 
              continue;
            if(SolveI* si = item->dyn_cast<SolveI>()) {              
              if(si->combinator_lite()) {
                si->ann().removeCall(constants().ann.combinator); // remove the combinator annotation
              }             
            }
            //os << *item;
            p.print(item);            
            //p_debug.print(item); // DEBUG: uncomment to see flatzinc model that is sent to FZN solver
          }
        }
        
        if (int childPID = fork()) {
          //std::cout << "DEBUG: if childpid = fork(). Processing timeouts etc. " << std::endl;
          close(pipes[0][0]);
          close(pipes[1][1]);
          if (_canPipe) {
            for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
              std::stringstream ss;
              Item* item = *it;
              ss << *item;
              std::string str = ss.str();
              write(pipes[0][1], str.c_str(), str.size());
            }
          }
          close(pipes[0][1]);
          
          fd_set fdset;
          struct timeval timeout;
          FD_ZERO(&fdset);
          FD_SET(pipes[1][0], &fdset);
          
          struct timeval starttime;
          gettimeofday(&starttime, NULL);

          timeval* timeout_p = NULL;
          // TODO: how to disable timeout??
          long long int timeout_msec = -1;
          if(opt.hasParam(constants().solver_options.time_limit_ms.str())) {
            timeout_msec = opt.getIntParam(constants().solver_options.time_limit_ms.str());
            //std::cerr << "DEBUG: runProcess: solve timeout in ms = " << timeout_msec << std::endl;
            int timeout_sec = timeout_msec / 1000;
            int timeout_usec = (timeout_msec % 1000) * 10;
            timeout.tv_sec = timeout_sec;
            timeout.tv_usec = timeout_usec;
            timeout_p = &timeout;
          }
          
          bool done = false;
          while (!done) {
            switch (select(FD_SETSIZE, &fdset, NULL, NULL, timeout_p)) {
              case 0:
              {
                timeval currentTime, elapsed;
                gettimeofday(&currentTime, NULL);
                elapsed.tv_sec = currentTime.tv_sec - starttime.tv_sec;
                elapsed.tv_usec = currentTime.tv_usec - starttime.tv_usec;
                if (elapsed.tv_usec < 0) {
                  elapsed.tv_sec--;
                  elapsed.tv_usec += 1000000;
                }
                long long int elapsed_msec = elapsed.tv_sec*1000;
                elapsed_msec += elapsed.tv_usec/1000;
                
                long long int tdiff = timeout_msec - elapsed_msec;
                if (tdiff < 0) {
                  done = true;
                } else {
                  timeout.tv_sec = tdiff / 1000;
                  timeout.tv_usec = (tdiff % 1000)*1000;
                }
              }
                break;
              case 1:
              {
                char buffer[100];
                int count = read(pipes[1][0], buffer, sizeof(buffer)-1);
                if (count > 0) {                  
                  buffer[count] = 0;
                  result << buffer;
                  if (timeout_msec >= 0) {
                    timeval currentTime, elapsed;
                    gettimeofday(&currentTime, NULL);
                    elapsed.tv_sec = currentTime.tv_sec - starttime.tv_sec;
                    elapsed.tv_usec = currentTime.tv_usec - starttime.tv_usec;
                    if (elapsed.tv_usec < 0) {
                      elapsed.tv_sec--;
                      elapsed.tv_usec += 1000000;
                    }
                    long long int elapsed_msec = elapsed.tv_sec*1000;
                    elapsed_msec += elapsed.tv_usec/1000;
                    
                    long long int tdiff = timeout_msec - elapsed_msec;
                    if (tdiff < 0) {
                      done = true;
                    }
                    timeout.tv_sec = tdiff / 1000;
                    timeout.tv_usec = (tdiff % 1000)*1000;
                  }
                } else {
                  done = true;
                }
              }
                break;
              case -1:
              {
              }
                break;
            }
          }
          close(pipes[1][0]);
          kill(childPID, SIGKILL);
          waitpid(childPID, NULL, 0);
          
          if (!_canPipe) {            
            remove(fznFile.c_str()); // commented for DEBUG only: do not remove fzn file
            //std::cerr << "DEBUG: name of fzn-file = " << fznFile << std::endl;
          }
          return result.str();
        } else {
          //std::cout << "DEBUG: if NOT childpid = fork(). Executing command. " << std::endl;
          close(STDOUT_FILENO);
          close(STDIN_FILENO);
          dup2(pipes[0][0], STDIN_FILENO);
          dup2(pipes[1][1], STDOUT_FILENO);
          close(pipes[0][0]);
          close(pipes[1][1]);
          close(pipes[1][0]);
          close(pipes[0][1]);
          //char* argv[] = {strdup(_fzncmd.c_str()),strdup("-a"),strdup("-"),0}; 
          int status = executeCommand(opt,fznFile);
                  
          if(status != 0) {
            std::stringstream ssm;
            ssm << "FznProcess::run: cannot execute command: " << _fzncmd.c_str();
            throw InternalError(ssm.str());
          }
        }
        assert(false);
      }
    
    protected:
      int executeCommand(Options& opt, std::string fznFile) {
          int status;          
          // determine the command line arguments
          if(opt.hasParam(constants().solver_options.time_limit_ms.str())) {            
            //int time_ms = opt.getIntParam(constants().solver_options.time_limit_ms.str());
            //char time_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
            //sprintf(time_c, "%d", time_ms); 
            // node limit & time limit
            if(opt.hasParam(constants().solver_options.node_limit.str())) {                           
              int nodes = opt.getIntParam(constants().solver_options.node_limit.str());
              char nodes_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
              //fail limit & node limit & time limit
              if(opt.hasParam(constants().solver_options.fail_limit.str())) {
                int fails = opt.getIntParam(constants().solver_options.fail_limit.str());
                char fails_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
                sprintf(fails_c, "%d", fails);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-fails"), fails_c, strdup("-node"), nodes_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[5] = strdup(fznFile.c_str());
                status = execv(argv[0],argv);                  
              }
              else { // only node and time limit
                sprintf(nodes_c, "%d", nodes);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-node"), nodes_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[3] = strdup(fznFile.c_str());
                status = execvp(argv[0],argv);  
              }
            }
            // fail limit
            else if(opt.hasParam(constants().solver_options.fail_limit.str())) {                
                int fails = opt.getIntParam(constants().solver_options.fail_limit.str());
                char fails_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
                sprintf(fails_c, "%d", fails);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-fail"), fails_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[3] = strdup(fznFile.c_str());
                status = execvp(argv[0],argv);  
            }                      
            else { // only time limit                     
              char* argv[] = {strdup(_fzncmd.c_str()), strdup("-"),0};          
              if (!_canPipe) 
                argv[1] = strdup(fznFile.c_str());
              //std::cerr << "DEBUG: Command " << argv[0] << " " << argv[1]  << std::endl;
              status = execvp(argv[0],argv);  
            }         
          } // only node limit
          else if(opt.hasParam(constants().solver_options.node_limit.str())) {            
            int nodes = opt.getIntParam(constants().solver_options.node_limit.str());
            char nodes_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
            sprintf(nodes_c, "%d", nodes);  
            char* argv[] = {strdup(_fzncmd.c_str()),strdup("-node"), nodes_c, strdup("-"),0};          
            if (!_canPipe) 
              argv[3] = strdup(fznFile.c_str());
            status = execvp(argv[0],argv);  
          } // only fail limit
          else if(opt.hasParam(constants().solver_options.fail_limit.str())) {           
            int fails = opt.getIntParam(constants().solver_options.fail_limit.str());
            char fails_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
            sprintf(fails_c, "%d", fails);  
            char* argv[] = {strdup(_fzncmd.c_str()),strdup("-fail"), fails_c, strdup("-"),0};          
            if (!_canPipe) 
              argv[3] = strdup(fznFile.c_str());
            status = execvp(argv[0],argv);  
          }          
          else {           
            char* argv[] = {strdup(_fzncmd.c_str()),strdup("-"),0};             
            if (!_canPipe) 
              argv[1] = strdup(fznFile.c_str());            
            status = execvp(argv[0],argv);                    
          }                           
        return status;
      }
    };
  }
  

  
  FZNSolverInstance::FZNSolverInstance(Env& env, const Options& options)
  : NISolverInstanceImpl<FZNSolver>(env,options), _fzn(env.flat()), _ozn(env.output()) {
     // fzn-solvers can directly return best solutions by using minimize/maximize solve item
    _options.setBoolParam(constants().solver_options.supports_maximize.str(),true);
    _options.setBoolParam(constants().solver_options.supports_maximize.str(),true);       
  }
  
  FZNSolverInstance::~FZNSolverInstance(void) {}

  SolverInstanceBase*
  FZNSolverInstance::copy(CopyMap& cmap) {
    Env* env_copy = _env.copyEnv(cmap); 
    Options options_copy; 
    options_copy = _options.copyEntries(options_copy);
    return new FZNSolverInstance(*env_copy,options_copy);
  }
 
  namespace {
    ArrayLit* b_arrayXd(Env& env, ASTExprVec<Expression> args, int d) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env.envi(), args[d]);
      std::vector<std::pair<int,int> > dims(d);
      unsigned int dim1d = 1;
      for (int i=0; i<d; i++) {
        IntSetVal* di = eval_intset(env.envi(), args[i]);
        if (di->size()==0) {
          dims[i] = std::pair<int,int>(1,0);
          dim1d = 0;
        } else if (di->size() != 1) {
          throw EvalError(env.envi(), args[i]->loc(), "arrayXd only defined for ranges");
        } else {
          dims[i] = std::pair<int,int>(static_cast<int>(di->min(0).toInt()),
                                       static_cast<int>(di->max(0).toInt()));
          dim1d *= dims[i].second-dims[i].first+1;
        }
      }
      if (dim1d != al->v().size())
        throw EvalError(env.envi(), al->loc(), "mismatch in array dimensions");
      ArrayLit* ret = new ArrayLit(al->loc(), al->v(), dims);
      Type t = al->type();
      t.dim(d);
      ret->type(t);
      ret->flat(al->flat());
      return ret;
    }
  }
  
  SolverInstance::Status
  FZNSolverInstance::solve(void) {
    std::vector<std::string> includePaths;
    std:: string fzn_solver = "fzn-gecode";
    if(_options.hasParam(constants().opts.solver.fzn_solver.str()))
      fzn_solver = _options.getStringParam(constants().opts.solver.fzn_solver.str());
    else fzn_solver = _options.getStringParam("solver","fzn-gecode"); // old CLI
    if(_options.hasParam(constants().opts.verbose.str())) {
      if(_options.getBoolParam(constants().opts.verbose.str()))
        std::cerr << "Using FZN solver " << fzn_solver << " for solving." << std::endl;
    }
    FznProcess proc(fzn_solver,false,_fzn); 
    std::string r = proc.run(_options);
    std::stringstream result;
    result << r;
    std::string solution;
    
    typedef std::pair<VarDecl*,Expression*> DE;
    ASTStringMap<DE>::t declmap;    
    for (unsigned int i=0; i<_ozn->size(); i++) {
      if (VarDeclI* vdi = (*_ozn)[i]->dyn_cast<VarDeclI>()) {
        GCLock lock;
        declmap.insert(std::make_pair(vdi->e()->id()->str(),DE(vdi->e(),vdi->e()->e())));
        //std::cerr << "DEBUG: inserting { " << (vdi->e()->id()->str()) << " -> (" << *(vdi->e()) << "," << *(vdi->e()->e()) << ") } into declmap" <<std::endl;
      }
    }

    bool hadSolution = false;
    std::string sstr = result.str();    
    //std::cerr << "DEBUG: output from solver:\n" << sstr << std::endl;
    //std::cerr << "DEBUG: printing fzn model:\n---------------------------------\n";
    //debugprint(_fzn);
    //std::cerr << "=============================\nDEBUG: printing ozn model:\n---------------------------------\n";
    //debugprint(_ozn);    
    //std::cerr << "=======================\n";
    while (result.good()) {
      std::string line;
      getline(result, line);    
      if (line==constants().solver_output.solution_delimiter.str()) {
        if (hadSolution) {
          for (ASTStringMap<DE>::t::iterator it=declmap.begin(); it != declmap.end(); ++it) {
            it->second.first->e(it->second.second);
          }
        }
        Model* sm = parseFromString(solution, "solution.szn", includePaths, true, false, false, std::cerr);        
        //std::cerr << "DEBUG: printing solution model:\n";
        //debugprint(sm);
        //std::cerr << "=======================================\n";
        if(sm == NULL) {
          std::stringstream ssm;
          ssm << "Error in solver " << fzn_solver <<". Could not parse solver output to MiniZinc solution:\n" << solution;
          throw InternalError(ssm.str());
        }
        for (Model::iterator it = sm->begin(); it != sm->end(); ++it) {            
          if (AssignI* ai = (*it)->dyn_cast<AssignI>()) {
            //std::cerr << "processing item in model:" << (*ai) << "\n";
            ASTStringMap<DE>::t::iterator it = declmap.find(ai->id());            
            if (it==declmap.end()) {
              std::cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
              exit(EXIT_FAILURE);            
            }
            if (Call* c = ai->e()->dyn_cast<Call>()) {             
              // This is an arrayXd call, make sure we get the right builtin
              assert(c->args()[c->args().size()-1]->isa<ArrayLit>());
              for (unsigned int i=0; i<c->args().size(); i++)
                c->args()[i]->type(Type::parsetint());
              c->args()[c->args().size()-1]->type(it->second.first->type());
              ArrayLit* al = b_arrayXd(_env, c->args(), c->args().size()-1);
              it->second.first->e(al);              
              setSolution(it->second.first->id(),al);
              //std::cerr << "DEBUG: setting " << *(it->second.first->id()) << " as " << *(al) << "\n";
            } else {  
              it->second.first->e(ai->e());       
              if(it->second.second) { // ID_X = ID_Y, then also set ID_Y
                if(Id* id = it->second.second->dyn_cast<Id>()) 
                  setSolution(id,ai->e());
              }
              setSolution(it->second.first->id(),ai->e());
              //std::cerr << "DEBUG: setting " << *(it->second.first->id()) << " as " << *(ai->e()) << "\n";                  
            }
          }
        }        
        delete sm;
        hadSolution = true;
        if(_env.flat()->solveItem()->st() == SolveI::SolveType::ST_SAT) {
          return SolverInstance::SUCCESS;
        }          
        
      } else if (line==constants().solver_output.opt.str()) {
        return hadSolution ? SolverInstance::SUCCESS : SolverInstance::FAILURE;
      } else if(line==constants().solver_output.unsat.str()) {
        return SolverInstance::FAILURE;
      } else if(line==constants().solver_output.unbounded.str()) {
        return SolverInstance::FAILURE; // TODO: maybe special status for unbounded case?
      } else if(line==constants().solver_output.unknown.str()) {
        return SolverInstance::FAILURE;
      } else {
        line += "\n"; // break the line, especially in case it is a comment
        solution += line;
      }
    }
    
    // XXX should always return 'UNKNOWN' because not well-defined termination?
    return hadSolution ? SolverInstance::SUCCESS : SolverInstance::FAILURE;
  }
  
  void
  FZNSolverInstance::processFlatZinc(void) {}  
  
  void
  FZNSolverInstance::setSolution(Id* id, Expression* e) {
    IdMap<Expression*>::iterator it = _solution.find(id);
    if (it == _solution.end()) {
      _solution.insert(id,e);
    } else {
      it->second = e;
    }
  }
  
  Expression*
  FZNSolverInstance::getSolutionValue(Id* id) {
    IdMap<Expression*>::iterator it = _solution.find(id);
    if(it == _solution.end()) {
      std::stringstream ssm;
      ssm << "FZNSolverInstance::getSolutionValue: cannot find solution for id: " << *id;
      throw InternalError(ssm.str());
    }
    return it->second;
  }
  
  SolverInstance::Status 
  FZNSolverInstance::nextSolution(void) {   
    _fzn = _env.flat(); // point to the new flat model
    return solve();
  }
  
  SolverInstance::Status 
  FZNSolverInstance::best(VarDecl* obj, bool minimize, bool print) {
    _fzn = _env.flat();
    GCLock lock;
    // replace old solve item with min/max objective  
    SolveI* solveI = _fzn->solveItem();
    SolveI* objective = minimize ? SolveI::min(Location(),obj->id()):
                                   SolveI::max(Location(),obj->id());
    
    for(ExpressionSetIter it = solveI->ann().begin(); it!= solveI->ann().end(); ++it) {
      if(Call* c = (*it)->dyn_cast<Call>()) {
        if(c->id() == constants().ann.combinator) {
          continue;
        }
      }
      objective->ann().add(MiniZinc::copy(_env.envi(),(*it)));
    }
    solveI->remove();
    _fzn->addItem(objective);
    _env.flat()->compact();
    //std::cerr << "DEBUG: printing modified fzn model for BEST:\n";
    //debugprint(_fzn);
    //std::cerr << "====================================\n";
    
    if(false){
      std::cout <<"++++++++++++++ before ++++++++++++++++"<<std::endl;
      Printer p(std::cout);
      p.print(_env.flat());
      std::cout << "============== before ==============="<< std::endl;
      
      translateObj(_env);
      /*
      std::cout <<"++++++++++++++ after ++++++++++++++++"<<std::endl;
      p.print(_env.flat());
      std::cout << "============== after ==============="<< std::endl;
       */
    }
    SolverInstance::Status status = solve();
    if(print && status == SolverInstance::SUCCESS)
      _env.evalOutput(std::cerr);
    return status;
  }
  
  //extensions David
  void translateObj(Env& e) {
    // how to find out whether tranlating or not?
    // how to deal with interger objective functions
    // test whether fzn-cplex is happy with linear objective functions
    
    std::cout << "entered translate objective"<< std::endl;
    GCLock lock;
    Model* m = e.flat();
    SolveI* si = m->solveItem();
    si->remove();
    Id* si_id_0 = si->e()->cast<Id>();
    
    //todo test whether the solve item is acutally float, the followin code is based on this assupiton
    if(!si_id_0->decl()->e()){
      IdMap<Expression*> definitionMap;
      //todo: how can I avoid the same id twice on the map
      //which items do I  need on the map
      
      //iterate over all costraints in model
      for (ConstraintIterator it = e.flat()->begin_constraints();
           it != e.flat()->end_constraints(); ++it) {
        //iterate over all elements in each constraint
        for (ExpressionSetIter anns = it->e()->ann().begin();
             anns != it->e()->ann().end(); ++anns) {
          //if constraint "defines_var"
          if (Call* ann_c = (*anns)->dyn_cast<Call>()) {
            if (ann_c->id()==constants().ann.defines_var) {
              Id* ident = ann_c->args()[0]->cast<Id>();
              //put constraint onto map and  remove them from model, but add them later again
              definitionMap.insert(ident, it->e());
              it->remove();
            }
          }
        }
        if (Call* c = it->e()->dyn_cast<Call>()) {
          // TODO: can this introduce cycles?
          if (c->id()==constants().ids.int2float) {
            if (Id* ident = c->args()[0]->dyn_cast<Id>()) {
              if (definitionMap.find(ident)==definitionMap.end())
              definitionMap.insert(ident, c);
              it->remove();
            }

            if (Id* ident = c->args()[1]->dyn_cast<Id>()) {
              if (definitionMap.find(ident)==definitionMap.end())
              definitionMap.insert(ident, c);
            }

          }
        }
      }
      
      
      Id* si_id = si->e()->cast<Id>(); // solve maximize si_id
      IdMap<Expression*>::iterator si_it =  definitionMap.find(si_id);
      Call* si_call = si_it->second->dyn_cast<Call>(); //float_lin_eq(X_INTRODUCED_8,[profit,X_INTRODUCED_5,X_INTRODUCED_3],-0.0):: defines_var(profit)
      ASTExprVec<Expression> si_args = si_call->args();
      std::cout << "solve item: " << *si_args[0] << std::endl;
      std::cout << "solve item: " << *si_args[1] << std::endl;
      std::cout << "solve item: " << *si_args[2] << std::endl;
      ArrayLit* int_lin_args = si_args[1]->dyn_cast<ArrayLit>();
      
      
      
//      std::cout << "juhaaaa" << int_lin_args->v() << std::endl;
      //todo check if si_args are variables or arrays
      ASTExprVec<Expression> obj_var = si_args[1]->dyn_cast<ArrayLit>()->v();
      float si_const = si_args[2]->dyn_cast<FloatLit>()->v();
      
      std::vector<Expression*> var;
      std::vector<Expression*> qMat;
      std::vector<Expression*> cVec;
      std::vector<int> q_vector;
      
      ASTExprVec<Expression> si_args_red;
    
      int idx = 0;
      for(int i=0; i<obj_var.size();i++){
        //if not objective variable
        if(obj_var[i]->dyn_cast<Id>()->str() != si_id_0->str()){
          //used to construct the qMat
          std::vector<int> tmp_vector(obj_var.size()-1, 0);
          Id* quad_var_id = obj_var[i]->dyn_cast<Id>();
          IdMap<Expression*>::iterator var_it = definitionMap.find(quad_var_id);
          //assumption: the call is float_times -> needs to be veryfied
          if(Call* quad_fun = var_it->second->dyn_cast<Call>()){
            if(quad_fun->id() == constants().ids.float_.times){
              var.push_back(quad_var_id);
              definitionMap.remove(quad_var_id);
              //assumption -> same variable is not in float_times and somehow linear -> is that true??
              cVec.push_back(new IntLit(Location(),0));
              tmp_vector[idx]=1;
            }
            else{
              cVec.push_back(new IntLit(Location(),1));
              definitionMap.remove(quad_var_id);
              var.push_back(quad_var_id);
            }
          }
          q_vector.insert(q_vector.end(),tmp_vector.begin(),tmp_vector.end());
          idx++;
        }
      }
      for(int i=0; i<var.size() ;i++){
      }
      for(int idx=0;idx<q_vector.size();idx++){
        qMat.push_back(new IntLit(Location(),q_vector[idx]));
      }

      SolveI* nsi = NULL;
      
      std::vector<Expression*> argsExpr;
      
      argsExpr.push_back(new ArrayLit(Location(),var));
      argsExpr.push_back(new ArrayLit(Location(),qMat));
      argsExpr.push_back(new ArrayLit(Location(),cVec));
      //add a constant at the end, probably not used for the progressive hedging implementation
      argsExpr.push_back(new FloatLit(Location(),si_const));
      
      //how do I determine wether min or max in case of satisfy for the PH
      //could make it default for the initial tests
      Call* c = new Call(Location(), constants().ids.quad_obj, argsExpr);
      c->type(Type::varint());
      
      switch (si->st()) {
          case SolveI::ST_SAT:
          nsi = SolveI::sat(Location());
          break;
          case SolveI::ST_MIN:
          nsi = SolveI::min(Location(),c);
          break;
          case SolveI::ST_MAX:
          nsi = SolveI::max(Location(),c);
          break;
      }
      
      for(IdMap<Expression*>::iterator it = definitionMap.begin();
          it != definitionMap.end();it++){
        Call* tmp = it->second->dyn_cast<Call>();
        std::vector<Expression*> argsExpr;
        for(int i=0; i<tmp->args().size();i++){
            argsExpr.push_back(tmp->args()[i]);
        }
        Call* c_tmp = new Call(Location(), tmp->id(),argsExpr);
        ConstraintI* c = new ConstraintI(Location(),c_tmp);
        m->addItem(c);
      }
      m->addItem(nsi);

    }
    else{
      std::cout << "should not enter here in fzn-solverinstance.cpp -> translateObj(..)"<<std::endl;
       }
  }
}
