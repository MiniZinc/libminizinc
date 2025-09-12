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
#define mzn_write(fd, buf, s) _write((fd), (buf), static_cast<unsigned int>(s))
#else
#include <csignal>
#include <unistd.h>
#define mzn_write(fd, buf, s) write((fd), (buf), (s))
#endif

namespace MiniZinc {

OverflowHandler::OverflowHandler() {}

std::unique_ptr<OverflowHandler::OverflowInfo> OverflowHandler::_ofi;

namespace {
void itoa_async_safe(unsigned int v, char* buf) {
  char* p = buf;
  do {
    unsigned int t = v;
    v /= 10;
    *p++ = "0123456789"[t - v * 10];
  } while (v != 0U);
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
  mzn_write(2, msg, strlen(msg));
  itoa_async_safe(static_cast<unsigned int>(stack.size()), buf);
  mzn_write(2, buf, strlen(buf));
  msg = "\nStack backtrace (only showing function/predicate calls):\n";
  mzn_write(2, msg, strlen(msg));

  int count = 15;
  for (size_t i = stack.size(); (i--) >= 0U;) {
    if (Expression::isa<Call>(stack[i].e)) {
      msg = "  frame #";
      mzn_write(2, msg, strlen(msg));
      itoa_async_safe(static_cast<unsigned int>(stack.size() - i) - 1, buf);
      mzn_write(2, buf, strlen(buf));
      if (stack.size() - i - 1 < 10) {
        msg = ":  ";
      } else {
        msg = ": ";
      }
      mzn_write(2, msg, strlen(msg));
      msg = Expression::cast<Call>(stack[i].e)->id().c_str();
      mzn_write(2, msg, strlen(msg));
      msg = " at ";
      mzn_write(2, msg, strlen(msg));
      const Location& loc = Expression::cast<Call>(stack[i].e)->decl()->loc();
      msg = loc.filename().c_str();
      if (msg == nullptr) {
        msg = "unknown file";
      } else {
        msg = basename_async_safe(msg);
      }
      mzn_write(2, msg, strlen(msg));
      msg = ":";
      mzn_write(2, msg, strlen(msg));
      itoa_async_safe(loc.firstLine(), buf);
      mzn_write(2, buf, strlen(buf));
      msg = ".";
      mzn_write(2, msg, strlen(msg));
      itoa_async_safe(loc.firstColumn(), buf);
      mzn_write(2, buf, strlen(buf));
      msg = "\n";
      mzn_write(2, msg, strlen(msg));
      if (--count == 0) {
        break;
      }
    }
  }
  if (stack.size() > 15) {
    msg = "  ...\n";
    mzn_write(2, msg, strlen(msg));
  }
}

}  // namespace

#if defined(_WIN32)

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

struct OverflowHandler::OverflowInfo {
  const char* stackTop;
  EnvI* env;
  void* altstack;
  OverflowInfo(const char** argv, void* altstack0)
      : stackTop(*argv), env(nullptr), altstack(altstack0) {}
  ~OverflowInfo() { ::free(altstack); }
  static void overflow(int sig, siginfo_t* info, void* context);
};

#if defined(__APPLE__)

#define MZN_CAN_HANDLE_OVERFLOW
#define SEGV_ADDR reinterpret_cast<const char*>(info->si_addr)
#define SIGFLAGS (SA_SIGINFO | SA_64REGSET)

#if defined(__x86_64__)
#define SP_ADDR (const char*)((ucontext_t*)context)->uc_mcontext->__ss.__rsp
#elif defined(__aarch64__)
#define SP_ADDR (const char*)((ucontext_t*)context)->uc_mcontext->__ss.__sp
#else
#undef MZN_CAN_HANDLE_OVERFLOW
#endif

#elif defined(__x86_64__) && defined(__linux__)

#define MZN_CAN_HANDLE_OVERFLOW
#define SEGV_ADDR reinterpret_cast<const char*>(((ucontext_t*)context)->uc_mcontext.gregs[REG_CR2])
#define SP_ADDR (const char*)((ucontext_t*)context)->uc_mcontext.gregs[REG_RSP]
#define SIGFLAGS SA_SIGINFO

#elif defined(__aarch64__) && defined(__linux__)

#define MZN_CAN_HANDLE_OVERFLOW
#define SEGV_ADDR reinterpret_cast<const char*>(((ucontext_t*)context)->uc_mcontext.fault_address)
#define SP_ADDR (const char*)((ucontext_t*)context)->uc_mcontext.sp
#define SIGFLAGS SA_SIGINFO

#elif defined(__arm__) && defined(__linux__)

#define MZN_CAN_HANDLE_OVERFLOW
#define SEGV_ADDR reinterpret_cast<const char*>(((ucontext_t*)context)->uc_mcontext.fault_address)
#define SP_ADDR (const char*)((ucontext_t*)context)->uc_mcontext.arm_sp
#define SIGFLAGS SA_SIGINFO

#else

#undef MZN_CAN_HANDLE_OVERFLOW

#endif

#if defined(MZN_CAN_HANDLE_OVERFLOW)

void OverflowHandler::OverflowInfo::overflow(int sig, siginfo_t* info, void* context) {
  ucontext_t ctx;
  const char* segv_addr = SEGV_ADDR;
  const char* ctx_sp = SP_ADDR;
  if (segv_addr < _ofi->stackTop && segv_addr >= ctx_sp - 1024) {
    const char* msg =
        "MiniZinc error: Memory violation detected (segmentation fault).\n"
        "This is most likely due to a stack overflow.\n"
        "Check for deep (or infinite) recursion in the model.\n";
    mzn_write(2, msg, strlen(msg));
    if (_ofi->env != nullptr) {
      dump_stack(_ofi->env->callStack);
    }
    _exit(1);
  } else {
    const char* msg = "MiniZinc error: Memory violation detected (segmentation fault).\n";
    mzn_write(2, msg, strlen(msg));
    msg = "This is a bug. Please file a bug report using the MiniZinc bug tracker.\n";
    mzn_write(2, msg, strlen(msg));
    abort();
  }
}

void OverflowHandler::install(const char** argv) {
  _ofi = std::unique_ptr<OverflowInfo>(new OverflowInfo(argv, ::malloc(SIGSTKSZ)));
  stack_t stk;
  stk.ss_sp = _ofi->altstack;
  if (stk.ss_sp != nullptr) {
    stk.ss_size = SIGSTKSZ;
    stk.ss_flags = 0;
    if (sigaltstack(&stk, nullptr) >= 0) {
      struct sigaction act;
      act.sa_sigaction = OverflowInfo::overflow;
      act.sa_flags = SIGFLAGS;
      act.sa_flags |= SA_ONSTACK | SA_NODEFER;
      sigemptyset(&act.sa_mask);
      sigaction(SIGSEGV, &act, nullptr);
      return;
    }
  }
  _ofi.reset();
  std::cerr << "WARNING: Cannot initialise stack overflow handler." << std::endl;
}

void OverflowHandler::setEnv(Env& env) { _ofi->env = &env.envi(); }
void OverflowHandler::removeEnv() { _ofi->env = nullptr; }

#else

void OverflowHandler::OverflowInfo::overflow(int sig, siginfo_t* info, void* context) {}
void OverflowHandler::install(const char** argv) {}
void OverflowHandler::setEnv(Env& env) {}
void OverflowHandler::removeEnv() {}

#endif
#endif

}  // namespace MiniZinc
