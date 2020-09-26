/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/solns2out.hh>
#include <minizinc/solvers/nl/nl_file.hh>

#include <cstring>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

// Reading a .sol file.
// Note: I did not find any description of the format, so I did a bit of reverse engineering.
// A sol file looks like this (first line is the 'message...' line)
/*
    message from the solver

    Options
    3
    1
    1
    0
    1
    0
    2
    2
    1
    2
    objno 0 0
*/
// A sol file does not contains comments, and the blank line between the message and 'Options' is
// meaningfull. Now, with our own comments.
/*
    message from the solver # Starts on first line. Can span several line as long as their is no
   blank line <blank line>            # the blank line marks the end of the message Options # Inform
   about option used by the solver (?). May be absent. 3                       # Number of options.
    1                       # opt 1         Note:   I did not fully understand this, but reading
   "hooking your solver" 1                       # opt 2                 suggests that we can
   (probably) ignore that. 0                       # opt 3 1                       # Number A of
   dual before "suf_sos()" (before solving ?) 0                       # Number of dual after. Seems
   to be either A or 0. Will probably always be 0 (???) 2                       # Number B of primal
   before "suf_sos()" (before solving ?) 2                       # Number of primal after. Seems to
   be either B or 0 1                       # No dual. So this is the result for the first primal
   (variable indice 0) 2                       # result for variable indice 1 objno 0 0 # Objectif 0
   <result code>
*/
// Result code can be :
// (from https://github.com/ampl/mp/blob/master/include/mp/common.h)
// UNKNOWN     = -1,
// SOLVED      = 0,     // Optimal sol found for an optim problem or a feasible solution found for a
// satisfaction problem. UNCERTAIN   = 100,   // Solution returned but it can be non-optimal or even
// infeasible. INFEASIBLE  = 200, UNBOUNDED   = 300,   // Problem is unbounded. LIMIT       = 400,
// // Stopped by a limit, e.g. on iterations or time. FAILURE     = 500,   // A solver error.
// INTERRUPTED = 600    // Interrupted by the user.

namespace MiniZinc {

/** Declaration of the exit codes. */
enum NL_Solver_Status {
  PARSE_ERROR = -2,  // We are adding our own error code for parsing
  UNKNOWN = -1,
  SOLVED = 0,       // Optimal sol found for an optim problem or a feasible solution found for a
                    // satisfaction problem.
  UNCERTAIN = 100,  // Solution returned but it can be non-optimal or even infeasible.
  INFEASIBLE = 200,
  UNBOUNDED = 300,   // Problem is unbounded.
  LIMIT = 400,       // Stopped by a limit, e.g. on iterations or time.
  FAILURE = 500,     // A solver error.
  INTERRUPTED = 600  // Interrupted by the user.
};

/** Represent a solution read from a file '.sol'. */
class NLSol {
public:
  // --- --- --- Fields
  std::string message;
  NL_Solver_Status status;
  std::vector<double> values;

  // --- --- --- Constructors

  NLSol() = default;

  NLSol(std::string mes, NL_Solver_Status st, std::vector<double> res)
      : message(std::move(mes)), status(st), values(std::move(res)) {}

  // --- --- --- Static functions

  static NLSol parseSolution(std::istream& in);
};

/** Our version of Solns2Out **/

class NLSolns2Out {
private:
  Solns2Out* _out;
  NLFile& _nlFile;
  std::ofstream _dummyOfstream;

  // Controls for feedRawDataChunk
  bool _inLine;
  bool _verbose;

public:
  NLSolns2Out(Solns2Out* out0, NLFile& nl_file0, bool verbose0)
      : _out(out0), _nlFile(nl_file0), _inLine(false), _verbose(verbose0) {}

  void parseSolution(const std::string& filename);

  bool feedRawDataChunk(const char* data);
  std::ostream& getLog();
};
}  // namespace MiniZinc
