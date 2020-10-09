/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Jason Nguyen <jason.nguyen@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#ifdef _WIN32

#define NOMINMAX  // Ensure the words min/max remain available
#include <Windows.h>
#undef ERROR
#include <sstream>
#include <thread>

namespace MiniZinc {
// Listens for a message on the named pipe \\.\pipe\minizinc-PID
// Triggers a Ctrl+C when an empty message is received.
class InterruptListener {
public:
  static InterruptListener& run() {
    static InterruptListener instance;
    return instance;
  }
  InterruptListener(InterruptListener const&) = delete;
  void operator=(InterruptListener const&) = delete;

  ~InterruptListener() {
    SetConsoleCtrlHandler(CtrlHandler, FALSE);
    SetEvent(hEvents[0]);
    thread.join();
    CloseHandle(hNamedPipe);
    CloseHandle(hEvents[0]);
    CloseHandle(hEvents[1]);
  }

private:
  std::thread thread;
  HANDLE hNamedPipe;

  InterruptListener() {
    // Setup events
    hEvents[0] = CreateEvent(NULL, FALSE, FALSE, NULL);  // Signalled when thread needs to exit
    hEvents[1] = CreateEvent(NULL, FALSE, FALSE, NULL);  // Signalled on pipe events

    // Setup a named pipe so that the IDE can trigger an interrupt
    std::stringstream ss;
    ss << "\\\\.\\pipe\\minizinc-" << GetCurrentProcessId();
    std::string pipeName = ss.str();
    hNamedPipe = CreateNamedPipe(pipeName.c_str(), PIPE_ACCESS_INBOUND | FILE_FLAG_OVERLAPPED,
                                 PIPE_TYPE_MESSAGE, 1, 0, 0, 0, NULL);

    if (hEvents[0] && hEvents[1] && hNamedPipe) {
      SetConsoleCtrlHandler(CtrlHandler, TRUE);
      thread = std::thread(&InterruptListener::listen, this);
    }
  }

  void listen() {
    OVERLAPPED ol;

    // Connect pipe
    ZeroMemory(&ol, sizeof(OVERLAPPED));
    ol.hEvent = hEvents[1];
    ConnectNamedPipe(hNamedPipe, &ol);
    DWORD ev = WaitForMultipleObjects(2, &hEvents[0], FALSE, INFINITE);
    if (ev - WAIT_OBJECT_0 == 0) {
      return;
    }

    // Listen for pings on pipe
    while (true) {
      ZeroMemory(&ol, sizeof(OVERLAPPED));
      ol.hEvent = hEvents[1];
      ReadFile(hNamedPipe, NULL, 0, NULL, &ol);

      DWORD ev = WaitForMultipleObjects(2, &hEvents[0], FALSE, INFINITE);
      if (ev - WAIT_OBJECT_0 == 0) {
        return;
      }

      GenerateConsoleCtrlEvent(CTRL_C_EVENT, 0);
    }
  }

  static HANDLE hEvents[2];

  static BOOL WINAPI CtrlHandler(DWORD fdwCtrlType) {
    // Tell thread to stop
    SetEvent(hEvents[0]);
    return FALSE;
  }
};

HANDLE InterruptListener::hEvents[2];

}  // namespace MiniZinc

#endif
