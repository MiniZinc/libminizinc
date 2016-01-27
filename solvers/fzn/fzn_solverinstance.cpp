/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

 /* This Source Code Form is subject to the terms of the Mozilla Public
  * License, v. 2.0. If a copy of the MPL was not distributed with this
  * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#ifdef _WIN32
#define NOMINMAX     // Need this before all (implicit) include's of Windows.h
#endif

#include "minizinc/solvers/fzn_solverinstance.hh"
const auto SolverInstance__ERROR = MiniZinc::SolverInstance::ERROR;  // before windows.h
#include <cstdio>
#include <fstream>

#include <minizinc/timer.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/parser.hh>
#include <minizinc/typecheck.hh>
#include <minizinc/builtins.hh>
#include <minizinc/eval_par.hh>

#ifdef _WIN32
#define NOMINMAX
#include <Windows.h>
#include <tchar.h>
//#include <atlstr.h>
#else
#include <unistd.h>
#include <sys/select.h>
#include <sys/time.h>
#endif
#include <sys/types.h>
#include <signal.h>
#include <thread>
#include <mutex>

namespace MiniZinc {

  class FZN_SolverFactory: public SolverFactory {
    Options _options;
  public:
    SolverInstanceBase* doCreateSI(Env& env) {
      return new FZNSolverInstance(env, _options);
    }
    string getVersion( );
    bool processOption(int& i, int argc, const char** argv);
    void printHelp(std::ostream& os);
  };
// #define __NO_EXPORT_FZN_SOLVERINSTANCE__  // define this to avoid exporting
#ifndef __NO_EXPORT_FZN_SOLVERINSTANCE__
  FZN_SolverFactory fzn_solverfactory;
#endif

  string FZN_SolverFactory::getVersion()
  {
    string v = "NICTA FZN solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }

  bool FZN_SolverFactory::processOption(int& i, int argc, const char** argv)
  {
    if (string(argv[i])=="--solver") {
      i++;
      if (i==argc) {
        goto error;
      }
      _options.setStringParam(constants().opts.solver.fzn_solver.str(), argv[i]);
    }
    return true;
  error:
    return false;
  }
  
  void FZN_SolverFactory::printHelp(ostream& os)
  {
    os
    << "FZN solver plugin options:" << std::endl
    << "--solver         the backend solver"
    << std::endl;
  }

  namespace {

#ifdef _WIN32
    mutex mtx;
    void ReadPipePrint(HANDLE g_hCh, ostream& os, ostream* pResult = nullptr) {
      bool done = false;
      while (!done) {
        char buffer[5255];
        DWORD count = 0;
        bool bSuccess = ReadFile(g_hCh, buffer, sizeof(buffer) - 1, &count, NULL);
        if (bSuccess && count > 0) {
          buffer[count] = 0;
          lock_guard<mutex> lck(mtx);
          do {
            if (pResult)
              (*pResult) << buffer;
            os << buffer << flush;
          } while (0);
        }
        else {
          done = true;
        }
      }
    }
#endif

    class FznProcess {
    protected:
      std::string _fzncmd;
      bool _canPipe;
      Model* _flat;
    public:
      FznProcess(const std::string& fzncmd, bool pipe, Model* flat) : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat) {}
      std::string run(void) {
#ifdef _WIN32
        std::stringstream result;

        SECURITY_ATTRIBUTES saAttr;
        saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
        saAttr.bInheritHandle = TRUE;
        saAttr.lpSecurityDescriptor = NULL;

        HANDLE g_hChildStd_IN_Rd = NULL;
        HANDLE g_hChildStd_IN_Wr = NULL;
        HANDLE g_hChildStd_OUT_Rd = NULL;
        HANDLE g_hChildStd_OUT_Wr = NULL;
        HANDLE g_hChildStd_ERR_Rd = NULL;
        HANDLE g_hChildStd_ERR_Wr = NULL;

        // Create a pipe for the child process's STDOUT. 
        if (!CreatePipe(&g_hChildStd_OUT_Rd, &g_hChildStd_OUT_Wr, &saAttr, 0))
          std::cerr << "Stdout CreatePipe" << std::endl;
        // Ensure the read handle to the pipe for STDOUT is not inherited.
        if (!SetHandleInformation(g_hChildStd_OUT_Rd, HANDLE_FLAG_INHERIT, 0))
          std::cerr << "Stdout SetHandleInformation" << std::endl;

        // Create a pipe for the child process's STDERR. 
        if (!CreatePipe(&g_hChildStd_ERR_Rd, &g_hChildStd_ERR_Wr, &saAttr, 0))
          std::cerr << "Stderr CreatePipe" << std::endl;
        // Ensure the read handle to the pipe for STDERR is not inherited.
        if (!SetHandleInformation(g_hChildStd_ERR_Rd, HANDLE_FLAG_INHERIT, 0))
          std::cerr << "Stderr SetHandleInformation" << std::endl;

        // Create a pipe for the child process's STDIN
        if (!CreatePipe(&g_hChildStd_IN_Rd, &g_hChildStd_IN_Wr, &saAttr, 0))
          std::cerr << "Stdin CreatePipe" << std::endl;
        // Ensure the write handle to the pipe for STDIN is not inherited. 
        if (!SetHandleInformation(g_hChildStd_IN_Wr, HANDLE_FLAG_INHERIT, 0))
          std::cerr << "Stdin SetHandleInformation" << std::endl;

        std::string fznFile;
        if (!_canPipe) {
          TCHAR szTempFileName[MAX_PATH];
          TCHAR lpTempPathBuffer[MAX_PATH];

          GetTempPath(MAX_PATH, lpTempPathBuffer);
          GetTempFileName(lpTempPathBuffer,
            "tmp_fzn_", 0, szTempFileName);

          fznFile = szTempFileName;
          MoveFile(fznFile.c_str(), (fznFile + ".fzn").c_str());
          fznFile += ".fzn";
          std::ofstream os(fznFile);
          Printer p(os, 0, true);
          p.print(_flat);
        }

        PROCESS_INFORMATION piProcInfo;
        STARTUPINFO siStartInfo;
        BOOL bSuccess = FALSE;

        // Set up members of the PROCESS_INFORMATION structure.
        ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

        // Set up members of the STARTUPINFO structure. 
        // This structure specifies the STDIN and STDOUT handles for redirection.
        ZeroMemory(&siStartInfo, sizeof(STARTUPINFO));
        siStartInfo.cb = sizeof(STARTUPINFO);
        siStartInfo.hStdError = g_hChildStd_ERR_Wr;
        siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
        siStartInfo.hStdInput = g_hChildStd_IN_Rd;
        siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

        std::stringstream cmdline;
        cmdline << strdup(_fzncmd.c_str()) << " " << strdup("-v") << " ";
        if (_canPipe) {
          cmdline << strdup("-");
        }
        else {
          cmdline << strdup(fznFile.c_str());
        }

        char* cmdstr = strdup(cmdline.str().c_str());

        bool processStarted = CreateProcess(NULL,
          cmdstr,        // command line 
          NULL,          // process security attributes 
          NULL,          // primary thread security attributes 
          TRUE,          // handles are inherited 
          0,             // creation flags 
          NULL,          // use parent's environment 
          NULL,          // use parent's current directory 
          &siStartInfo,  // STARTUPINFO pointer 
          &piProcInfo);  // receives PROCESS_INFORMATION 

        if (!processStarted) {
          std::stringstream ssm;
          ssm << "Error occurred when executing FZN solver with command \"" << cmdstr << "\".";
          throw InternalError(ssm.str());
        }

        CloseHandle(piProcInfo.hProcess);
        CloseHandle(piProcInfo.hThread);
        delete cmdstr;

        if (_canPipe) {
          DWORD dwWritten;
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            std::stringstream ss;
            Item* item = *it;
            ss << *item;
            std::string str = ss.str();
            bSuccess = WriteFile(g_hChildStd_IN_Wr, str.c_str(),
              str.size(), &dwWritten, NULL);
          }
        }

        // Stop ReadFile from blocking
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_ERR_Wr);
        // Just close the child's in pipe here
        CloseHandle(g_hChildStd_IN_Rd);

        // Threaded solution seems simpler than asyncronous pipe reading
        thread thrStdout(ReadPipePrint, g_hChildStd_OUT_Rd, ref(cout), &(result));
        thread thrStderr(ReadPipePrint, g_hChildStd_ERR_Rd, ref(cerr), nullptr);
        thrStdout.join();
        thrStderr.join();

        // Hard timeout: GenerateConsoleCtrlEvent()

        if (!_canPipe) {
          remove(fznFile.c_str());
        }

        return result.str();
      }
#else
        int pipes[2][2];
        pipe(pipes[0]);
        pipe(pipes[1]);

        std::string fznFile;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX.fzn";
          mkstemps(tmpfile, 4);
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            Item* item = *it;
            os << *item;
          }
        }

        // Make sure to reap child processes to avoid creating zombies
        signal(SIGCHLD, SIG_IGN);
            
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
          FD_ZERO(&fdset);
          FD_SET(pipes[1][0], &fdset);

          bool done = false;
          while (!done) {
            switch (select(FD_SETSIZE, &fdset, NULL, NULL, NULL)) {
            case 0:
            {
              kill(childPID, SIGKILL);
              done = true;
            }
            break;
            case 1:
            {
              char buffer[100];
              int count = read(pipes[1][0], buffer, sizeof(buffer) - 1);
              if (count > 0) {
                buffer[count] = 0;
                result << buffer;
              }
              else {
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
          return result.str();
        }
        else {
          close(STDOUT_FILENO);
          close(STDIN_FILENO);
          dup2(pipes[0][0], STDIN_FILENO);
          dup2(pipes[1][1], STDOUT_FILENO);
          close(pipes[0][0]);
          close(pipes[1][1]);
          close(pipes[1][0]);
          close(pipes[0][1]);

          std::vector<char*> cmd_line;
          cmd_line.push_back(strdup(_fzncmd.c_str()));
          cmd_line.push_back(strdup("-a"));
          cmd_line.push_back(strdup(_canPipe ? "-" : fznFile.c_str()));

          char** argv = new char*[cmd_line.size() + 1];
          for (unsigned int i = 0; i < cmd_line.size(); i++)
            argv[i] = cmd_line[i];
          argv[cmd_line.size()] = 0;

          int status = execvp(argv[0], argv);
          if (status == -1) {
            std::stringstream ssm;
            ssm << "Error occurred when executing FZN solver with command \"" << argv[0] << " " << argv[1] << " " << argv[2] << "\".";
            throw InternalError(ssm.str());
          }
        }
        assert(false);
    }
#endif
    };
  }

  FZNSolverInstance::FZNSolverInstance(Env& env, const Options& options)
    : SolverInstanceImpl<FZNSolver>(env, options), _fzn(env.flat()), _ozn(env.output()), hadSolution(false) {}

  FZNSolverInstance::~FZNSolverInstance(void) {}

  namespace {
    ArrayLit* b_arrayXd(Env& env, ASTExprVec<Expression> args, int d) {
      GCLock lock;
      ArrayLit* al = eval_array_lit(env.envi(), args[d]);
      std::vector<std::pair<int, int> > dims(d);
      unsigned int dim1d = 1;
      for (int i = 0; i < d; i++) {
        IntSetVal* di = eval_intset(env.envi(), args[i]);
        if (di->size() == 0) {
          dims[i] = std::pair<int, int>(1, 0);
          dim1d = 0;
        }
        else if (di->size() != 1) {
          throw EvalError(env.envi(), args[i]->loc(), "arrayXd only defined for ranges");
        }
        else {
          dims[i] = std::pair<int, int>(static_cast<int>(di->min(0).toInt()),
            static_cast<int>(di->max(0).toInt()));
          dim1d *= dims[i].second - dims[i].first + 1;
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
    std::string fzn_solver = _options.getStringParam(constants().opts.solver.fzn_solver.str(), "flatzinc");
    if (_options.getBoolParam(constants().opts.verbose.str(), false)) {
      std::cerr << "Using FZN solver " << fzn_solver << " for solving." << std::endl;
    }
    FznProcess proc(fzn_solver, false, _fzn);
    std::string r = proc.run();
    std::stringstream result;
    result << r;
    std::string solution;

    typedef std::pair<VarDecl*, Expression*> DE;
    ASTStringMap<DE>::t declmap;
    for (unsigned int i = 0; i < _ozn->size(); i++) {
      if (VarDeclI* vdi = (*_ozn)[i]->dyn_cast<VarDeclI>()) {
        declmap.insert(std::make_pair(vdi->e()->id()->str(), DE(vdi->e(), vdi->e()->e())));
      }
    }

    hadSolution = false;
    while (result.good()) {
      std::string line;
      getline(result, line);

      if (beginswith(line, "----------")) {
        if (hadSolution) {
          for (ASTStringMap<DE>::t::iterator it = declmap.begin(); it != declmap.end(); ++it) {
            it->second.first->e(it->second.second);
          }
        }
        Model* sm = parseFromString(solution, "solution.szn", includePaths, true, false, false, std::cerr);
        if (sm) {
          for (Model::iterator it = sm->begin(); it != sm->end(); ++it) {
            if (AssignI* ai = (*it)->dyn_cast<AssignI>()) {
              ASTStringMap<DE>::t::iterator it = declmap.find(ai->id());
              if (it == declmap.end()) {
                std::cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
                exit(EXIT_FAILURE);
              }
              if (Call* c = ai->e()->dyn_cast<Call>()) {
                // This is an arrayXd call, make sure we get the right builtin
                assert(c->args()[c->args().size() - 1]->isa<ArrayLit>());
                for (unsigned int i = 0; i < c->args().size(); i++)
                  c->args()[i]->type(Type::parsetint());
                c->args()[c->args().size() - 1]->type(it->second.first->type());
                ArrayLit* al = b_arrayXd(_env, c->args(), c->args().size() - 1);
                it->second.first->e(al);
              }
              else {
                it->second.first->e(ai->e());
              }
            }
          }
          delete sm;
          hadSolution = true;
        } else {
          std::cerr << "\n\n\nError: solver output malformed; DUMPING: ---------------------\n\n\n";
          cerr << result.str() << endl;
          std::cerr << "\n\nError: solver output malformed (END DUMPING) ---------------------" << endl;
          exit(EXIT_FAILURE);
        }
      }
      else if (beginswith(line, "==========")) {
        return hadSolution ? SolverInstance::OPT : SolverInstance::UNSAT;
      }
      else if (beginswith(line, "=====UNSATISFIABLE=====")) {
        return SolverInstance::UNSAT;
      }
      else if (beginswith(line, "=====UNBOUNDED=====")) {
        return SolverInstance::UNBND;
      }
      else if (beginswith(line, "=====UNSATorUNBOUNDED=====")) {
        return SolverInstance::UNSATorUNBND;
      }
      else if (beginswith(line, "=====ERROR=====")) {
        return SolverInstance__ERROR;
      }
      else if (beginswith(line, "=====UNKNOWN=====")) {
        return SolverInstance::UNKNOWN;
      }
      else {
        solution += line;
      }
    }

    return hadSolution ? SolverInstance::SAT : SolverInstance::UNSAT;
  }

  void FZNSolverInstance::printSolution(ostream& os) {
    assert(hadSolution);
    _env.evalOutput(os);
  }

  void
    FZNSolverInstance::processFlatZinc(void) {}

  void
    FZNSolverInstance::resetSolver(void) {}

  Expression*
    FZNSolverInstance::getSolutionValue(Id* id) {
    assert(false);
    return NULL;
  }
}
