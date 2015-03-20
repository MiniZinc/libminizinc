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

namespace MiniZinc {
  
  namespace {
    class FznProcess {
    protected:
      std::string _fzncmd;
      bool _canPipe;
      Model* _flat;
    public:
      FznProcess(const std::string& fzncmd, bool pipe, Model* flat) : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat) {}
      void run(std::stringstream& result, Options& opt) {
        int pipes[2][2];
        pipe(pipes[0]);
        pipe(pipes[1]);

        std::string fznFile;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX.fzn";
          mkstemp(tmpfile);
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            Item* item = *it;
            if(SolveI* si = item->dyn_cast<SolveI>()) {
              if(si->combinator_lite()) {
                // remove combinator lite annotation
                si->ann().clear();
              }
            }
            os << *item;  
          }           
        }
        
        if (int childPID = fork()) {
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
          
          // TODO: how to disable timeout??
          int timeout_sec = 2147483647; //10 
          
          timeout.tv_sec = timeout_sec;
          timeout.tv_usec = 0;
          
          bool done = false;
          while (!done) {
           
            switch (select(FD_SETSIZE, &fdset, NULL, NULL, &timeout)) {
              case 0:
              {
                kill(childPID, SIGKILL);
                done = true;
              }
                break;
              case 1:
              {                
                char buffer[100];
                int count = read(pipes[1][0], buffer, sizeof(buffer)-1);
                if (count > 0) {                  
                  buffer[count] = 0;
                  result << buffer;                 
                  timeval currentTime, elapsed;
                  gettimeofday(&currentTime, NULL);
                  elapsed.tv_sec = currentTime.tv_sec - starttime.tv_sec;
                  elapsed.tv_usec = currentTime.tv_usec - starttime.tv_usec;
                  if (elapsed.tv_usec < 0) {
                    elapsed.tv_sec--;
                    elapsed.tv_usec += 1000000;
                  }
                  timeout.tv_sec = timeout_sec - elapsed.tv_sec;
                  if (elapsed.tv_usec > 0)
                    timeout.tv_sec--;
                  timeout.tv_usec = 1000000 - elapsed.tv_usec;
                  if (timeout.tv_sec <= 0 && timeout.tv_usec <=0)
                    done = true;
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
          
          if (!_canPipe) {
            remove(fznFile.c_str());
          } 
          return;
        } else {
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
          if(opt.hasParam(constants().solver_options.time_limit_sec.str())) {
            int time_ms = opt.getFloatParam(constants().solver_options.time_limit_sec.str())*1000;
            char time_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
            sprintf(time_c, "%d", time_ms); 
            // node limit
            if(opt.hasParam(constants().solver_options.node_limit.str())) {                           
              int nodes = opt.getIntParam(constants().solver_options.node_limit.str());
              char nodes_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
              //fail limit
              if(opt.hasParam(constants().solver_options.fail_limit.str())) {
                int fails = opt.getIntParam(constants().solver_options.fail_limit.str());
                char fails_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
                sprintf(fails_c, "%d", fails);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-fails"), fails_c, strdup("-node"), nodes_c, strdup("-time"), time_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[7] = strdup(fznFile.c_str());
                status = execvp(argv[0],argv);                  
              }
              else { // only node and time limit
                sprintf(nodes_c, "%d", nodes);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-node"), nodes_c,strdup("-time"), time_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[5] = strdup(fznFile.c_str());
                status = execvp(argv[0],argv);  
              }
            }
            // fail limit
            else if(opt.hasParam(constants().solver_options.fail_limit.str())) {
                int fails = opt.getIntParam(constants().solver_options.fail_limit.str());
                char fails_c[(sizeof(int)*CHAR_BIT-1)/3 + 3]; 
                sprintf(fails_c, "%d", fails);  
                char* argv[] = {strdup(_fzncmd.c_str()),strdup("-fail"), fails_c, strdup("-time"), time_c, strdup("-"),0};          
                if (!_canPipe) 
                  argv[5] = strdup(fznFile.c_str());
                status = execvp(argv[0],argv);  
            }                      
            else { // only time limit                     
              char* argv[] = {strdup(_fzncmd.c_str()),strdup("-time"), time_c, strdup("-"),0};          
              if (!_canPipe) 
                argv[3] = strdup(fznFile.c_str());
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
  : NISolverInstanceImpl<FZNSolver>(env,options), _fzn(env.flat()), _ozn(env.output()) {}
  
  FZNSolverInstance::~FZNSolverInstance(void) {}

  SolverInstanceBase*
  FZNSolverInstance::copy(void) {
    Env* env_copy = _env.copyEnv(); 
    Options options_copy; // TODO: do we need to use a reference/pointer instead because local variable will be removed?
    options_copy = _options.copyEntries(options_copy);
    return new FZNSolverInstance(*env_copy,options_copy);
  }
 
  namespace {
    ArrayLit* b_arrayXd(EnvI& envi, ASTExprVec<Expression> args, int d) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(envi, args[d]);
      std::vector<std::pair<int,int> > dims(d);
      unsigned int dim1d = 1;
      for (int i=0; i<d; i++) {
        IntSetVal* di = eval_intset(envi, args[i]);
        if (di->size()==0) {
          dims[i] = std::pair<int,int>(1,0);
          dim1d = 0;
        } else if (di->size() != 1) {
          throw EvalError(args[i]->loc(), "arrayXd only defined for ranges");
        } else {
          dims[i] = std::pair<int,int>(static_cast<int>(di->min(0).toInt()),
                                       static_cast<int>(di->max(0).toInt()));
          dim1d *= dims[i].second-dims[i].first+1;
        }
      }
      if (dim1d != al->v().size())
        throw EvalError(al->loc(), "mismatch in array dimensions");
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
    std::string solver_exec = _options.getStringParam("solver","flatzinc");
    // TODO: check if solver exec is in path
    FznProcess proc(solver_exec,false,_fzn); 
    std::stringstream result; 
    proc.run(result,_options);
    std::string solution;
    
    typedef std::pair<VarDecl*,Expression*> DE;
    ASTStringMap<DE>::t declmap;
    for (unsigned int i=0; i<_ozn->size(); i++) {
      if (VarDeclI* vdi = (*_ozn)[i]->dyn_cast<VarDeclI>()) {
        declmap.insert(std::make_pair(vdi->e()->id()->v(),DE(vdi->e(),vdi->e()->e())));
      }
    }

    bool hadSolution = false;
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
        for (Model::iterator it = sm->begin(); it != sm->end(); ++it) {          
          if (AssignI* ai = (*it)->dyn_cast<AssignI>()) {
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
              ArrayLit* al = b_arrayXd(env().envi(), c->args(), c->args().size()-1);
              it->second.first->e(al);              
              setSolution(it->second.first->id(),al);
            } else {             
              it->second.first->e(ai->e());            
              setSolution(it->second.first->id(),ai->e());
            }
          }
        }
        delete sm;
        hadSolution = true;
        if(_env.flat()->solveItem()->st() == SolveI::SolveType::ST_SAT) {
          return SolverInstance::SAT;
        }
      } else if (line==constants().solver_output.opt.str()) {
        return hadSolution ? SolverInstance::OPT : SolverInstance::UNSAT;
      } else if(line==constants().solver_output.unsat.str()) {
        return SolverInstance::UNSAT;
      } else if(line==constants().solver_output.unbounded.str()) {
        return SolverInstance::UNKNOWN;
      } else if(line==constants().solver_output.unknown.str()) {
        return SolverInstance::UNKNOWN;
      } else {
        line += "\n"; // break the line, especially in case it is a comment
        solution += line;
      }
    }
    
    // XXX should always return 'UNKNOWN' because not well-defined termination?
    return hadSolution ? SolverInstance::SAT : SolverInstance::UNKNOWN;
  }
  
  void
  FZNSolverInstance::processFlatZinc(void) {}  
  
  void
  FZNSolverInstance::setSolution(Id* id, Expression* e) {
    _solution.insert(id,e);
  }
  
  Expression*
  FZNSolverInstance::getSolutionValue(Id* id) {
    if(_solution.find(id) == _solution.end()) {
      std::stringstream ssm;
      ssm << "FZNSolverInstance::getSolutionValue: cannot find solution for id: " << *id;
      throw InternalError(ssm.str());
    }
    return _solution.get(id);
  }
  
  SolverInstance::Status 
  FZNSolverInstance::nextSolution(void) {   
    _fzn = _env.flat(); // point to the new flat model
    return solve();
  }
  
}