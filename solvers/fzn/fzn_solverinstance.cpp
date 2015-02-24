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

#include "fzn_solverinstance.hh"

#include <minizinc/parser.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>

namespace MiniZinc {
  
  namespace {
    class FznProcess {
    protected:
      std::string _fzncmd;
      bool _canPipe;
      Model* _flat;
    public:
      FznProcess(const std::string& fzncmd, bool pipe, Model* flat) : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat) {}
      std::stringstream run(void) {
        int pipes[2][2];
        pipe(pipes[0]);
        pipe(pipes[1]);

        std::string fznFile;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX";
          mkstemp(tmpfile);
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            Item* item = *it;
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
          std::stringstream result;
          
          fd_set fdset;
          struct timeval timeout;
          FD_ZERO(&fdset);
          FD_SET(pipes[1][0], &fdset);
          
          struct timeval starttime;
          gettimeofday(&starttime, NULL);
          
          int timeout_sec = 10;
          
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
          return result;
        } else {
          close(STDOUT_FILENO);
          close(STDIN_FILENO);
          dup2(pipes[0][0], STDIN_FILENO);
          dup2(pipes[1][1], STDOUT_FILENO);
          close(pipes[0][0]);
          close(pipes[1][1]);
          close(pipes[1][0]);
          close(pipes[0][1]);
          char* argv[] = {strdup(_fzncmd.c_str()),strdup("-a"),strdup("-"),0};
          if (!_canPipe) {
            argv[2] = strdup(fznFile.c_str());
          }
          execvp(argv[0],argv);
        }
        assert(false);
      }
    };
  }
  
  FZNSolverInstance::FZNSolverInstance(Env& env, const Options& options)
  : SolverInstanceImpl<FZNSolver>(env,options), _fzn(env.flat()), _ozn(env.output()) {}
  
  FZNSolverInstance::~FZNSolverInstance(void) {}
  
  SolverInstance::Status
  FZNSolverInstance::next(void) { return SolverInstance::ERROR; }

  namespace {
    ArrayLit* b_arrayXd(ASTExprVec<Expression> args, int d) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(args[d]);
      std::vector<std::pair<int,int> > dims(d);
      unsigned int dim1d = 1;
      for (int i=0; i<d; i++) {
        IntSetVal* di = eval_intset(args[i]);
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
    FznProcess proc("fzn-gecode",false,_fzn);
    std::stringstream result = proc.run();
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
      if (line=="----------") {
        if (hadSolution) {
          for (ASTStringMap<DE>::t::iterator it=declmap.begin(); it != declmap.end(); ++it) {
            it->second.first->e(it->second.second);
          }
        }
        Model* sm = parseFromString(solution, "solution.szn", includePaths, true, std::cerr);
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
              ArrayLit* al = b_arrayXd(c->args(), c->args().size()-1);
              it->second.first->e(al);
            } else {
              it->second.first->e(ai->e());
            }
          }
        }
        delete sm;
        hadSolution = true;
      } else if (line=="==========") {
        return hadSolution ? SolverInstance::OPT : SolverInstance::UNSAT;
      } else if(line=="=====UNSATISFIABLE=====") {
        return SolverInstance::UNSAT;
      } else if(line=="=====UNBOUNDED=====") {
        return SolverInstance::UNKNOWN;
      } else if(line=="=====UNKNOWN=====") {
        return SolverInstance::UNKNOWN;
      } else {
        solution += line;
      }
    }
    
    return hadSolution ? SolverInstance::SAT : SolverInstance::UNSAT;
  }
  
  void
  FZNSolverInstance::processFlatZinc(void) {}  
  
  Expression*
  FZNSolverInstance::getSolutionValue(Id* id) {
    assert(false);
  }
  
  bool 
  FZNSolverInstance::updateIntBounds(VarDecl* vd, int lb, int ub) {
    return false; // TODO: FZN should inherit from non-incremental interface, as soon as it is done
  }
  
}