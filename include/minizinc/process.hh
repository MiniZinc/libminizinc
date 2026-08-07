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
#elif !defined(__EMSCRIPTEN__)
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <spawn.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef __APPLE__
#include <crt_externs.h>
#endif
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

#if !defined(_WIN32) && !defined(__EMSCRIPTEN__)
namespace ProcessInternal {

#ifdef __APPLE__
// macOS does not declare environ in a shared library; use the accessor instead.
#define MZN_ENVIRON (*_NSGetEnviron())
#else
// environ is declared by <unistd.h> on other platforms (glibc, musl, BSD).
#define MZN_ENVIRON environ
#endif

/// Set FD_CLOEXEC on \a fd. Returns 0 on success, -1 on error (errno set).
inline int set_cloexec(int fd) {
  int flags = fcntl(fd, F_GETFD);
  if (flags == -1) {
    return -1;
  }
  return fcntl(fd, F_SETFD, flags | FD_CLOEXEC);
}

/// Create a pipe whose ends are both close-on-exec, so they never leak into the
/// solver process (or any other subprocess spawned concurrently).
inline int make_cloexec_pipe(int fds[2]) {
#ifdef __linux__
  if (pipe2(fds, O_CLOEXEC) == 0) {
    return 0;
  }
  if (errno != ENOSYS) {
    return -1;
  }
#endif
  if (pipe(fds) != 0) {
    return -1;
  }
  if ((set_cloexec(fds[0]) != 0) || (set_cloexec(fds[1]) != 0)) {
    int e = errno;
    ::close(fds[0]);
    ::close(fds[1]);
    errno = e;
    return -1;
  }
  return 0;
}

/// RAII owner of a file descriptor.
class FileDescriptor {
private:
  int _fd;

public:
  explicit FileDescriptor(int fd = -1) : _fd(fd) {}
  FileDescriptor(const FileDescriptor&) = delete;
  FileDescriptor& operator=(const FileDescriptor&) = delete;
  ~FileDescriptor() { reset(); }
  int get() const { return _fd; }
  int release() {
    int fd = _fd;
    _fd = -1;
    return fd;
  }
  void reset(int fd = -1) {
    if (_fd != -1) {
      ::close(_fd);
    }
    _fd = fd;
  }
};

/// Move \a fd above the standard descriptors (0,1,2) so that, when the standard
/// descriptors are set up as posix_spawn dup2 targets, a source descriptor
/// cannot be clobbered. The duplicate is close-on-exec. Returns -1 on error.
inline int move_from_standard_fd(FileDescriptor& fd) {
  if (fd.get() > STDERR_FILENO) {
    return fd.get();
  }
  int nfd;
#ifdef F_DUPFD_CLOEXEC
  nfd = fcntl(fd.get(), F_DUPFD_CLOEXEC, STDERR_FILENO + 1);
#else
  nfd = fcntl(fd.get(), F_DUPFD, STDERR_FILENO + 1);
  if ((nfd != -1) && (set_cloexec(nfd) != 0)) {
    int e = errno;
    ::close(nfd);
    errno = e;
    nfd = -1;
  }
#endif
  if (nfd == -1) {
    return -1;
  }
  fd.reset(nfd);
  return nfd;
}

/// RAII wrapper for posix_spawn file actions.
class SpawnFileActions {
private:
  posix_spawn_file_actions_t _actions;
  bool _init;

public:
  SpawnFileActions() : _init(false) {}
  SpawnFileActions(const SpawnFileActions&) = delete;
  SpawnFileActions& operator=(const SpawnFileActions&) = delete;
  ~SpawnFileActions() {
    if (_init) {
      posix_spawn_file_actions_destroy(&_actions);
    }
  }
  int init() {
    int err = posix_spawn_file_actions_init(&_actions);
    _init = (err == 0);
    return err;
  }
  posix_spawn_file_actions_t* get() { return &_actions; }
};

/// RAII wrapper for posix_spawn attributes.
class SpawnAttributes {
private:
  posix_spawnattr_t _attr;
  bool _init;

public:
  SpawnAttributes() : _init(false) {}
  SpawnAttributes(const SpawnAttributes&) = delete;
  SpawnAttributes& operator=(const SpawnAttributes&) = delete;
  ~SpawnAttributes() {
    if (_init) {
      posix_spawnattr_destroy(&_attr);
    }
  }
  int init() {
    int err = posix_spawnattr_init(&_attr);
    _init = (err == 0);
    return err;
  }
  posix_spawnattr_t* get() { return &_attr; }
};

/// The child pid plus the parent's read ends of the child's stdout/stderr.
struct SpawnResult {
  pid_t pid;
  int stdoutFd;
  int stderrFd;
};

/// Spawn \a cmd via posix_spawnp with hardened descriptor handling.
///
/// Non-interactive: the child is placed in its own process group (so the parent
/// can signal the whole group on time-out) and its stdin is an already
/// write-closed pipe, so it sees EOF. Interactive: the child inherits the
/// parent's terminal stdin and stays in the foreground process group. Throws
/// Error on any failure.
inline SpawnResult spawn_solver(const std::vector<std::string>& cmd, bool interactive) {
  if (cmd.empty()) {
    throw Error("Cannot execute an empty solver command");
  }

  FileDescriptor stdinRead;
  FileDescriptor stdinWrite;
  FileDescriptor stdoutRead;
  FileDescriptor stdoutWrite;
  FileDescriptor stderrRead;
  FileDescriptor stderrWrite;

  int fds[2];
  if (!interactive) {
    if (make_cloexec_pipe(fds) != 0) {
      throw Error(std::string("Failed to create solver stdin pipe: ") + strerror(errno));
    }
    stdinRead.reset(fds[0]);
    stdinWrite.reset(fds[1]);
  }
  if (make_cloexec_pipe(fds) != 0) {
    throw Error(std::string("Failed to create solver stdout pipe: ") + strerror(errno));
  }
  stdoutRead.reset(fds[0]);
  stdoutWrite.reset(fds[1]);
  if (make_cloexec_pipe(fds) != 0) {
    throw Error(std::string("Failed to create solver stderr pipe: ") + strerror(errno));
  }
  stderrRead.reset(fds[0]);
  stderrWrite.reset(fds[1]);

  // Keep the descriptors the child receives out of the 0/1/2 range so the dup2
  // file actions below cannot clobber a source descriptor.
  FileDescriptor* childFds[] = {&stdinRead, &stdoutWrite, &stderrWrite};
  for (FileDescriptor* fd : childFds) {
    if ((fd->get() != -1) && (move_from_standard_fd(*fd) == -1)) {
      throw Error(std::string("Failed to prepare solver descriptors: ") + strerror(errno));
    }
  }

  SpawnFileActions actions;
  int err = actions.init();
  if (err != 0) {
    errno = err;
    throw Error(std::string("Failed to initialise solver spawn file actions: ") + strerror(errno));
  }
  SpawnAttributes attr;
  err = attr.init();
  if (err != 0) {
    errno = err;
    throw Error(std::string("Failed to initialise solver spawn attributes: ") + strerror(errno));
  }

  if (!interactive) {
    // Isolate the child in its own process group so the parent can signal the
    // whole group on time-out. Interactive mode leaves the child in the
    // terminal's foreground group, otherwise reading the tty raises SIGTTIN.
    err = posix_spawnattr_setpgroup(attr.get(), 0);
    if (err == 0) {
      err = posix_spawnattr_setflags(attr.get(), POSIX_SPAWN_SETPGROUP);
    }
    if (err == 0) {
      err = posix_spawn_file_actions_adddup2(actions.get(), stdinRead.get(), STDIN_FILENO);
    }
  }
  if (err == 0) {
    err = posix_spawn_file_actions_adddup2(actions.get(), stdoutWrite.get(), STDOUT_FILENO);
  }
  if (err == 0) {
    err = posix_spawn_file_actions_adddup2(actions.get(), stderrWrite.get(), STDERR_FILENO);
  }
  if (err != 0) {
    errno = err;
    throw Error(std::string("Failed to configure solver spawn: ") + strerror(errno));
  }

  std::vector<char*> argv;
  argv.reserve(cmd.size() + 1);
  for (const std::string& a : cmd) {
    argv.push_back(const_cast<char*>(a.c_str()));
  }
  argv.push_back(nullptr);

  pid_t pid = -1;
  err = posix_spawnp(&pid, argv[0], actions.get(), attr.get(), argv.data(), MZN_ENVIRON);
  if (err != 0) {
    errno = err;
    std::stringstream ssm;
    ssm << "Error occurred when executing FZN solver with command \"";
    for (const std::string& a : cmd) {
      ssm << a << ' ';
    }
    ssm << "\" (" << strerror(errno) << ").";
    throw Error(ssm.str());
  }

  SpawnResult result;
  result.pid = pid;
  result.stdoutFd = stdoutRead.release();
  result.stderrFd = stderrRead.release();
  // The remaining descriptors (stdin read/write and the child's stdout/stderr
  // write ends) are closed here by RAII: the child holds its own dup2'd copies,
  // and closing the parent's stdin write end gives the child EOF on stdin.
  return result;
}

}  // namespace ProcessInternal
#endif

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

namespace ProcessInternal {

inline std::string win_error(const std::string& prefix) {
  return prefix + " (Windows error " + std::to_string(GetLastError()) + ")";
}

/// RAII owner of a Windows HANDLE.
class WindowsHandle {
private:
  HANDLE _handle;

public:
  explicit WindowsHandle(HANDLE h = NULL) : _handle(h) {}
  WindowsHandle(const WindowsHandle&) = delete;
  WindowsHandle& operator=(const WindowsHandle&) = delete;
  ~WindowsHandle() { reset(); }
  HANDLE get() const { return _handle; }
  HANDLE* put() {
    reset();
    return &_handle;
  }
  HANDLE release() {
    HANDLE h = _handle;
    _handle = NULL;
    return h;
  }
  bool valid() const { return (_handle != NULL) && (_handle != INVALID_HANDLE_VALUE); }
  void reset(HANDLE h = NULL) {
    if (valid()) {
      CloseHandle(_handle);
    }
    _handle = h;
  }
};

/// RAII wrapper for a process/thread attribute list carrying the explicit
/// handle-inheritance list, so the solver inherits only the stdio handles
/// instead of every inheritable handle in the process.
class WindowsAttributeList {
private:
  std::vector<char> _buffer;
  LPPROC_THREAD_ATTRIBUTE_LIST _list;
  bool _init;

public:
  WindowsAttributeList() : _list(NULL), _init(false) {}
  WindowsAttributeList(const WindowsAttributeList&) = delete;
  WindowsAttributeList& operator=(const WindowsAttributeList&) = delete;
  ~WindowsAttributeList() {
    if (_init) {
      DeleteProcThreadAttributeList(_list);
    }
  }
  void init() {
    SIZE_T size = 0;
    InitializeProcThreadAttributeList(NULL, 1, 0, &size);
    if (size == 0) {
      throw Error(win_error("Failed to size solver process attribute list"));
    }
    _buffer.resize(size);
    _list = reinterpret_cast<LPPROC_THREAD_ATTRIBUTE_LIST>(_buffer.data());
    if (!InitializeProcThreadAttributeList(_list, 1, 0, &size)) {
      throw Error(win_error("Failed to initialise solver process attribute list"));
    }
    _init = true;
  }
  void set_inherited_handles(HANDLE* handles, DWORD count) {
    if (!UpdateProcThreadAttribute(_list, 0, PROC_THREAD_ATTRIBUTE_HANDLE_LIST, handles,
                                   sizeof(HANDLE) * count, NULL, NULL)) {
      throw Error(win_error("Failed to set solver inherited handle list"));
    }
  }
  LPPROC_THREAD_ATTRIBUTE_LIST get() const { return _list; }
};

/// A running solver: its kill-on-close job object, process handle, and the
/// parent's read ends of the child's stdout/stderr.
struct WinSpawnResult {
  HANDLE job;
  HANDLE process;
  HANDLE stdoutRead;
  HANDLE stderrRead;
};

/// Spawn \a cmd with hardened Windows handle and job-object management.
///
/// The solver inherits only the three stdio handles (explicit handle list), is
/// created suspended and assigned to a kill-on-close job object before being
/// resumed (closing the assignment race), and — in batch mode — reads an
/// immediately EOF'd stdin pipe. Interactive mode instead hands it an
/// inheritable duplicate of the real console stdin. Throws Error on failure;
/// all handles are released by RAII on any error path.
inline WinSpawnResult spawn_solver_windows(const std::vector<std::string>& cmd, bool interactive) {
  SECURITY_ATTRIBUTES saAttr;
  saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
  saAttr.bInheritHandle = TRUE;
  saAttr.lpSecurityDescriptor = NULL;

  WindowsHandle stdinRead;
  WindowsHandle stdinWrite;
  WindowsHandle stdoutRead;
  WindowsHandle stdoutWrite;
  WindowsHandle stderrRead;
  WindowsHandle stderrWrite;
  WindowsHandle consoleStdin;  // interactive: inheritable dup of the console stdin

  if (!CreatePipe(stdoutRead.put(), stdoutWrite.put(), &saAttr, 0)) {
    throw Error(win_error("Stdout CreatePipe failed"));
  }
  if (!SetHandleInformation(stdoutRead.get(), HANDLE_FLAG_INHERIT, 0)) {
    throw Error(win_error("Stdout SetHandleInformation failed"));
  }
  if (!CreatePipe(stderrRead.put(), stderrWrite.put(), &saAttr, 0)) {
    throw Error(win_error("Stderr CreatePipe failed"));
  }
  if (!SetHandleInformation(stderrRead.get(), HANDLE_FLAG_INHERIT, 0)) {
    throw Error(win_error("Stderr SetHandleInformation failed"));
  }

  HANDLE childStdin;
  if (interactive) {
    HANDLE parentStdin = GetStdHandle(STD_INPUT_HANDLE);
    if (!DuplicateHandle(GetCurrentProcess(), parentStdin, GetCurrentProcess(), consoleStdin.put(),
                         0, TRUE, DUPLICATE_SAME_ACCESS)) {
      throw Error(win_error("stdin DuplicateHandle failed"));
    }
    childStdin = consoleStdin.get();
  } else {
    if (!CreatePipe(stdinRead.put(), stdinWrite.put(), &saAttr, 0)) {
      throw Error(win_error("Stdin CreatePipe failed"));
    }
    if (!SetHandleInformation(stdinWrite.get(), HANDLE_FLAG_INHERIT, 0)) {
      throw Error(win_error("Stdin SetHandleInformation failed"));
    }
    childStdin = stdinRead.get();
  }

  WindowsAttributeList attrList;
  attrList.init();
  HANDLE inheritHandles[3] = {childStdin, stdoutWrite.get(), stderrWrite.get()};
  attrList.set_inherited_handles(inheritHandles, 3);

  STARTUPINFOEXW siStartInfo;
  ZeroMemory(&siStartInfo, sizeof(STARTUPINFOEXW));
  siStartInfo.StartupInfo.cb = sizeof(STARTUPINFOEXW);
  siStartInfo.StartupInfo.hStdError = stderrWrite.get();
  siStartInfo.StartupInfo.hStdOutput = stdoutWrite.get();
  siStartInfo.StartupInfo.hStdInput = childStdin;
  siStartInfo.StartupInfo.dwFlags |= STARTF_USESTDHANDLES;
  siStartInfo.lpAttributeList = attrList.get();

  WindowsHandle job(CreateJobObjectW(NULL, NULL));
  if (!job.valid()) {
    throw Error(win_error("CreateJobObject failed"));
  }
  JOBOBJECT_EXTENDED_LIMIT_INFORMATION jobInfo;
  ZeroMemory(&jobInfo, sizeof(jobInfo));
  jobInfo.BasicLimitInformation.LimitFlags = JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE;
  if (!SetInformationJobObject(job.get(), JobObjectExtendedLimitInformation, &jobInfo,
                               sizeof(jobInfo))) {
    throw Error(win_error("SetInformationJobObject failed"));
  }

  std::string cmdline = FileUtils::combine_cmd_line(cmd);
  std::wstring cmdwide = FileUtils::utf8_to_wide(cmdline);
  std::vector<wchar_t> cmdbuf(cmdwide.begin(), cmdwide.end());
  cmdbuf.push_back(L'\0');

  PROCESS_INFORMATION piProcInfo;
  ZeroMemory(&piProcInfo, sizeof(PROCESS_INFORMATION));

  SetDllDirectoryW(FileUtils::utf8_to_wide(FileUtils::progpath()).c_str());
  BOOL processStarted = CreateProcessW(NULL, cmdbuf.data(), NULL, NULL, TRUE,
                                       EXTENDED_STARTUPINFO_PRESENT | CREATE_SUSPENDED, NULL, NULL,
                                       &siStartInfo.StartupInfo, &piProcInfo);
  DWORD createErr = GetLastError();
  SetDllDirectoryW(L"");
  if (!processStarted) {
    SetLastError(createErr);
    throw Error(win_error("Error occurred when executing FZN solver with command \"" +
                          FileUtils::wide_to_utf8(cmdwide) + "\""));
  }
  WindowsHandle process(piProcInfo.hProcess);
  WindowsHandle thread(piProcInfo.hThread);

  if (!AssignProcessToJobObject(job.get(), process.get())) {
    std::string message = win_error("Failed to assign solver process to job");
    TerminateProcess(process.get(), 1);
    WaitForSingleObject(process.get(), 5000);
    throw Error(message);
  }
  if (ResumeThread(thread.get()) == static_cast<DWORD>(-1)) {
    std::string message = win_error("Failed to resume solver process");
    TerminateJobObject(job.get(), 1);
    WaitForSingleObject(process.get(), 5000);
    throw Error(message);
  }

  // Release the child-side handles the parent no longer needs. Closing the
  // stdin write end (batch mode) gives the child EOF on stdin.
  stdoutWrite.reset();
  stderrWrite.reset();
  stdinRead.reset();
  stdinWrite.reset();
  consoleStdin.reset();

  WinSpawnResult result;
  result.job = job.release();
  result.process = process.release();
  result.stdoutRead = stdoutRead.release();
  result.stderrRead = stderrRead.release();
  return result;
}

}  // namespace ProcessInternal
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
#ifdef __EMSCRIPTEN__
    throw Error("Executable solver processes are not supported in WebAssembly");
#elif defined(_WIN32)
    // Interactive mode lets the solver own Ctrl-C; MiniZinc only swallows it so
    // it does not tear down the session.
    PHANDLER_ROUTINE ctrlHandler =
        _interactive ? &Process::ignoreInterrupt : &Process::handleInterrupt;
    SetConsoleCtrlHandler(ctrlHandler, TRUE);

    // Spawn the solver with explicit handle inheritance (only the three stdio
    // handles), a kill-on-close job object, and a suspended-then-resumed start
    // that closes the job-assignment race. See ProcessInternal::spawn_solver_windows.
    ProcessInternal::WinSpawnResult spawned =
        ProcessInternal::spawn_solver_windows(_fzncmd, _interactive);
    HANDLE hJobObject = spawned.job;
    HANDLE hProcess = spawned.process;
    HANDLE g_hChildStd_OUT_Rd = spawned.stdoutRead;
    HANDLE g_hChildStd_ERR_Rd = spawned.stderrRead;

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
          CloseHandle(hProcess);
          CloseHandle(hJobObject);
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
    if (GetExitCodeProcess(hProcess, &exitCode) == FALSE) {
      exitCode = 1;
    }
    CloseHandle(hProcess);
    // Closing the job (kill-on-close) guarantees any lingering grandchildren die.
    CloseHandle(hJobObject);

    SetConsoleCtrlHandler(ctrlHandler, FALSE);
    if (hadInterrupt) {
      // Re-trigger signal if it was not caused by our own timeout
      throw SignalRaised(CTRL_C_EVENT);
    }
    return timedOut ? 0 : exitCode;
#else
    // Spawn the solver with hardened descriptor handling (own process group,
    // close-on-exec pipes, no leaked descriptors). See ProcessInternal.
    ProcessInternal::SpawnResult spawned = ProcessInternal::spawn_solver(_fzncmd, _interactive);
    int childPID = spawned.pid;
    // Parent read ends of the child's stdout/stderr; indexed 1/2 to mirror the
    // original pipe layout the read loops below expect.
    int readFds[3] = {-1, spawned.stdoutFd, spawned.stderrFd};

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
        FD_SET(readFds[1], &ifdset);
        if (watchErr) {
          FD_SET(readFds[2], &ifdset);
        }
        int sel = select(FD_SETSIZE, &ifdset, nullptr, nullptr, nullptr);
        if (sel == -1) {
          if (errno == EINTR) {
            continue;
          }
          sigaction(SIGINT, &old_sa_int, nullptr);
          throw Error(std::string("Error in communication with solver: ") + strerror(errno));
        }
        if (FD_ISSET(readFds[1], &ifdset)) {
          char buffer[1000];
          ssize_t count = read(readFds[1], buffer, sizeof(buffer) - 1);
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
        if (watchErr && FD_ISSET(readFds[2], &ifdset)) {
          char buffer[1000];
          ssize_t count = read(readFds[2], buffer, sizeof(buffer) - 1);
          if (count > 0) {
            buffer[count] = 0;
            _pS2Out->getLog() << buffer << std::flush;
          } else {
            watchErr = false;
          }
        }
      }

      close(readFds[1]);
      close(readFds[2]);
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
      FD_SET(readFds[1], &fdset);
      FD_SET(readFds[2], &fdset);
      int sel = select(FD_SETSIZE, &fdset, nullptr, nullptr, _timelimit == 0 ? nullptr : &timeout);
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
      // Only inspect the descriptors when select reported them ready. On EINTR
      // (sel == -1) -- e.g. an interrupt broke the wait -- the fd_set is left
      // unspecified, and reading it would block on a solver that ignores the
      // signal and produces no output, preventing the kill escalation above from
      // ever running.
      if (sel > 0) {
        for (int i = 1; i <= 2; ++i) {
          if (FD_ISSET(readFds[i], &fdset)) {
            char buffer[1000];
            ssize_t count = read(readFds[i], buffer, sizeof(buffer) - 1);
            if (count > 0) {
              buffer[count] = 0;
              if (1 == i) {
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
      }
      if (killed && !addedNl) {
        _pS2Out->feedRawDataChunk("\n");  // in case last chunk did not end with \n
      }
    }

    close(readFds[1]);
    close(readFds[2]);
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
#endif
  }
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
