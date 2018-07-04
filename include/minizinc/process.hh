/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PROCESS_HH__
#define __MINIZINC_PROCESS_HH__

#include <minizinc/solver.hh>

const auto SolverInstance__ERROR = MiniZinc::SolverInstance::ERROR;  // before windows.h
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
#include <vector>
#include <string>
#include <thread>
#include <mutex>

namespace MiniZinc {

#ifdef _WIN32

  template<class S2O>
  void ReadPipePrint(HANDLE g_hCh, bool* _done, std::ostream* pOs, S2O* pSo, std::mutex* mtx) {
    bool& done = *_done;
    assert( pOs!=0 || pSo!=0 );
    while (!done) {
      char buffer[5255];
      DWORD count = 0;
      BOOL bSuccess = ReadFile(g_hCh, buffer, sizeof(buffer) - 1, &count, NULL);
      if (bSuccess && count > 0) {
        buffer[count] = 0;
        std::lock_guard<std::mutex> lck(*mtx);
        if (pSo)
          pSo->feedRawDataChunk( buffer );
        if (pOs)
          (*pOs) << buffer << std::flush;
      }
      else {
        if (pSo)
          pSo->feedRawDataChunk( "\n" );   // in case the last chunk had none
        done = true;
      }
    }
  }

  void TimeOut(HANDLE hProcess, bool* doneStdout, bool* doneStderr, int timeout, std::timed_mutex* mtx);

#endif

  template<class S2O>
  class Process {
  protected:
    std::vector<std::string> _fzncmd;
    S2O* pS2Out;
    int timelimit;
    bool sigint;
  public:
    Process(std::vector<std::string>& fzncmd, S2O* pso, int tl, bool si)
      : _fzncmd(fzncmd), pS2Out(pso), timelimit(tl), sigint(si) {
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

      char* cmdstr = strdup(cmdline.str().c_str());

      BOOL processStarted = CreateProcess(NULL,
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

      // Stop ReadFile from blocking
      CloseHandle(g_hChildStd_OUT_Wr);
      CloseHandle(g_hChildStd_ERR_Wr);
      // Just close the child's in pipe here
      CloseHandle(g_hChildStd_IN_Rd);
      bool doneStdout = false;
      bool doneStderr = false;
      // Threaded solution seems simpler than asyncronous pipe reading
      std::mutex pipeMutex;
      std::timed_mutex terminateMutex;
      terminateMutex.lock();
      thread thrStdout(&ReadPipePrint<S2O>, g_hChildStd_OUT_Rd, &doneStdout, nullptr, pS2Out, &pipeMutex);
      thread thrStderr(&ReadPipePrint<S2O>, g_hChildStd_ERR_Rd, &doneStderr, pS2Out->getLog(), nullptr, &pipeMutex);
      thread thrTimeout(TimeOut, piProcInfo.hProcess, &doneStdout, &doneStderr, timelimit, &terminateMutex);
      thrStdout.join();
      thrStderr.join();
      terminateMutex.unlock();
      thrTimeout.join();
      CloseHandle(piProcInfo.hProcess);

      // Hard timeout: GenerateConsoleCtrlEvent()

      return result.str();
    }
#else
      int pipes[3][2];
      pipe(pipes[0]);
      pipe(pipes[1]);
      pipe(pipes[2]);

      // Make sure to reap child processes to avoid creating zombies
      signal(SIGCHLD, SIG_IGN);
    
      if (int childPID = fork()) {
        close(pipes[0][0]);
        close(pipes[1][1]);
        close(pipes[2][1]);
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
                  pS2Out->getLog() << buffer << std::flush;
                }
              }
              else if ( 1==i ) {
                pS2Out->feedRawDataChunk("\n");   // in case last chunk did not end with \n
                done = true;
              }
            }
          }
        }

        close(pipes[1][0]);
        close(pipes[2][0]);
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
            std::string sBuf;
            iss >> std::skipws >> sBuf;
            if ( sBuf.size() )
              cmd_line.push_back( strdup(sBuf.c_str()) );
            else
              break;
          }
        }

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
#endif

