/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 *     Gleb Belov <gleb.belov@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was ! distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/solns2out.hh>
#include <minizinc/solver.hh>

#include <fstream>
#include <utility>

using namespace std;
using namespace MiniZinc;

void Solns2Out::printHelp(ostream& os) {
  os << "Solution output options:" << std::endl
     << "  --ozn-file <file>\n    Read output specification from ozn file." << std::endl
     << "  -o <file>, --output-to-file <file>\n    Filename for generated output." << std::endl
     << "  -i <n>, --ignore-lines <n>, --ignore-leading-lines <n>\n    Ignore the first <n> lines "
        "in the FlatZinc solution stream."
     << std::endl
     << "  --soln-sep <s>, --soln-separator <s>, --solution-separator <s>\n    Specify the string "
        "printed after each solution (as a separate line).\n    The default is to use the same as "
        "FlatZinc, \"----------\"."
     << std::endl
     << "  --soln-comma <s>, --solution-comma <s>\n    Specify the string used to separate "
        "solutions.\n    The default is the empty string."
     << std::endl
     << "  --unsat-msg (--unsatisfiable-msg), --unbounded-msg, --unsatorunbnd-msg,\n"
        "        --unknown-msg, --error-msg, --search-complete-msg <msg>\n"
        "    Specify solution status messages. The defaults:\n"
        "    \"=====UNSATISFIABLE=====\", \"=====UNSATorUNBOUNDED=====\", "
        "\"=====UNBOUNDED=====\",\n"
        "    \"=====UNKNOWN=====\", \"=====ERROR=====\", \"==========\", respectively."
     << std::endl
     << "  --non-unique\n    Allow duplicate solutions.\n"
     << "  -c, --canonicalize\n    Canonicalize the output solution stream (i.e., buffer and "
        "sort).\n"
     << "  --output-non-canonical <file>\n    Non-buffered solution output file in case of "
        "canonicalization.\n"
     << "  --output-raw <file>\n    File to dump the solver's raw output (not for hard-linked "
        "solvers)\n"
     // Unclear how to exit then:
     //   << "  --number-output <n>\n    Maximal number of different solutions printed." <<
     //   std::endl
     << "  --no-output-comments\n    Do not print comments in the FlatZinc solution stream."
     << std::endl
     << "  --output-time\n    Print timing information in the FlatZinc solution stream."
     << std::endl
     << "  --no-flush-output\n    Don't flush output stream after every line." << std::endl;
}

bool Solns2Out::processOption(int& i, std::vector<std::string>& argv) {
  CLOParser cop(i, argv);
  std::string oznfile;
  if (cop.getOption("--ozn-file", &oznfile)) {
    initFromOzn(oznfile);
  } else if (cop.getOption("-o --output-to-file", &_opt.flag_output_file)) {
    // Parsed by reference
  } else if (cop.getOption("--no-flush-output")) {
    _opt.flag_output_flush = false;
  } else if (cop.getOption("--no-output-comments")) {
    _opt.flag_output_comments = false;
  } else if (cop.getOption("-i --ignore-lines --ignore-leading-lines", &_opt.flag_ignore_lines)) {
    // Parsed by reference
  } else if (cop.getOption("--output-time")) {
    _opt.flag_output_time = true;
  } else if (cop.getOption("--soln-sep --soln-separator --solution-separator",
                           &_opt.solution_separator)) {
    // Parsed by reference
  } else if (cop.getOption("--soln-comma --solution-comma", &_opt.solution_comma)) {
    // Parsed by reference
  } else if (cop.getOption("--unsat-msg --unsatisfiable-msg", &_opt.unsatisfiable_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--unbounded-msg", &_opt.unbounded_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--unsatorunbnd-msg", &_opt.unsatorunbnd_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--unknown-msg", &_opt.unknown_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--error-msg", &_opt.error_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--search-complete-msg", &_opt.search_complete_msg)) {
    // Parsed by reference
  } else if (cop.getOption("--unique")) {
    _opt.flag_unique = true;
  } else if (cop.getOption("--non-unique")) {
    _opt.flag_unique = false;
  } else if (cop.getOption("-c --canonicalize")) {
    _opt.flag_canonicalize = true;
  } else if (cop.getOption("--output-non-canonical --output-non-canon",
                           &_opt.flag_output_noncanonical)) {
    // Parsed by reference
  } else if (cop.getOption("--output-raw", &_opt.flag_output_raw)) {
    // Parsed by reference
  } else if (_opt.flag_standaloneSolns2Out) {
    std::string oznfile(argv[i]);
    if (oznfile.length() <= 4) {
      return false;
    }
    size_t last_dot = oznfile.find_last_of('.');
    if (last_dot == string::npos) {
      return false;
    }
    std::string extension = oznfile.substr(last_dot, string::npos);
    if (extension == ".ozn") {
      initFromOzn(oznfile);
      return true;
    }
    return false;
  } else {
    return false;
  }
  return true;
}

bool Solns2Out::initFromEnv(Env* pE) {
  assert(pE);
  pEnv = pE;
  includePaths.push_back(stdlibDir + "/std/");
  init();
  return true;
}

void Solns2Out::initFromOzn(const std::string& filename) {
  std::vector<string> filenames(1, filename);

  includePaths.push_back(stdlibDir + "/std/");

  for (auto& includePath : includePaths) {
    if (!FileUtils::directory_exists(includePath)) {
      std::cerr << "solns2out: cannot access include directory " << includePath << "\n";
      std::exit(EXIT_FAILURE);
    }
  }

  {
    pEnv = new Env();
    std::stringstream errstream;
    if ((pOutput = parse(*pEnv, filenames, std::vector<std::string>(), "", "", includePaths, false,
                         false, false, false, errstream)) != nullptr) {
      std::vector<TypeError> typeErrors;
      pEnv->model(pOutput);
      MZN_ASSERT_HARD_MSG(pEnv, "solns2out: could not allocate Env");
      pEnv_guard.reset(pEnv);
      MiniZinc::typecheck(*pEnv, pOutput, typeErrors, false, false);
      MiniZinc::registerBuiltins(*pEnv);
      pEnv->envi().swap_output();
      init();
    } else {
      throw Error(errstream.str());
    }
  }
}

Solns2Out::DE& Solns2Out::findOutputVar(ASTString name) {
  declNewOutput();
  auto it = declmap.find(name);
  MZN_ASSERT_HARD_MSG(declmap.end() != it, "solns2out_base: unexpected id in output: " << name);
  return it->second;
}

void Solns2Out::restoreDefaults() {
  for (auto& i : *getModel()) {
    if (auto* vdi = i->dyn_cast<VarDeclI>()) {
      if (vdi->e()->id()->idn() != -1 || (vdi->e()->id()->v() != "_mzn_solution_checker" &&
                                          vdi->e()->id()->v() != "_mzn_stats_checker")) {
        GCLock lock;
        auto& de = findOutputVar(vdi->e()->id()->str());
        vdi->e()->e(de.second());
        vdi->e()->evaluated(false);
      }
    }
  }
  fNewSol2Print = false;
}

void Solns2Out::parseAssignments(string& solution) {
  std::vector<SyntaxError> se;
  unique_ptr<Model> sm(parseFromString(*pEnv, solution, "solution received from solver",
                                       includePaths, false, true, false, false, log, se));
  if (sm.get() == nullptr) {
    throw Error("solns2out_base: could not parse solution");
  }
  solution = "";
  for (unsigned int i = 0; i < sm->size(); i++) {
    if (auto* ai = (*sm)[i]->dyn_cast<AssignI>()) {
      auto& de = findOutputVar(ai->id());
      if (!ai->e()->isa<BoolLit>() && !ai->e()->isa<IntLit>() && !ai->e()->isa<FloatLit>()) {
        Type de_t = de.first->type();
        de_t.cv(false);
        ai->e()->type(de_t);
      }
      ai->decl(de.first);
      typecheck(*pEnv, getModel(), ai);
      if (Call* c = ai->e()->dyn_cast<Call>()) {
        // This is an arrayXd call, make sure we get the right builtin
        assert(c->arg(c->n_args() - 1)->isa<ArrayLit>());
        for (unsigned int i = 0; i < c->n_args(); i++) {
          c->arg(i)->type(Type::parsetint());
        }
        c->arg(c->n_args() - 1)->type(de.first->type());
        c->decl(getModel()->matchFn(pEnv->envi(), c, false));
      }
      de.first->e(ai->e());
    }
  }
  declNewOutput();
}

void Solns2Out::declNewOutput() {
  fNewSol2Print = true;
  status = SolverInstance::SAT;
}

bool Solns2Out::evalOutput(const string& s_ExtraInfo) {
  if (!fNewSol2Print) {
    return true;
  }
  ostringstream oss;
  if (!checkerModel.empty()) {
    auto& checkerStream = pEnv->envi().checker_output;
    checkerStream.clear();
    checkerStream.str("");
    checkSolution(checkerStream);
  }
  if (!__evalOutput(oss)) {
    return false;
  }
  bool fNew = true;
  if (_opt.flag_unique || _opt.flag_canonicalize) {
    auto res = sSolsCanon.insert(oss.str());
    if (!res.second) {  // repeated solution
      fNew = false;
    }
  }
  if (fNew) {
    {
      auto& checkerStream = pEnv->envi().checker_output;
      checkerStream.flush();
      std::string line;
      if (std::getline(checkerStream, line)) {
        os << "% Solution checker report:\n";
        os << "% " << line << "\n";
        while (std::getline(checkerStream, line)) {
          os << "% " << line << "\n";
        }
      }
    }
    ++_stats.nSolns;
    if (_opt.flag_canonicalize) {
      if (pOfs_non_canon.get() != nullptr) {
        if (pOfs_non_canon->good()) {
          (*pOfs_non_canon) << oss.str();
          (*pOfs_non_canon) << comments;
          if (!s_ExtraInfo.empty()) {
            (*pOfs_non_canon) << s_ExtraInfo;
            if ('\n' != s_ExtraInfo.back()) {  /// TODO is this enough to check EOL?
              (*pOfs_non_canon) << '\n';
            }
          }
          if (_opt.flag_output_time) {
            (*pOfs_non_canon) << "% time elapsed: " << starttime.stoptime() << "\n";
          }
          if (!_opt.solution_separator.empty()) {
            (*pOfs_non_canon) << _opt.solution_separator << '\n';
          }
          if (_opt.flag_output_flush) {
            pOfs_non_canon->flush();
          }
        }
      }
    } else {
      if ((!_opt.solution_comma.empty()) && _stats.nSolns > 1) {
        getOutput() << _opt.solution_comma << '\n';
      }
      getOutput() << oss.str();
    }
  }
  getOutput() << comments;  // print them now ????
  comments = "";
  if (!s_ExtraInfo.empty()) {
    getOutput() << s_ExtraInfo;
    if ('\n' != s_ExtraInfo.back()) {  /// TODO is this enough to check EOL?
      getOutput() << '\n';
    }
  }
  if (fNew && _opt.flag_output_time) {
    getOutput() << "% time elapsed: " << starttime.stoptime() << "\n";
  }
  if (fNew && !_opt.flag_canonicalize && !_opt.solution_separator.empty()) {
    getOutput() << _opt.solution_separator << '\n';
  }
  if (_opt.flag_output_flush) {
    getOutput().flush();
  }
  restoreDefaults();  // cleans data. evalOutput() should not be called again w/o assigning new
                      // data.
  return true;
}

void Solns2Out::checkSolution(std::ostream& oss) {
#ifdef HAS_GECODE

  std::ostringstream checker;
  checker << checkerModel;
  {
    GCLock lock;
    for (auto& i : *getModel()) {
      if (auto* vdi = i->dyn_cast<VarDeclI>()) {
        if (vdi->e()->ann().contains(constants().ann.mzn_check_var)) {
          checker << vdi->e()->id()->str() << " = ";
          Expression* e = eval_par(getEnv()->envi(), vdi->e()->e());
          auto* al = e->dyn_cast<ArrayLit>();
          std::vector<Id*> enumids;
          if (Call* cev = vdi->e()->ann().getCall(constants().ann.mzn_check_enum_var)) {
            auto* enumIdsAl = cev->arg(0)->cast<ArrayLit>();
            for (int j = 0; j < enumIdsAl->size(); j++) {
              enumids.push_back((*enumIdsAl)[j]->dyn_cast<Id>());
            }
          }

          if (al != nullptr) {
            checker << "array" << al->dims() << "d(";
            for (int i = 0; i < al->dims(); i++) {
              if (!enumids.empty() && enumids[i] != nullptr) {
                checker << "to_enum(" << *enumids[i] << ",";
              }
              checker << al->min(i) << ".." << al->max(i);
              if (!enumids.empty() && enumids[i] != nullptr) {
                checker << ")";
              }
              checker << ",";
            }
          }
          if (!enumids.empty() && enumids.back() != nullptr) {
            checker << "to_enum(" << *enumids.back() << "," << *e << ")";
          } else {
            checker << *e;
          }
          if (al != nullptr) {
            checker << ")";
          }
          checker << ";\n";
        }
      }
    }
  }

  MznSolver slv(oss, oss);
  slv.s2out._opt.solution_separator = "";
  try {
    std::vector<std::string> args({"--solver", "org.minizinc.gecode_presolver"});
    slv.run(args, checker.str(), "minizinc", "checker.mzc");
  } catch (const LocationException& e) {
    oss << e.loc() << ":" << std::endl;
    oss << e.what() << ": " << e.msg() << std::endl;
  } catch (const Exception& e) {
    std::string what = e.what();
    oss << what << (what.empty() ? "" : ": ") << e.msg() << std::endl;
  } catch (const exception& e) {
    oss << e.what() << std::endl;
  } catch (...) {
    oss << "  UNKNOWN EXCEPTION." << std::endl;
  }

#else
  oss << "% solution checking not supported (need built-in Gecode)" << std::endl;
#endif
}

void Solns2Out::checkStatistics(std::ostream& oss) {
#ifdef HAS_GECODE

  std::ostringstream checker;
  checker << statisticsCheckerModel;
  checker << "mzn_stats_failures = " << _stats.nFails << ";\n";
  checker << "mzn_stats_solutions = " << _stats.nSolns << ";\n";
  checker << "mzn_stats_nodes = " << _stats.nNodes << ";\n";
  checker << "mzn_stats_time = " << starttime.ms() << ";\n";

  MznSolver slv(oss, oss);
  slv.s2out._opt.solution_separator = "";
  try {
    std::vector<std::string> args({"--solver", "org.minizinc.gecode_presolver"});
    slv.run(args, checker.str(), "minizinc", "checker.mzc");
  } catch (const LocationException& e) {
    oss << e.loc() << ":" << std::endl;
    oss << e.what() << ": " << e.msg() << std::endl;
  } catch (const Exception& e) {
    std::string what = e.what();
    oss << what << (what.empty() ? "" : ": ") << e.msg() << std::endl;
  } catch (const exception& e) {
    oss << e.what() << std::endl;
  } catch (...) {
    oss << "  UNKNOWN EXCEPTION." << std::endl;
  }

#else
  oss << "% statistics checking not supported (need built-in Gecode)" << std::endl;
#endif
}

bool Solns2Out::__evalOutput(ostream& fout) {
  if (nullptr != outputExpr) {
    pEnv->envi().evalOutput(fout);
  }
  return true;
}

bool Solns2Out::evalStatus(SolverInstance::Status status) {
  if (_opt.flag_canonicalize) {
    __evalOutputFinal(_opt.flag_output_flush);
  }
  __evalStatusMsg(status);
  fStatusPrinted = true;
  return true;
}

bool Solns2Out::__evalOutputFinal(bool /*b*/) {
  /// Print the canonical list
  for (const auto& sol : sSolsCanon) {
    if ((!_opt.solution_comma.empty()) && &sol != &*sSolsCanon.begin()) {
      getOutput() << _opt.solution_comma << '\n';
    }
    getOutput() << sol;
    if (!_opt.solution_separator.empty()) {
      getOutput() << _opt.solution_separator << '\n';
    }
  }
  return true;
}

bool Solns2Out::__evalStatusMsg(SolverInstance::Status status) {
  std::map<SolverInstance::Status, string> stat2msg;
  stat2msg[SolverInstance::OPT] = _opt.search_complete_msg;
  stat2msg[SolverInstance::UNSAT] = _opt.unsatisfiable_msg;
  stat2msg[SolverInstance::UNBND] = _opt.unbounded_msg;
  stat2msg[SolverInstance::UNSATorUNBND] = _opt.unsatorunbnd_msg;
  stat2msg[SolverInstance::UNKNOWN] = _opt.unknown_msg;
  stat2msg[SolverInstance::ERROR] = _opt.error_msg;
  stat2msg[SolverInstance::NONE] = "";
  auto it = stat2msg.find(status);
  if (stat2msg.end() != it) {
    getOutput() << comments;
    if (!it->second.empty()) {
      getOutput() << it->second << '\n';
    }
    if (_opt.flag_output_time) {
      getOutput() << "% time elapsed: " << starttime.stoptime() << "\n";
    }
    if (_opt.flag_output_flush) {
      getOutput().flush();
    }
    Solns2Out::status = status;
  } else {
    getOutput() << comments;
    if (_opt.flag_output_flush) {
      getOutput().flush();
    }
    MZN_ASSERT_HARD_MSG(SolverInstance::SAT == status,  // which is ignored
                        "solns2out_base: undefined solution status code " << status);
    Solns2Out::status = SolverInstance::SAT;
  }
  comments = "";
  return true;
}

void Solns2Out::init() {
  declmap.clear();
  for (auto& i : *getModel()) {
    if (auto* oi = i->dyn_cast<OutputI>()) {
      outputExpr = oi->e();
    } else if (auto* vdi = i->dyn_cast<VarDeclI>()) {
      if (vdi->e()->id()->idn() == -1 && vdi->e()->id()->v() == "_mzn_solution_checker") {
        checkerModel = eval_string(getEnv()->envi(), vdi->e()->e());
        if (!checkerModel.empty() && checkerModel[0] == '@') {
          checkerModel = FileUtils::decodeBase64(checkerModel);
          FileUtils::inflateString(checkerModel);
        }
      } else if (vdi->e()->id()->idn() == -1 && vdi->e()->id()->v() == "_mzn_stats_checker") {
        statisticsCheckerModel = eval_string(getEnv()->envi(), vdi->e()->e());
        if (!statisticsCheckerModel.empty() && statisticsCheckerModel[0] == '@') {
          statisticsCheckerModel = FileUtils::decodeBase64(statisticsCheckerModel);
          FileUtils::inflateString(statisticsCheckerModel);
        }
      } else {
        GCLock lock;
        declmap.insert(make_pair(vdi->e()->id()->str(), DE(vdi->e(), vdi->e()->e())));
      }
    }
  }

  /// Main output file
  if (nullptr == pOut) {
    if (!_opt.flag_output_file.empty()) {
      pOut.reset(new ofstream(FILE_PATH(_opt.flag_output_file)));
      MZN_ASSERT_HARD_MSG(pOut.get(),
                          "solns2out_base: could not allocate stream object for file output into "
                              << _opt.flag_output_file);
      checkIOStatus(pOut->good(), _opt.flag_output_file);
    }
  }
  /// Non-canonical output
  if (_opt.flag_canonicalize && (!_opt.flag_output_noncanonical.empty())) {
    pOfs_non_canon.reset(new ofstream(FILE_PATH(_opt.flag_output_noncanonical)));
    MZN_ASSERT_HARD_MSG(pOfs_non_canon.get(),
                        "solns2out_base: could not allocate stream object for non-canon output");
    checkIOStatus(pOfs_non_canon->good(), _opt.flag_output_noncanonical, false);
  }
  /// Raw output
  if (!_opt.flag_output_raw.empty()) {
    pOfs_raw.reset(new ofstream(FILE_PATH(_opt.flag_output_raw)));
    MZN_ASSERT_HARD_MSG(pOfs_raw.get(),
                        "solns2out_base: could not allocate stream object for raw output");
    checkIOStatus(pOfs_raw->good(), _opt.flag_output_raw, false);
  }
  /// Assume all options are set before
  nLinesIgnore = _opt.flag_ignore_lines;
}

Solns2Out::Solns2Out(std::ostream& os0, std::ostream& log0, std::string stdlibDir0)
    : os(os0), log(log0), stdlibDir(std::move(stdlibDir0)) {}

Solns2Out::~Solns2Out() {
  getOutput() << comments;
  if (_opt.flag_output_flush) {
    getOutput() << flush;
  }
}

ostream& Solns2Out::getOutput() { return (((pOut.get() != nullptr) && pOut->good()) ? *pOut : os); }

ostream& Solns2Out::getLog() { return log; }

bool Solns2Out::feedRawDataChunk(const char* data) {
  istringstream solstream(data);
  while (solstream.good()) {
    string line;
    getline(solstream, line);
    if (!line_part.empty()) {
      line = line_part + line;
      line_part.clear();
    }
    if (solstream.eof()) {  // wait next chunk
      line_part = line;
      break;  // to get to raw output
    }
    if (!line.empty()) {
      if ('\r' == line.back()) {
        line.pop_back();  // For WIN files
      }
    }
    if (nLinesIgnore > 0) {
      --nLinesIgnore;
      continue;
    }
    if (mapInputStatus.empty()) {
      createInputMap();
    }
    auto it = mapInputStatus.find(line);
    if (mapInputStatus.end() != it) {
      if (SolverInstance::SAT == it->second) {
        parseAssignments(solution);
        evalOutput();
      } else {
        evalStatus(it->second);
      }
    } else {
      solution += line + '\n';
      if (_opt.flag_output_comments) {
        std::istringstream iss(line);
        char c = '_';
        iss >> skipws >> c;
        if (iss.good() && '%' == c) {
          // Feed comments directly
          getOutput() << line << '\n';
          if (_opt.flag_output_flush) {
            getOutput().flush();
          }
          if (pOfs_non_canon.get() != nullptr) {
            if (pOfs_non_canon->good()) {
              (*pOfs_non_canon) << line << '\n';
            }
          }
          if (line.substr(0, 13) == "%%%mzn-stat: " && line.size() > 13) {
            if (line.substr(13, 6) == "nodes=") {
              std::istringstream iss(line.substr(19));
              int n_nodes;
              iss >> n_nodes;
              _stats.nNodes = n_nodes;
            } else if (line.substr(13, 9) == "failures=") {
              std::istringstream iss(line.substr(22));
              int n_failures;
              iss >> n_failures;
              _stats.nFails = n_failures;
            }
          }
        }
      }
    }
  }
  if (pOfs_raw.get() != nullptr) {
    (*pOfs_raw.get()) << data;
    if (_opt.flag_output_flush) {
      pOfs_raw->flush();
    }
  }
  return true;
}

void Solns2Out::createInputMap() {
  mapInputStatus[_opt.search_complete_msg_00] = SolverInstance::OPT;
  mapInputStatus[_opt.solution_separator_00] = SolverInstance::SAT;
  mapInputStatus[_opt.unsatisfiable_msg_00] = SolverInstance::UNSAT;
  mapInputStatus[_opt.unbounded_msg_00] = SolverInstance::UNBND;
  mapInputStatus[_opt.unsatorunbnd_msg_00] = SolverInstance::UNSATorUNBND;
  mapInputStatus[_opt.unknown_msg_00] = SolverInstance::UNKNOWN;
  mapInputStatus[_opt.error_msg] = SolverInstance::ERROR;
}

void Solns2Out::printStatistics(ostream& os) {
  os << "%%%mzn-stat: nSolutions=" << _stats.nSolns << "\n";
  if (!statisticsCheckerModel.empty()) {
    std::ostringstream oss;
    checkStatistics(oss);
    os << "%%%mzn-stat: statisticsCheck=\"" << Printer::escapeStringLit(oss.str()) << "\"\n";
  }
  os << "%%%mzn-stat-end\n";
}
