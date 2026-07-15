/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solver.hh>

#ifdef _WIN32
#include <Windows.h>
#include <tchar.h>
#undef ERROR
#else
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#endif
#include <condition_variable>
#include <csignal>
#include <deque>
#include <mutex>
#include <string>
#include <sys/types.h>
#include <thread>
#include <vector>

namespace MiniZinc {

#ifdef _WIN32

template <class S2O>
void ReadPipePrint(HANDLE g_hCh, bool* _done, std::ostream* pOs,
                   std::deque<std::string>* outputQueue, std::mutex* mtx, std::mutex* cv_mutex,
                   std::condition_variable* cv) {
  bool& done = *_done;
  assert(pOs != 0 || outputQueue != 0);
  while (!done) {
    char buffer[5255];
    char nl_buffer[5255];
    DWORD count = 0;
    BOOL bSuccess = ReadFile(g_hCh, buffer, sizeof(buffer) - 1, &count, NULL);
    if (bSuccess && count > 0) {
      int nl_count = 0;
      for (DWORD i = 0; i < count; i++) {
        if (buffer[i] != 13) {
          nl_buffer[nl_count++] = buffer[i];
        }
      }
      nl_buffer[nl_count] = 0;
      std::lock_guard<std::mutex> lck(*mtx);
      if (outputQueue) {
        std::unique_lock<std::mutex> lk(*cv_mutex);
        bool wasEmpty = outputQueue->empty();
        outputQueue->push_back(nl_buffer);
        lk.unlock();
        if (wasEmpty) {
          cv->notify_one();
        }
      }
      if (pOs) (*pOs) << nl_buffer << std::flush;
    } else {
      if (outputQueue) {
        std::unique_lock<std::mutex> lk(*cv_mutex);
        bool wasEmpty = outputQueue->empty();
        outputQueue->push_back("\n");
        done = true;
        lk.unlock();
        if (wasEmpty) {
          cv->notify_one();
        }
      } else {
        done = true;
      }
    }
  }
}
#endif

template <class S2O>
class Process {
protected:
  std::vector<std::string> _fzncmd;
  S2O* _pS2Out;
  int _timelimit;
  bool _sigint;
  int _cleanupTime;
  /// Interactive solver: share the parent's terminal stdin with the child and
  /// let it own Ctrl-C, instead of isolating it in a pipe + own process group.
  bool _interactive;
#ifdef _WIN32
  static BOOL WINAPI handleInterrupt(DWORD fdwCtrlType) {
    switch (fdwCtrlType) {
      case CTRL_C_EVENT: {
        std::unique_lock<std::mutex> lck(_interruptMutex);
        hadInterrupt = true;
        _interruptCondition.notify_all();
        return TRUE;
      }
      default:
        return FALSE;
    }
  }
  /// Interactive solver: swallow Ctrl-C so MiniZinc does not tear down the
  /// session, while the child (which shares the console) still receives and
  /// handles it itself.
  static BOOL WINAPI ignoreInterrupt(DWORD fdwCtrlType) {
    return fdwCtrlType == CTRL_C_EVENT ? TRUE : FALSE;
  }
  static std::mutex _interruptMutex;
  static std::condition_variable _interruptCondition;
#else
  static void handleInterrupt(int signal) {
    if (signal == SIGINT) {
      hadInterrupt = true;
    } else {
      hadTerm = true;
    }
  }
  static bool hadTerm;
#endif
  static bool hadInterrupt;

public:
  Process(std::vector<std::string>& fzncmd, S2O* pso, int tl, bool si, int cleanupTime = 1000,
          bool interactive = false)
      : _fzncmd(fzncmd),
        _pS2Out(pso),
        _timelimit(tl),
        _sigint(si),
        _cleanupTime(cleanupTime),
        _interactive(interactive) {
    assert(nullptr != _pS2Out);
  }
  int run() {
#ifdef _WIN32
    // Interactive mode lets the solver own Ctrl-C; MiniZinc only swallows it so
    // it does not tear down the session.
    PHANDLER_ROUTINE ctrlHandler =
        _interactive ? &Process::ignoreInterrupt : &Process::handleInterrupt;
    SetConsoleCtrlHandler(ctrlHandler, TRUE);

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
    STARTUPINFOW siStartInfo;
    BOOL bSuccess = FALSE;

    // Set up members of the PROCESS_INFORMATION structure.
    ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

    // Set up members of the STARTUPINFO structure.
    // This structure specifies the STDIN and STDOUT handles for redirection.
    ZeroMemory(&siStartInfo, sizeof(STARTUPINFOW));
    siStartInfo.cb = sizeof(STARTUPINFOW);
    siStartInfo.hStdError = g_hChildStd_ERR_Wr;
    siStartInfo.hStdOutput = g_hChildStd_OUT_Wr;
    // Interactive: let the child read the real console directly so the user can
    // type commands; otherwise feed it through the (immediately closed) pipe.
    siStartInfo.hStdInput = _interactive ? GetStdHandle(STD_INPUT_HANDLE) : g_hChildStd_IN_Rd;
    siStartInfo.dwFlags |= STARTF_USESTDHANDLES;

    std::string cmdline = FileUtils::combine_cmd_line(_fzncmd);
    wchar_t* cmdstr = _wcsdup(FileUtils::utf8_to_wide(cmdline).c_str());

    HANDLE hJobObject = CreateJobObject(NULL, NULL);

    SetDllDirectoryW(FileUtils::utf8_to_wide(FileUtils::progpath()).c_str());
    BOOL processStarted = CreateProcessW(NULL,
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
      ssm << "Error occurred when executing FZN solver with command \""
          << FileUtils::wide_to_utf8(cmdstr) << "\".";
      throw Error(ssm.str());
    }

    BOOL assignedToJob = AssignProcessToJobObject(hJobObject, piProcInfo.hProcess);
    if (!assignedToJob) {
      throw Error("Failed to assign process to job.");
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
    bool timedOut = false;

    // Threaded solution seems simpler than asyncronous pipe reading
    std::mutex pipeMutex;

    std::mutex cv_mutex;
    std::condition_variable cv;

    std::deque<std::string> outputQueue;
    thread thrStdout(&ReadPipePrint<S2O>, g_hChildStd_OUT_Rd, &doneStdout, nullptr, &outputQueue,
                     &pipeMutex, &cv_mutex, &cv);
    thread thrStderr(&ReadPipePrint<S2O>, g_hChildStd_ERR_Rd, &doneStderr, &_pS2Out->getLog(),
                     nullptr, &pipeMutex, nullptr, nullptr);
    thread thrTimeout([&] {
      auto shouldStop = [&] { return hadInterrupt || (doneStderr && doneStdout); };
      std::unique_lock<std::mutex> lck(_interruptMutex);
      if (_interactive) {
        // The solver drives its own session and owns Ctrl-C; wait for it to
        // finish on its own without enforcing a time limit or terminating it.
        _interruptCondition.wait(lck, [&] { return doneStderr && doneStdout; });
        return;
      }
      if (_timelimit != 0) {
        if (!_interruptCondition.wait_for(lck, std::chrono::milliseconds(_timelimit), shouldStop)) {
          // If we timed out, generate an interrupt but ignore it ourselves
          timedOut = true;
          bool oldHadInterrupt = hadInterrupt;
          GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
          _interruptCondition.wait(lck, [&] { return hadInterrupt; });
          hadInterrupt = oldHadInterrupt;
        }
      } else {
        _interruptCondition.wait(lck, shouldStop);
      }
      // At this point the child should be stopped/stopping
      if (!doneStderr || !doneStdout) {
        if (!_interruptCondition.wait_for(lck, std::chrono::milliseconds(_cleanupTime),
                                          [&] { return doneStderr && doneStdout; })) {
          // Force terminate the child after 1s
          TerminateJobObject(hJobObject, 0);
        };
      }
    });

    while (true) {
      std::unique_lock<std::mutex> lk(cv_mutex);
      cv.wait(lk, [&] { return !outputQueue.empty(); });
      while (!outputQueue.empty()) {
        try {
          _pS2Out->feedRawDataChunk(outputQueue.front().c_str());
          outputQueue.pop_front();
        } catch (...) {
          TerminateJobObject(hJobObject, 0);
          doneStdout = true;
          doneStderr = true;
          lk.unlock();
          thrStdout.join();
          thrStderr.join();
          {
            // Make sure thrTimeout terminates
            std::unique_lock<std::mutex> lck(_interruptMutex);
            _interruptCondition.notify_all();
          }
          thrTimeout.join();
          SetConsoleCtrlHandler(ctrlHandler, FALSE);
          std::rethrow_exception(std::current_exception());
        }
      }
      if (doneStdout) break;
    }

    thrStdout.join();
    thrStderr.join();
    {
      // Make sure thrTimeout terminates
      std::unique_lock<std::mutex> lck(_interruptMutex);
      _interruptCondition.notify_all();
    }
    thrTimeout.join();
    DWORD exitCode = 0;
    if (GetExitCodeProcess(piProcInfo.hProcess, &exitCode) == FALSE) {
      exitCode = 1;
    }
    CloseHandle(piProcInfo.hProcess);

    SetConsoleCtrlHandler(ctrlHandler, FALSE);
    SetDllDirectoryW(L"");
    if (hadInterrupt) {
      // Re-trigger signal if it was not caused by our own timeout
      throw SignalRaised(CTRL_C_EVENT);
    }
    return timedOut ? 0 : exitCode;
  }
#else
    int pipes[3][2];
    pipe(pipes[0]);
    pipe(pipes[1]);
    pipe(pipes[2]);

    if (int childPID = fork()) {
      close(pipes[0][0]);
      close(pipes[1][1]);
      close(pipes[2][1]);
      close(pipes[0][1]);

      if (_interactive) {
        // The child shares our terminal stdin and process group, so a terminal
        // Ctrl-C is delivered to it directly. Ignore SIGINT here so the solver
        // (not MiniZinc) owns the interrupt; then just pump the solver's stdout
        // through the marker filter and its stderr to the log until it exits.
        struct sigaction sa_ign;
        struct sigaction old_sa_int;
        sa_ign.sa_handler = SIG_IGN;
        sa_ign.sa_flags = 0;
        sigemptyset(&sa_ign.sa_mask);
        sigaction(SIGINT, &sa_ign, &old_sa_int);

        bool done = false;
        bool watchErr = true;  // stop watching stderr once it closes, to avoid a busy-wait
        while (!done) {
          fd_set ifdset;
          FD_ZERO(&ifdset);  // NOLINT(readability-isolate-declaration)
          FD_SET(pipes[1][0], &ifdset);
          if (watchErr) {
            FD_SET(pipes[2][0], &ifdset);
          }
          int sel = select(FD_SETSIZE, &ifdset, nullptr, nullptr, nullptr);
          if (sel == -1) {
            if (errno == EINTR) {
              continue;
            }
            sigaction(SIGINT, &old_sa_int, nullptr);
            throw Error(std::string("Error in communication with solver: ") + strerror(errno));
          }
          if (FD_ISSET(pipes[1][0], &ifdset)) {
            char buffer[1000];
            ssize_t count = read(pipes[1][0], buffer, sizeof(buffer) - 1);
            if (count > 0) {
              buffer[count] = 0;
              try {
                _pS2Out->feedRawDataChunk(buffer);
              } catch (...) {
                kill(childPID, SIGKILL);
                sigaction(SIGINT, &old_sa_int, nullptr);
                throw;
              }
            } else {
              _pS2Out->feedRawDataChunk("\n");  // flush any unterminated last line
              done = true;
            }
          }
          if (watchErr && FD_ISSET(pipes[2][0], &ifdset)) {
            char buffer[1000];
            ssize_t count = read(pipes[2][0], buffer, sizeof(buffer) - 1);
            if (count > 0) {
              buffer[count] = 0;
              _pS2Out->getLog() << buffer << std::flush;
            } else {
              watchErr = false;
            }
          }
        }

        close(pipes[1][0]);
        close(pipes[2][0]);
        int exitStatus = 1;
        int childStatus;
        if (waitpid(childPID, &childStatus, 0) > 0 && WIFEXITED(childStatus)) {
          exitStatus = WEXITSTATUS(childStatus);
        }
        sigaction(SIGINT, &old_sa_int, nullptr);
        return exitStatus;
      }

      fd_set fdset;
      FD_ZERO(&fdset);  // NOLINT(readability-isolate-declaration)

      struct timeval starttime;
      gettimeofday(&starttime, nullptr);

      struct timeval timeout_orig;
      timeout_orig.tv_sec = _timelimit / 1000;
      timeout_orig.tv_usec = (static_cast<suseconds_t>(_timelimit) % 1000) * 1000;
      struct timeval timeout = timeout_orig;

      hadInterrupt = false;
      hadTerm = false;
      struct sigaction sa;
      struct sigaction old_sa_int;
      struct sigaction old_sa_term;
      sa.sa_handler = &handleInterrupt;
      sa.sa_flags = 0;
      sigfillset(&sa.sa_mask);
      sigaction(SIGINT, &sa, &old_sa_int);
      sigaction(SIGTERM, &sa, &old_sa_term);
      int signal = _sigint ? SIGINT : SIGTERM;
      bool handledInterrupt = false;
      bool handledTerm = false;

      bool done = hadTerm || hadInterrupt;
      bool timed_out = false;
      while (!done) {
        FD_SET(pipes[1][0], &fdset);
        FD_SET(pipes[2][0], &fdset);
        int sel =
            select(FD_SETSIZE, &fdset, nullptr, nullptr, _timelimit == 0 ? nullptr : &timeout);
        if (sel == -1) {
          if (errno != EINTR) {
            // some error has happened
            throw Error(std::string("Error in communication with solver: ") + strerror(errno));
          }
        }
        bool timeoutImmediately = false;
        if (hadInterrupt && !handledInterrupt) {
          signal = SIGINT;
          handledInterrupt = true;
          timeoutImmediately = true;
        }
        if (hadTerm && !handledTerm) {
          signal = SIGTERM;
          handledTerm = true;
          timeoutImmediately = true;
        }
        if (timeoutImmediately) {
          // Set timeout to immediately expire
          _timelimit = -1;
          timeout.tv_sec = 0;
          timeout.tv_usec = 0;
          timeout_orig = timeout;
          timeval currentTime;
          gettimeofday(&currentTime, nullptr);
          starttime = currentTime;
        }

        bool killed = false;
        if (_timelimit != 0) {
          timeval currentTime;
          gettimeofday(&currentTime, nullptr);
          if (sel != 0) {
            timeval elapsed;
            elapsed.tv_sec = currentTime.tv_sec - starttime.tv_sec;
            elapsed.tv_usec = currentTime.tv_usec - starttime.tv_usec;
            if (elapsed.tv_usec < 0) {
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
          if (timeout.tv_sec < 0 || (timeout.tv_sec == 0 && timeout.tv_usec == 0)) {
            timed_out = true;
            if (signal == SIGKILL) {
              killed = true;
              done = true;
            }
            if (killpg(childPID, signal) == -1) {
              // Fallback to killing the child if killing the process group fails
              kill(childPID, signal);
            }
            timeout.tv_sec = _cleanupTime / 1000;
            timeout.tv_usec = (static_cast<suseconds_t>(_cleanupTime) % 1000) * 1000;
            timeout_orig = timeout;
            starttime = currentTime;
            // Upgrade signal for next attempt
            signal = signal == SIGINT ? SIGTERM : SIGKILL;
          }
        }

        bool addedNl = false;
        for (int i = 1; i <= 2; ++i) {
          if (FD_ISSET(pipes[i][0], &fdset)) {
            char buffer[1000];
            size_t count = read(pipes[i][0], buffer, sizeof(buffer) - 1);
            if (count > 0) {
              buffer[count] = 0;
              if (1 == i) {
                //                       cerr << "mzn-fzn: raw chunk stdout:::  " << flush;
                //                       cerr << buffer << flush;
                try {
                  _pS2Out->feedRawDataChunk(buffer);
                } catch (...) {
                  // Exception during solns2out, kill process and re-throw
                  if (killpg(childPID, SIGKILL) == -1) {
                    // Fallback to killing the child if killing the process group fails
                    kill(childPID, SIGKILL);
                  }
                  throw;
                }
              } else {
                _pS2Out->getLog() << buffer << std::flush;
              }
            } else if (1 == i) {
              _pS2Out->feedRawDataChunk("\n");  // in case last chunk did not end with \n
              addedNl = true;
              done = true;
            }
          }
        }
        if (killed && !addedNl) {
          _pS2Out->feedRawDataChunk("\n");  // in case last chunk did not end with \n
        }
      }

      close(pipes[1][0]);
      close(pipes[2][0]);
      int exitStatus = timed_out ? 0 : 1;
      int childStatus;
      int pidStatus = waitpid(childPID, &childStatus, 0);
      if (!timed_out && pidStatus > 0) {
        if (WIFEXITED(childStatus)) {
          exitStatus = WEXITSTATUS(childStatus);
        }
      }
      sigaction(SIGINT, &old_sa_int, nullptr);
      sigaction(SIGTERM, &old_sa_term, nullptr);
      if (hadInterrupt) {
        throw SignalRaised(SIGINT);
      }
      if (hadTerm) {
        throw SignalRaised(SIGTERM);
      }
      return exitStatus;
    }
    if (!_interactive) {
      // Isolate the child in its own process group so the parent can signal the
      // whole group on time-out. In interactive mode the child must stay in the
      // terminal's foreground group, otherwise reading the tty raises SIGTTIN.
      if (setpgid(0, 0) == -1) {
        throw Error("Failed to set pgid of subprocess");
      }
    }
    close(STDOUT_FILENO);
    close(STDERR_FILENO);
    if (!_interactive) {
      // Batch mode: the child's stdin is the (already write-closed) pipe, so it
      // sees EOF. Interactive mode leaves STDIN_FILENO inherited from the parent
      // so the solver reads the user's terminal directly.
      close(STDIN_FILENO);
      dup2(pipes[0][0], STDIN_FILENO);
    }
    dup2(pipes[1][1], STDOUT_FILENO);
    dup2(pipes[2][1], STDERR_FILENO);
    close(pipes[0][0]);
    close(pipes[0][1]);
    close(pipes[1][1]);
    close(pipes[1][0]);
    close(pipes[2][1]);
    close(pipes[2][0]);

    std::vector<char*> cmd_line;
    for (auto& iCmdl : _fzncmd) {
      cmd_line.push_back(strdup(iCmdl.c_str()));
    }

    char** argv = new char*[cmd_line.size() + 1];
    for (unsigned int i = 0; i < cmd_line.size(); i++) {
      argv[i] = cmd_line[i];
    }
    argv[cmd_line.size()] = nullptr;

    int status = execvp(argv[0], argv);  // execvp only returns if an error occurs.
    assert(status == -1);                // the returned value will always be -1
    std::stringstream ssm;
    ssm << "Error occurred when executing FZN solver with command \"";
    for (auto& s : cmd_line) {
      ssm << s << ' ';
    }
    ssm << "\".";
    throw Error(ssm.str());
  }
#endif
};

template <class S2O>
bool Process<S2O>::hadInterrupt;
#ifdef _WIN32
template <class S2O>
std::mutex Process<S2O>::_interruptMutex;
template <class S2O>
std::condition_variable Process<S2O>::_interruptCondition;
#else
template <class S2O>
bool Process<S2O>::hadTerm;
#endif

}  // namespace MiniZinc
