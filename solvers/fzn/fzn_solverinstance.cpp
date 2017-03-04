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

using namespace std;

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
  
  SolverFactory* SolverFactory::createF_FZN() {
    return new FZN_SolverFactory;
  }

  string FZN_SolverFactory::getVersion()
  {
    string v = "FZN solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }

  void FZN_SolverFactory::printHelp(ostream& os)
  {
    os
    << "MZN-FZN plugin options:" << std::endl
    << "  -f, --solver, --flatzinc-cmd <exe>\n     the backend solver filename. The default: flatzinc.\n"
    << "  -b, --backend, --solver-backend <be>\n     the backend codename. Currently passed to the solver.\n"
    << "  --fzn-flags <options>, --flatzinc-flags <options>\n     Specify option to be passed to the FlatZinc interpreter.\n"
    << "  --fzn-flag <option>, --flatzinc-flag <option>\n     As above, but for a single option string that need to be quoted in a shell.\n"
    << "  -n <n>, --num-solutions <n>\n     An upper bound on the number of solutions to output. The default should be 1.\n"
    << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions.\n"
    << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
    << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
                           "     or <this_exe> --fzn ..., --ozn ...\n"
    << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags instead.\n"
    ;
  }

  bool FZN_SolverFactory::processOption(int& i, int argc, const char** argv)
  {
    CLOParser cop( i, argc, argv );
    string buffer;
    double dd;
    int nn=-1;
    
    if ( cop.getOption( "-f --solver --flatzinc-cmd", &buffer) ) {
      _options.setStringParam(constants().opts.solver.fzn_solver.str(), buffer);
    } else if ( cop.getOption( "-b --backend --solver-backend", &buffer) ) {
      _options.setStringParam( "backend", buffer );
    } else if ( cop.getOption( "--fzn-flags --flatzinc-flags", &buffer) ) {
      string old = _options.getStringParam(constants().opts.solver.fzn_flags.str(), "");
      old += ' ';
      old += buffer;
      _options.setStringParam(constants().opts.solver.fzn_flags.str(), old);
    } else if ( cop.getOption( "--fzn-flag --flatzinc-flag", &buffer) ) {
      string old = _options.getStringParam(constants().opts.solver.fzn_flag.str(), "");
      old += " \"";
      old += buffer;
      old += "\" ";
      _options.setStringParam(constants().opts.solver.fzn_flag.str(), buffer);
    } else if ( cop.getOption( "-n --num-solutions", &nn) ) {
      _options.setIntParam(constants().opts.solver.numSols.str(), nn);
    } else if ( cop.getOption( "-a --all --all-solns --all-solutions") ) {
      _options.setBoolParam(constants().opts.solver.allSols.str(), true);
    } else if ( cop.getOption( "-p --parallel", &nn) ) {
      _options.setIntParam(constants().opts.solver.fzn_flag.str(), nn);
    } else if ( cop.getOption( "-k --keep-files" ) ) {
    } else if ( cop.getOption( "-r --seed --random-seed", &dd) ) {
    } else {
      return false;
    }
    return true;
  }
  
  namespace {

#ifdef _WIN32
    mutex mtx;
    void ReadPipePrint(HANDLE g_hCh, ostream* pOs, Solns2Out* pSo = nullptr) {
      assert( pOs!=0 || pSo!=0 );
      bool done = false;
      while (!done) {
        char buffer[5255];
        DWORD count = 0;
        bool bSuccess = ReadFile(g_hCh, buffer, sizeof(buffer) - 1, &count, NULL);
        if (bSuccess && count > 0) {
          buffer[count] = 0;
          lock_guard<mutex> lck(mtx);
          if (pSo)
            pSo->feedRawDataChunk( buffer );
          if (pOs)
            (*pOs) << buffer << flush;
        }
        else {
          if (pSo)
            pSo->feedRawDataChunk( "\n" );   // in case the last chunk had none
          done = true;
        }
      }
    }
#endif

    class FznProcess {
    protected:
      vector<string> _fzncmd;
      bool _canPipe;
      Model* _flat=0;
      Solns2Out* pS2Out=0;
    public:
      FznProcess(vector<string>& fzncmd, bool pipe, Model* flat, Solns2Out* pso)
        : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat), pS2Out(pso) {
        assert( 0!=_flat );
        assert( 0!=pS2Out );
      }
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
        for (auto& iCmdl: _fzncmd)
          cmdline << iCmdl << " ";
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
            if(!(*it)->removed()) {
              std::stringstream ss;
              Item* item = *it;
              ss << *item;
              std::string str = ss.str();
              bSuccess = WriteFile(g_hChildStd_IN_Wr, str.c_str(),
                  str.size(), &dwWritten, NULL);
            }
          }
        }

        // Stop ReadFile from blocking
        CloseHandle(g_hChildStd_OUT_Wr);
        CloseHandle(g_hChildStd_ERR_Wr);
        // Just close the child's in pipe here
        CloseHandle(g_hChildStd_IN_Rd);

        // Threaded solution seems simpler than asyncronous pipe reading
        thread thrStdout(ReadPipePrint, g_hChildStd_OUT_Rd, nullptr, pS2Out);
        thread thrStderr(ReadPipePrint, g_hChildStd_ERR_Rd, &cerr, nullptr);
        thrStdout.join();
        thrStderr.join();

        // Hard timeout: GenerateConsoleCtrlEvent()

        if (!_canPipe) {
          remove(fznFile.c_str());
        }

        return result.str();
      }
#else
        int pipes[3][2];
        pipe(pipes[0]);
        pipe(pipes[1]);
        pipe(pipes[2]);

        std::string fznFile;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX.fzn";
          mkstemps(tmpfile, 4);
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            if(!(*it)->removed()) {
              Item* item = *it;
              os << *item;
            }
          }
        }

        // Make sure to reap child processes to avoid creating zombies
        signal(SIGCHLD, SIG_IGN);
            
        if (int childPID = fork()) {
          close(pipes[0][0]);
          close(pipes[1][1]);
          close(pipes[2][1]);
          if (_canPipe) {
            for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
              if(!(*it)->removed()) {
                std::stringstream ss;
                Item* item = *it;
                ss << *item;
                std::string str = ss.str();
                write(pipes[0][1], str.c_str(), str.size());
              }
            }
          }
          close(pipes[0][1]);
          std::stringstream result;

          fd_set fdset;
          FD_ZERO(&fdset);

          bool done = false;
          while (!done) {
            FD_SET(pipes[1][0], &fdset);
            FD_SET(pipes[2][0], &fdset);
            if ( 0>=select(FD_SETSIZE, &fdset, NULL, NULL, NULL) )
            {
              kill(childPID, SIGKILL);
              pS2Out->feedRawDataChunk( "\n" );   // in case last chunk did not end with \n
              done = true;
            } else {
              for ( int i=1; i<=2; ++i )
                if ( FD_ISSET( pipes[i][0], &fdset ) )
                {
                  char buffer[1000];
                  int count = read(pipes[i][0], buffer, sizeof(buffer) - 1);
                  if (count > 0) {
                    buffer[count] = 0;
                    if ( 1==i ) {
//                       cerr << "mzn-fzn: raw chunk stdout:::  " << flush;
//                       cerr << buffer << flush;
                      pS2Out->feedRawDataChunk( buffer );
                    }
                    else {
                      cerr << buffer << flush;
                    }
                  }
                  else if ( 1==i ) {
                    pS2Out->feedRawDataChunk("\n");   // in case last chunk did not end with \n
                    done = true;
                  }
                }
            }
          }

          if (!_canPipe) {
            //remove(fznFile.c_str());
          }
          return result.str();
        }
        else {
          close(STDOUT_FILENO);
          close(STDERR_FILENO);
          close(STDIN_FILENO);
          dup2(pipes[0][0], STDIN_FILENO);
          dup2(pipes[1][1], STDOUT_FILENO);
          dup2(pipes[2][1], STDERR_FILENO);
          close(pipes[0][0]);
          close(pipes[0][1]);
          close(pipes[1][1]);
          close(pipes[1][0]);
          close(pipes[2][1]);
          close(pipes[2][0]);

          std::vector<char*> cmd_line;
          for (auto& iCmdl: _fzncmd) {
            std::istringstream iss(iCmdl);
            while (1) {
              string sBuf;
              iss >> std::skipws >> sBuf;
              if ( sBuf.size() )
                cmd_line.push_back( strdup(sBuf.c_str()) );
              else
                break;
            }
          }
          cmd_line.push_back(strdup(_canPipe ? "-" : fznFile.c_str()));

          char** argv = new char*[cmd_line.size() + 1];
          for (unsigned int i = 0; i < cmd_line.size(); i++)
            argv[i] = cmd_line[i];
          argv[cmd_line.size()] = 0;

          int status = execvp(argv[0], argv);
          if (status == -1) {
            std::stringstream ssm;
            ssm << "Error occurred when executing FZN solver with command \"";
            for ( auto& s: cmd_line )
              ssm << s << ' ';
            ssm << "\".";
            throw InternalError(ssm.str());
          }
        }
        assert(false);
    }
#endif
    };
  }

  FZNSolverInstance::FZNSolverInstance(Env& env, const Options& options)
    : SolverInstanceBase(env, options), _fzn(env.flat()), _ozn(env.output()) {}

  FZNSolverInstance::~FZNSolverInstance(void) {}

//   namespace {
//     ArrayLit* b_arrayXd(Env& env, ASTExprVec<Expression> args, int d) {
//       GCLock lock;
//       ArrayLit* al = eval_array_lit(env.envi(), args[d]);
//       std::vector<std::pair<int, int> > dims(d);
//       unsigned int dim1d = 1;
//       for (int i = 0; i < d; i++) {
//         IntSetVal* di = eval_intset(env.envi(), args[i]);
//         if (di->size() == 0) {
//           dims[i] = std::pair<int, int>(1, 0);
//           dim1d = 0;
//         }
//         else if (di->size() != 1) {
//           throw EvalError(env.envi(), args[i]->loc(), "arrayXd only defined for ranges");
//         }
//         else {
//           dims[i] = std::pair<int, int>(static_cast<int>(di->min(0).toInt()),
//             static_cast<int>(di->max(0).toInt()));
//           dim1d *= dims[i].second - dims[i].first + 1;
//         }
//       }
//       if (dim1d != al->v().size())
//         throw EvalError(env.envi(), al->loc(), "mismatch in array dimensions");
//       ArrayLit* ret = new ArrayLit(al->loc(), al->v(), dims);
//       Type t = al->type();
//       t.dim(d);
//       ret->type(t);
//       ret->flat(al->flat());
//       return ret;
//     }
//   }

  SolverInstance::Status
  FZNSolverInstance::solve(void) {
    /// Passing options to solver
    vector<string> cmd_line;
    cmd_line.push_back( _options.getStringParam(constants().opts.solver.fzn_solver.str(), "flatzinc") );
    string sBE = _options.getStringParam("backend", "");
    if ( sBE.size() ) {
      cmd_line.push_back( "-b" );
      cmd_line.push_back( sBE );
    }
    string sFlags = _options.getStringParam(constants().opts.solver.fzn_flags.str(), "");
    if ( sFlags.size() )
      cmd_line.push_back( sFlags );
    string sFlagQuoted = _options.getStringParam(constants().opts.solver.fzn_flag.str(), "");
    if ( sFlagQuoted.size() )
      cmd_line.push_back( sFlagQuoted );
    if ( _options.hasParam(constants().opts.solver.numSols.str()) ) {
      ostringstream oss;
      oss << _options.getIntParam(constants().opts.solver.numSols.str());
      cmd_line.push_back( "-n "+oss.str() );
    }
    if ( _options.getBoolParam(constants().opts.solver.allSols.str(), false) ) {
      cmd_line.push_back( "-a" );
    }
    if ( _options.hasParam(constants().opts.solver.threads.str()) ) {
      ostringstream oss;
      oss << _options.getIntParam(constants().opts.solver.threads.str());
      cmd_line.push_back( "-p "+oss.str() );
    }
    if (_options.getBoolParam(constants().opts.statistics.str(), false)) {
      cmd_line.push_back( "-s" );
    }
    if (_options.getBoolParam(constants().opts.verbose.str(), false)) {
      cmd_line.push_back( "-v" );
      std::cerr << "Using FZN solver " << cmd_line[0]
        << " for solving, parameters: ";
      for ( int i=1; i<cmd_line.size(); ++i )
        cerr << "" << cmd_line[i] << " ";
      cerr << std::endl;
    }
    
    FznProcess proc(cmd_line, false, _fzn, getSolns2Out());
    proc.run();

//     std::stringstream result;
//     result << r;
//     std::string solution;
// 
//     typedef std::pair<VarDecl*, Expression*> DE;
//     ASTStringMap<DE>::t declmap;
//     for (unsigned int i = 0; i < _ozn->size(); i++) {
//       if (VarDeclI* vdi = (*_ozn)[i]->dyn_cast<VarDeclI>()) {
//         declmap.insert(std::make_pair(vdi->e()->id()->str(), DE(vdi->e(), vdi->e()->e())));
//       }
//     }
// 
//     hadSolution = false;
//     while (result.good()) {
//       std::string line;
//       getline(result, line);
// 
//       if (beginswith(line, "----------")) {
//         if (hadSolution) {
//           for (ASTStringMap<DE>::t::iterator it = declmap.begin(); it != declmap.end(); ++it) {
//             it->second.first->e(it->second.second);
//           }
//         }
//         Model* sm = parseFromString(solution, "solution.szn", includePaths, true, false, false, std::cerr);
//         if (sm) {
//           for (Model::iterator it = sm->begin(); it != sm->end(); ++it) {
//             if (AssignI* ai = (*it)->dyn_cast<AssignI>()) {
//               ASTStringMap<DE>::t::iterator it = declmap.find(ai->id());
//               if (it == declmap.end()) {
//                 std::cerr << "Error: unexpected identifier " << ai->id() << " in output\n";
//                 exit(EXIT_FAILURE);
//               }
//               if (Call* c = ai->e()->dyn_cast<Call>()) {
//                 // This is an arrayXd call, make sure we get the right builtin
//                 assert(c->args()[c->args().size() - 1]->isa<ArrayLit>());
//                 for (unsigned int i = 0; i < c->args().size(); i++)
//                   c->args()[i]->type(Type::parsetint());
//                 c->args()[c->args().size() - 1]->type(it->second.first->type());
//                 ArrayLit* al = b_arrayXd(_env, c->args(), c->args().size() - 1);
//                 it->second.first->e(al);
//               }
//               else {
//                 it->second.first->e(ai->e());
//               }
//             }
//           }
//           delete sm;
//           hadSolution = true;
//         } else {
//           std::cerr << "\n\n\nError: solver output malformed; DUMPING: ---------------------\n\n\n";
//           cerr << result.str() << endl;
//           std::cerr << "\n\nError: solver output malformed (END DUMPING) ---------------------" << endl;
//           exit(EXIT_FAILURE);
//         }
//       }
//       else if (beginswith(line, "==========")) {
//         return hadSolution ? SolverInstance::OPT : SolverInstance::UNSAT;
//       }
//       else if (beginswith(line, "=====UNSATISFIABLE=====")) {
//         return SolverInstance::UNSAT;
//       }
//       else if (beginswith(line, "=====UNBOUNDED=====")) {
//         return SolverInstance::UNBND;
//       }
//       else if (beginswith(line, "=====UNSATorUNBOUNDED=====")) {
//         return SolverInstance::UNSATorUNBND;
//       }
//       else if (beginswith(line, "=====ERROR=====")) {
//         return SolverInstance__ERROR;
//       }
//       else if (beginswith(line, "=====UNKNOWN=====")) {
//         return SolverInstance::UNKNOWN;
//       }
//       else {
//         solution += line;
//       }
//     }
// 
//     return hadSolution ? SolverInstance::SAT : SolverInstance::UNSAT;
    return getSolns2Out()->status;
  }

//   void FZNSolverInstance::printSolution(ostream& os) {
//     assert(hadSolution);
//     _env.evalOutput(os);
//   }

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
