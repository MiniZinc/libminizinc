/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/solvers/nl/nl_solreader.hh>

using namespace std;

namespace MiniZinc {

// *** *** *** NLSol *** *** ***

// Parse a string into a NLSol object
NLSol NLSol::parseSolution(istream& in) {
  string buffer;
  string msg;
  vector<double> vec;
  NL_Solver_Status st;

  try {
    // Read the message
    while (getline(in, buffer) && !buffer.empty()) {
      msg += buffer + '\n';
    }

    if (in.bad()) {
      return NLSol("Error reading the solver message", NL_Solver_Status::PARSE_ERROR, {});
    }

    // Check if we have 'Options', and skip them
    if (getline(in, buffer)) {
      if (buffer == "Options") {
        if (getline(in, buffer)) {
          int nb_options = stoi(buffer);
          while (nb_options > 0) {
            getline(in, buffer);
            nb_options--;
          }
        }
      }
    }

    if (in.bad()) {
      return NLSol("Error reading the solver Options", NL_Solver_Status::PARSE_ERROR, {});
    }

    // Check the dual: we ignore the first, read the second. If non zero, some lines are to be
    // skipped
    (getline(in, buffer) && getline(in, buffer));
    if (in.bad()) {
      return NLSol("Error reading the number of dual", NL_Solver_Status::PARSE_ERROR, {});
    }
    int nb_duals = stoi(buffer);

    // Check the primal: we ignore the first, read the second
    getline(in, buffer) && getline(in, buffer);
    if (in.bad()) {
      return NLSol("Error reading the number of primal", NL_Solver_Status::PARSE_ERROR, {});
    }
    int nb_vars = stoi(buffer);

    // Skip the duals
    while (nb_duals > 0 && getline(in, buffer)) {
      nb_duals--;
    }

    if (in.bad()) {
      return NLSol("Error reading the dual values", NL_Solver_Status::PARSE_ERROR, {});
    }

    // Read the vars
    while (nb_vars > 0 && getline(in, buffer)) {
      double d = stod(buffer);
      vec.push_back(d);
      nb_vars--;
    }

    if (in.bad()) {
      return NLSol("Error reading the primal values", NL_Solver_Status::PARSE_ERROR, {});
    }

    // Reading status code
    // objno 0 EXIT
    // ........
    // 8 char
    getline(in, buffer);
    string sub = buffer.substr(8, buffer.length() - 8);
    int resultCode = stoi(sub);
    st = NL_Solver_Status::UNKNOWN;
    // Not this case, this one is our own: case -2: st = NL_Solver_Status::PARSE_ERROR;
    if (resultCode == -1) {
      st = NL_Solver_Status::UNKNOWN;
    } else if (resultCode >= 0 && resultCode < 100) {
      st = NL_Solver_Status::SOLVED;
    } else if (resultCode >= 100 && resultCode < 200) {
      st = NL_Solver_Status::UNCERTAIN;
    } else if (resultCode >= 200 && resultCode < 300) {
      st = NL_Solver_Status::INFEASIBLE;
    } else if (resultCode >= 300 && resultCode < 400) {
      st = NL_Solver_Status::UNBOUNDED;
    } else if (resultCode >= 400 && resultCode < 500) {
      st = NL_Solver_Status::LIMIT;
    } else if (resultCode >= 500 && resultCode < 600) {
      st = NL_Solver_Status::FAILURE;
    } else if (resultCode == 600) {
      st = NL_Solver_Status::INTERRUPTED;
    }

  } catch (...) {
    return NLSol("Parsing error (probably a bad number)", NL_Solver_Status::PARSE_ERROR, vec);
  }

  return NLSol(msg, st, vec);
}

// *** *** *** NLSolns2Out *** *** ***

/** Our "feedrawdatachunk" directly gets the solver's output, which is not the result.
 *  The result is written in the .sol file.
 *  Get the solver output and add a comment % in front of the lines
 *  We may be in a middle of a line!
 */
bool NLSolns2Out::feedRawDataChunk(const char* data) {
  if (data != nullptr) {
    std::stringstream ss(data);
    string to;

    while (getline(ss, to)) {
      if (ss.eof()) {
        if (_inLine) {  // Must complete a line, and the line is not over yet
          getLog() << to << endl;
        } else {  // Start an incomple line
          getLog() << "% " << to;
          _inLine = true;
        }
      } else {
        if (_inLine) {  // Must complete a line, and the line is over.
          getLog() << to << endl;
          _inLine = false;
        } else {  // Full line
          getLog() << "% " << to << endl;
        }
      }
    }
  }
  return true;
}

void NLSolns2Out::parseSolution(const string& filename) {
  ifstream f(FILE_PATH(filename));
  NLSol sol = NLSol::parseSolution(f);

  switch (sol.status) {
    case NL_Solver_Status::PARSE_ERROR: {
      DEBUG_MSG("NL_Solver_Status: PARSE ERROR" << endl);
      _out->feedRawDataChunk(_out->opt.errorMsgDef);
      break;
    }

    case NL_Solver_Status::UNKNOWN: {
      DEBUG_MSG("NL_Solver_Status: UNKNOWN" << endl);
      _out->feedRawDataChunk(_out->opt.unknownMsgDef);
      break;
    }

    case NL_Solver_Status::SOLVED: {
      DEBUG_MSG("NL_Solver_Status: SOLVED" << endl);

      stringstream sb;
      // sb << std::hexfloat;  // Use hexadecimal format for FP
      sb << std::showpoint;  // Always shows the decimal point, so we have e.g. '256.0' when 256 is
                             // the answer for a fp value.
      sb.precision(numeric_limits<double>::digits10 + 2);

      for (int i = 0; i < _nlFile.variables.size(); ++i) {
        string n = _nlFile.vnames[i];
        NLVar v = _nlFile.variables[n];
        if (v.toReport) {
          sb << v.name << " = ";
          if (v.isInteger) {
            long value = sol.values[i];
            sb << value;
          } else {
            double value = sol.values[i];
            sb << value;
          }
          sb << ";\n";
        }
      }

      // Output the arrays
      for (auto& a : _nlFile.outputArrays) {
        sb << a.name << " = array" << a.dimensions.size() << "d( ";
        for (const string& s : a.dimensions) {
          sb << s << ", ";
        }
        sb << "[";
        for (int j = 0; j < a.items.size(); ++j) {
          const NLArray::Item& item = a.items.at(j);

          // Case of the literals
          if (item.variable.empty()) {
            if (a.isInteger) {
              sb << static_cast<long long int>(item.value);
            } else {
              sb << item.value;
            }
          } else {
            int index = _nlFile.variableIndexes.at(item.variable);
            if (a.isInteger) {
              long value = sol.values[index];
              sb << value;
            } else {
              double value = sol.values[index];
              sb << value;
            }
          }

          if (j < a.items.size() - 1) {
            sb << ", ";
          }
        }

        sb << "]);\n";
      }

      string s = sb.str();
      _out->feedRawDataChunk(s.c_str());
      _out->feedRawDataChunk(_out->opt.solutionSeparatorDef);
      if (_nlFile.objective.isOptimisation()) {
        _out->feedRawDataChunk("\n");
        _out->feedRawDataChunk(_out->opt.searchCompleteMsgDef);
      }

      break;
    }

    case NL_Solver_Status::UNCERTAIN: {
      DEBUG_MSG("NL_Solver_Status: UNCERTAIN" << endl);
      _out->feedRawDataChunk(_out->opt.unknownMsgDef);
      break;
    }

    case NL_Solver_Status::INFEASIBLE: {
      DEBUG_MSG("NL_Solver_Status: INFEASIBLE" << endl);
      _out->feedRawDataChunk(_out->opt.unsatisfiableMsgDef);
      break;
    }

    case NL_Solver_Status::UNBOUNDED: {
      DEBUG_MSG("NL_Solver_Status: UNBOUNDED" << endl);
      _out->feedRawDataChunk(_out->opt.unboundedMsgDef);
      break;
    }

    case NL_Solver_Status::LIMIT: {
      DEBUG_MSG("NL_Solver_Status: LIMIT" << endl);
      _out->feedRawDataChunk(_out->opt.unknownMsgDef);
      break;
    }

    case NL_Solver_Status::INTERRUPTED: {
      DEBUG_MSG("NL_Solver_Status: INTERRUPTED" << endl);
      _out->feedRawDataChunk(_out->opt.unknownMsgDef);
      break;
    }

    default:
      should_not_happen("parseSolution: switch on status with unknown code: " << sol.status);
  }

  // "Finish" the feed
  _out->feedRawDataChunk("\n");
}

ostream& NLSolns2Out::getLog() { return _verbose ? _out->getLog() : _dummyOfstream; }

}  // namespace MiniZinc
