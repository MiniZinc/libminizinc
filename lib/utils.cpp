/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/flatten_internal.hh>
#include <minizinc/utils.hh>

#if _WIN32
#include <Windows.h>
#include <io.h>
#else
#include <csignal>
#include <unistd.h>
#endif

namespace MiniZinc {

OverflowHandler::OverflowHandler() {}

std::unique_ptr<OverflowHandler::OverflowInfo> OverflowHandler::_ofi;

namespace {
void itoa_async_safe(unsigned int v, char* buf) {
  char* p = buf;
  do {
    int t = v;
    v /= 10;
    *p++ = "0123456789"[t - v * 10];
  } while (v != 0);
  *p = '\0';
  char* start = buf;
  char* end = p - 1;
  while (start < end) {
    char t = *end;
    *end-- = *start;
    *start++ = t;
  }
}
const char* basename_async_safe(const char* f) {
  const char* max = f - 1;
  for (const char* p = f; *p != '\0'; p++) {
    if (*p == '/' || *p == '\\') {
      max = p;
    }
  }
  return max + 1;
}

void dump_stack(const std::vector<EnvI::CallStackEntry>& stack) {
  char buf[24];
  const char* msg = "Stack depth: ";
  write(2, msg, strlen(msg));
  itoa_async_safe(stack.size(), buf);
  write(2, buf, strlen(buf));
  msg = "\nStack backtrace (only showing function/predicate calls):\n";
  write(2, msg, strlen(msg));

  int count = 15;
  for (unsigned int i = stack.size(); (i--) >= 0u;) {
    if (stack[i].e->isa<Call>()) {
      msg = "  frame #";
      write(2, msg, strlen(msg));
      itoa_async_safe(stack.size() - i - 1, buf);
      write(2, buf, strlen(buf));
      if (stack.size() - i - 1 < 10) {
        msg = ":  ";
      } else {
        msg = ": ";
      }
      write(2, msg, strlen(msg));
      msg = stack[i].e->cast<Call>()->id().c_str();
      write(2, msg, strlen(msg));
      msg = " at ";
      write(2, msg, strlen(msg));
      const Location& loc = stack[i].e->cast<Call>()->decl()->loc();
      msg = loc.filename().c_str();
      if (msg == nullptr) {
        msg = "unknown file";
      } else {
        msg = basename_async_safe(msg);
      }
      write(2, msg, strlen(msg));
      msg = ":";
      write(2, msg, strlen(msg));
      itoa_async_safe(loc.firstLine(), buf);
      write(2, buf, strlen(buf));
      msg = ".";
      write(2, msg, strlen(msg));
      itoa_async_safe(loc.firstColumn(), buf);
      write(2, buf, strlen(buf));
      msg = "\n";
      write(2, msg, strlen(msg));
      if (--count == 0) {
        break;
      }
    }
  }
  if (stack.size() > 15) {
    msg = "  ...\n";
    write(2, msg, strlen(msg));
  }
}

}  // namespace

#if defined(__x86_64__) && defined(__APPLE__)

struct OverflowHandler::OverflowInfo {
  unsigned long long stack_top;
  EnvI* env;
  OverflowInfo(const char** argv)
      : stack_top(reinterpret_cast<unsigned long long>(*argv)), env(nullptr) {}
  static void overflow(int sig, siginfo_t* info, void* context);
};

void OverflowHandler::OverflowInfo::overflow(int sig, siginfo_t* info, void* context) {
  unsigned long long segv_addr = reinterpret_cast<unsigned long long>(info->si_addr);
  unsigned long long ctx_sp = ((ucontext_t*)context)->uc_mcontext->__ss.__rsp;
  if (segv_addr < _ofi->stack_top && segv_addr >= ctx_sp - 1024) {
    const char* msg =
        "MiniZinc error: Memory violation detected (segmentation fault).\n"
        "This is most likely due to a stack overflow.\n"
        "Check for deep (or infinite) recursion in the model.\n";
    write(2, msg, strlen(msg));
    if (_ofi->env != nullptr) {
      dump_stack(_ofi->env->callStack);
    }
    _exit(1);
  } else {
    const char* msg = "MiniZinc error: Memory violation detected (segmentation fault).\n";
    write(2, msg, strlen(msg));
    msg = "This is a bug. Please file a bug report using the MiniZinc bug tracker.\n";
    write(2, msg, strlen(msg));
    abort();
  }
}

void OverflowHandler::install(const char** argv) {
  _ofi = std::unique_ptr<OverflowInfo>(new OverflowInfo(argv));
  stack_t stk;
  stk.ss_sp = malloc(SIGSTKSZ);
  if (stk.ss_sp != nullptr) {
    stk.ss_size = SIGSTKSZ;
    stk.ss_flags = 0;
    if (sigaltstack(&stk, nullptr) >= 0) {
      struct sigaction act;
      act.sa_sigaction = OverflowInfo::overflow;
      act.sa_flags = SA_SIGINFO | SA_64REGSET | SA_ONSTACK | SA_NODEFER;
      sigemptyset(&act.sa_mask);
      sigaction(SIGSEGV, &act, NULL);
      return;
    }
  }
  std::cerr << "Cannot initialise stack overflow handler.\n";
  std::exit(1);
}

void OverflowHandler::setEnv(Env& env) { _ofi->env = &env.envi(); }
void OverflowHandler::removeEnv() { _ofi->env = nullptr; }

#elif _WIN32

struct OverflowHandler::OverflowInfo {
  EnvI* env;
};

void OverflowHandler::install() { _ofi = std::unique_ptr<OverflowInfo>(new OverflowInfo()); }

void OverflowHandler::setEnv(Env& env) { _ofi->env = &env.envi(); }
void OverflowHandler::removeEnv() { _ofi->env = nullptr; }

int OverflowHandler::filter(unsigned int code) {
  // To be used as SEH exception filter on Windows
  switch (code) {
    case EXCEPTION_STACK_OVERFLOW:
    case EXCEPTION_ACCESS_VIOLATION:
      return EXCEPTION_EXECUTE_HANDLER;
    default:
      return EXCEPTION_CONTINUE_SEARCH;
  }
}

void OverflowHandler::handle(unsigned int code) {
  // To be used as SEH exception handler on Windows
  switch (code) {
    case EXCEPTION_STACK_OVERFLOW: {
      std::cerr << "MiniZinc error: Memory violation detected (stack overflow).\n"
                << "Check for deep (or infinite) recursion in the model.\n";
      if (_ofi->env != nullptr) {
        dump_stack(_ofi->env->callStack);
      }
      _exit(1);
      break;
    }
    case EXCEPTION_ACCESS_VIOLATION: {
      std::cerr << "MiniZinc error: Memory violation detected (segmentation fault).\n"
                << "This is a bug. Please file a bug report using the MiniZinc bug tracker.\n";
      abort();
      break;
    }
    default:
      assert(false);
  }
}
#else

struct OverflowHandler::OverflowInfo {};

void OverflowHandler::setEnv(Env& env) {}
void OverflowHandler::removeEnv() {}

void OverflowHandler::install(const char** argv) {}

#endif

}  // namespace MiniZinc
