/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/utils.hh>

#if defined(__x86_64__) && defined(__APPLE__)
#include <csignal>
#include <unistd.h>
#endif

namespace MiniZinc {

OverflowHandler::OverflowHandler() {}

std::unique_ptr<OverflowHandler::OverflowInfo> OverflowHandler::_ofi;

#if defined(__x86_64__) && defined(__APPLE__)

struct OverflowHandler::OverflowInfo {
  unsigned long long stack_top;
  OverflowInfo(const char** argv) : stack_top(reinterpret_cast<unsigned long long>(*argv)) {}
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
    exit(1);
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

#else

struct OverflowHandler::OverflowInfo {};

#ifdef _WIN32
void OverflowHandler::install(wchar_t** argv) {}
#else
void OverflowHandler::install(const char** argv) {}
#endif

#endif

}  // namespace MiniZinc
