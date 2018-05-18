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

#include <minizinc/solvers/fzn_solverinstance.hh>
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
  
  FZN_SolverFactory::FZN_SolverFactory(void) {
    SolverConfig sc("org.minizinc.mzn-fzn",MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
    sc.name("Generic FlatZinc driver");
    sc.mznlibVersion(1);
    sc.description("MiniZinc generic FlatZinc solver plugin");
    sc.requiredFlags({"-f"});
    SolverConfigs::registerBuiltinSolver(sc);
  }
  
  string FZN_SolverFactory::getDescription()  {
    string v = "FZN solver plugin, compiled  " __DATE__ "  " __TIME__;
    return v;
  }


  string FZN_SolverFactory::getVersion() {
    return MZN_VERSION_MAJOR;
  }

  string FZN_SolverFactory::getId()
  {
    return "org.minizinc.mzn-fzn";
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
    << "  --fzn-time-limit <ms>\n     Set a hard timelimit that overrides those set for the solver using --fzn-flag(s).\n"
    << "  --fzn-sigint\n     Send SIGINT instead of SIGTERM.\n"
    << "  -a, --all, --all-solns, --all-solutions\n     Print all solutions.\n"
    << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is solver-dependent.\n"
    << "  -k, --keep-files\n     For compatibility only: to produce .ozn and .fzn, use mzn2fzn\n"
                           "     or <this_exe> --fzn ..., --ozn ...\n"
    << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags instead.\n"
    ;
  }

  SolverInstanceBase::Options* FZN_SolverFactory::createOptions(void) {
    return new FZNSolverOptions;
  }

  SolverInstanceBase* FZN_SolverFactory::doCreateSI(Env& env, std::ostream& log, SolverInstanceBase::Options* opt) {
    return new FZNSolverInstance(env, log, opt);
  }

  bool FZN_SolverFactory::processOption(SolverInstanceBase::Options* opt, int& i, int argc, const char** argv)
  {
    FZNSolverOptions& _opt = static_cast<FZNSolverOptions&>(*opt);
    CLOParser cop( i, argc, argv );
    string buffer;
    double dd;
    int nn=-1;
    
    if ( cop.getOption( "-f --solver --flatzinc-cmd", &buffer) ) {
      _opt.fzn_solver = buffer;
    } else if ( cop.getOption( "-b --backend --solver-backend", &buffer) ) {
      _opt.backend = buffer;
    } else if ( cop.getOption( "--fzn-flags --flatzinc-flags", &buffer) ) {
      string old = _opt.fzn_flags;
      old += ' ';
      old += buffer;
      _opt.fzn_flags = old;
    } else if ( cop.getOption( "--fzn-time-limit", &nn) ) {
      _opt.fzn_time_limit_ms = nn;
    } else if ( cop.getOption( "--fzn-sigint") ) {
      _opt.fzn_sigint = true;
    } else if ( cop.getOption( "--fzn-flag --flatzinc-flag", &buffer) ) {
      string old = _opt.fzn_flag;
      old += " \"";
      old += buffer;
      old += "\" ";
      _opt.fzn_flag = old;
    } else if ( cop.getOption( "-n --num-solutions", &nn) ) {
      _opt.numSols = nn;
    } else if ( cop.getOption( "-a --all --all-solns --all-solutions") ) {
      _opt.allSols = true;
    } else if ( cop.getOption( "-p --parallel", &nn) ) {
      _opt.parallel = nn;
    } else if ( cop.getOption( "-k --keep-files" ) ) {
    } else if ( cop.getOption( "-r --seed --random-seed", &dd) ) {
    } else {
      return false;
    }
    return true;
  }
  
  namespace {

#ifdef _WIN32

    void ReadPipePrint(HANDLE g_hCh, bool* _done, ostream* pOs, Solns2Out* pSo, mutex* mtx) {
      bool& done = *_done;
      assert( pOs!=0 || pSo!=0 );
      while (!done) {
        char buffer[5255];
        DWORD count = 0;
        bool bSuccess = ReadFile(g_hCh, buffer, sizeof(buffer) - 1, &count, NULL);
        if (bSuccess && count > 0) {
          buffer[count] = 0;
          lock_guard<mutex> lck(*mtx);
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
    void TimeOut(HANDLE hProcess, bool* done, int timeout, timed_mutex* mtx) {
      if (timeout > 0) {
        if (!mtx->try_lock_for(std::chrono::milliseconds(timeout))) {
          if (!*done) {
            *done = true;
            TerminateProcess(hProcess,0);
          }
        }
      }
    }
#endif

    class FznProcess {
    protected:
      vector<string> _fzncmd;
      bool _canPipe;
      Model* _flat;
      Solns2Out* pS2Out;
      int timelimit;
      bool sigint;
    public:
      FznProcess(vector<string>& fzncmd, bool pipe, Model* flat, Solns2Out* pso, int tl, bool si)
        : _fzncmd(fzncmd), _canPipe(pipe), _flat(flat), pS2Out(pso), timelimit(tl), sigint(si) {
        assert( 0!=_flat );
        assert( 0!=pS2Out );
      }
      std::string run(void) {
#ifdef _WIN32
        //TODO: implement hard timelimits for windows
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
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            if(!(*it)->removed() && !(*it)->isa<IncludeI>()) {
              Item* item = *it;
              p.print(item);
            }
          }
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

        CloseHandle(piProcInfo.hThread);
        delete cmdstr;

        if (_canPipe) {
          DWORD dwWritten;
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            if(!(*it)->removed() && !(*it)->isa<IncludeI>()) {
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
        bool done = false;
        // Threaded solution seems simpler than asyncronous pipe reading
        mutex pipeMutex;
        timed_mutex terminateMutex;
        terminateMutex.lock();
        thread thrStdout(ReadPipePrint, g_hChildStd_OUT_Rd, &done, nullptr, pS2Out, &pipeMutex);
        thread thrStderr(ReadPipePrint, g_hChildStd_ERR_Rd, &done, &cerr, nullptr, &pipeMutex);
        thread thrTimeout(TimeOut, piProcInfo.hProcess, &done, timelimit, &terminateMutex);
        thrStdout.join();
        thrStderr.join();
        terminateMutex.unlock();
        thrTimeout.join();
        CloseHandle(piProcInfo.hProcess);

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
        int tmpfile_desc = -1;
        if (!_canPipe) {
          char tmpfile[] = "/tmp/fznfileXXXXXX.fzn";
          tmpfile_desc = mkstemps(tmpfile, 4);
          if (tmpfile_desc == -1) {
            throw InternalError("Error occurred when executing FZN solver, could not create temporary file for FlatZinc");
          }
          fznFile = tmpfile;
          std::ofstream os(tmpfile);
          Printer p(os, 0, true);
          for (Model::iterator it = _flat->begin(); it != _flat->end(); ++it) {
            if(!(*it)->removed() && !(*it)->isa<IncludeI>()) {
              Item* item = *it;
              p.print(item);
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
              if(!(*it)->removed() && !(*it)->isa<IncludeI>()) {
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

          struct timeval starttime;
          gettimeofday(&starttime, NULL);

          struct timeval timeout_orig;
          timeout_orig.tv_sec = timelimit / 1000;
          timeout_orig.tv_usec = (timelimit % 1000) * 1000;
          struct timeval timeout = timeout_orig;

          bool done = false;
          while (!done) {
            FD_SET(pipes[1][0], &fdset);
            FD_SET(pipes[2][0], &fdset);
            int sel = select(FD_SETSIZE, &fdset, NULL, NULL, timelimit==0 ? NULL : &timeout);
            if (sel==-1) {
              if (errno != EINTR) {
                // some error has happened
                throw InternalError(std::string("Error in communication with solver: ")+strerror(errno));
              }
            }
            if (timelimit!=0) {
              timeval currentTime;
              gettimeofday(&currentTime, NULL);
              if (sel != 0) {
                timeval elapsed;
                elapsed.tv_sec = currentTime.tv_sec - starttime.tv_sec;
                elapsed.tv_usec = currentTime.tv_usec - starttime.tv_usec;
                if(elapsed.tv_usec < 0) {
                  elapsed.tv_sec--;
                  elapsed.tv_usec += 1000000;
                }
                // Reset timeout to original limit
                timeout = timeout_orig;
                // Subtract elapsed time
                timeout.tv_usec = timeout.tv_usec - elapsed.tv_usec;
                if (timeout.tv_usec < 0) {
                  timeout.tv_sec--;
                  timeout.tv_usec += 1000000;
                }
                timeout.tv_sec = timeout.tv_sec - elapsed.tv_sec;
              } else {
                timeout.tv_usec = 0;
                timeout.tv_sec = 0;
              }
              if(timeout.tv_sec <= 0 && timeout.tv_usec <= 0) {
                if(sigint) {
                  kill(childPID, SIGINT);
                  timeout.tv_sec = 0;
                  timeout.tv_usec = 200000;
                  timeout_orig = timeout;
                  starttime = currentTime;
                  sigint = false;
                } else {
                  kill(childPID, SIGTERM);
                  pS2Out->feedRawDataChunk( "\n" );   // in case last chunk did not end with \n
                  done = true;
                }
              }
            }

            for ( int i=1; i<=2; ++i ) {
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
            remove(fznFile.c_str());
          }
          close(pipes[1][0]);
          close(pipes[2][0]);
          if (tmpfile_desc != -1)
            close(tmpfile_desc);
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

          int status = execvp(argv[0], argv); // execvp only returns if an error occurs.
          assert(status == -1); // the returned value will always be -1
          std::stringstream ssm;
          ssm << "Error occurred when executing FZN solver with command \"";
          for ( auto& s: cmd_line )
            ssm << s << ' ';
          ssm << "\".";
          throw InternalError(ssm.str());
        }
    }
#endif
    };
  }

  FZNSolverInstance::FZNSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

  FZNSolverInstance::~FZNSolverInstance(void) {}

  SolverInstance::Status
  FZNSolverInstance::solve(void) {
    FZNSolverOptions& opt = static_cast<FZNSolverOptions&>(*_options);
    if (opt.fzn_solver.empty()) {
      throw InternalError("No FlatZinc solver specified");
    }
    /// Passing options to solver
    vector<string> cmd_line;
    cmd_line.push_back( opt.fzn_solver );
    string sBE = opt.backend;
    if ( sBE.size() ) {
      cmd_line.push_back( "-b" );
      cmd_line.push_back( sBE );
    }
    string sFlags = opt.fzn_flags;
    if ( sFlags.size() )
      cmd_line.push_back( sFlags );
    string sFlagQuoted = opt.fzn_flag;
    if ( sFlagQuoted.size() )
      cmd_line.push_back( sFlagQuoted );
    if ( opt.numSols != 1 ) {
      ostringstream oss;
      oss << "-n " << opt.numSols;
      cmd_line.push_back( oss.str() );
    }
    if ( opt.allSols ) {
      cmd_line.push_back( "-a" );
    }
    if ( opt.parallel.size() ) {
      ostringstream oss;
      oss << "-p " << opt.parallel;
      cmd_line.push_back( oss.str() );
    }
    if (opt.printStatistics) {
      cmd_line.push_back( "-s" );
    }
    if (opt.verbose) {
      cmd_line.push_back( "-v" );
      std::cerr << "Using FZN solver " << cmd_line[0]
        << " for solving, parameters: ";
      for ( int i=1; i<cmd_line.size(); ++i )
        cerr << "" << cmd_line[i] << " ";
      cerr << std::endl;
    }
    int timelimit = opt.fzn_time_limit_ms;
    bool sigint = opt.fzn_sigint;
    
    FznProcess proc(cmd_line, false, _fzn, getSolns2Out(), timelimit, sigint);
    proc.run();

    return getSolns2Out()->status;
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
