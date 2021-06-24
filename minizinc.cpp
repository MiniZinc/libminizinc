/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

/* This (main) file coordinates flattening and solving.
 * The corresponding modules are flexibly plugged in
 * as derived classes, prospectively from DLLs.
 * A flattening module should provide MinZinc::GetFlattener()
 * A solving module should provide an object of a class derived from SolverFactory.
 * Need to get more flexible for multi-pass & multi-solving stuff  TODO
 */

#include <minizinc/solver.hh>

#include <chrono>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <ratio>

using namespace MiniZinc;

namespace {

int run(const std::string& exe, const std::vector<std::string>& args) {
  try {
    Timer startTime;
    bool fSuccess = false;
    MznSolver slv(std::cout, std::cerr, startTime);
    try {
      fSuccess = (slv.run(args, "", exe) != SolverInstance::ERROR);
    } catch (const InternalError& e) {
      if (slv.getFlagVerbose()) {
        std::cerr << std::endl;
      }
      std::cerr << "MiniZinc has encountered an internal error. This is a bug." << std::endl;
      std::cerr << "Please file a bug report using the MiniZinc bug tracker." << std::endl;
      std::cerr << "The internal error message was: " << std::endl;
      std::cerr << "\"" << e.msg() << "\"" << std::endl;
    } catch (const Exception& e) {
      if (slv.flagEncapsulateJSON) {
        e.json(std::cout);
      } else {
        if (slv.getFlagVerbose()) {
          std::cerr << std::endl;
        }
        e.print(std::cerr);
      }
    } catch (const std::exception& e) {
      if (slv.getFlagVerbose()) {
        std::cerr << std::endl;
      }
      std::cerr << e.what() << std::endl;
    } catch (...) {
      if (slv.getFlagVerbose()) {
        std::cerr << std::endl;
      }
      std::cerr << "  UNKNOWN EXCEPTION." << std::endl;
    }

    if (slv.getFlagVerbose()) {
      std::cerr << "   Done (";
      std::cerr << "overall time " << startTime.stoptime() << ")." << std::endl;
    }
    return static_cast<int>(!fSuccess);
  } catch (const Exception& e) {
    std::string what = e.what();
    std::cerr << what << (what.empty() ? "" : ": ") << e.msg() << std::endl;
    std::exit(EXIT_FAILURE);
  }
}

}  // namespace

#ifdef _WIN32
#include <minizinc/interrupt.hh>

int wmain(int argc, wchar_t* argv[], wchar_t* envp[]) {
  InterruptListener::run();
  OverflowHandler::install();
  std::vector<std::string> args(argc - 1);
  for (int i = 1; i < argc; i++) {
    args[i - 1] = FileUtils::wide_to_utf8(argv[i]);
  }
  auto exe = FileUtils::wide_to_utf8(argv[0]);

#ifdef NDEBUG
  // Lambda to prevent object unwinding not allowed with __try..__except
  return ([&]() {
    __try {
      return run(exe, args);
    } __except (OverflowHandler::filter(GetExceptionCode())) {
      OverflowHandler::handle(GetExceptionCode());
    }
  })();
#else
  // Let debugger catch SEH exceptions
  return run(exe, args);
#endif
}
#else
int main(int argc, const char** argv) {
  OverflowHandler::install(argv);
  std::vector<std::string> args(argc - 1);
  for (int i = 1; i < argc; i++) {
    args[i - 1] = argv[i];
  }
  return run(argv[0], args);
}
#endif
