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
     << "  --only-sections <section_1,...section_n>" << std::endl
     << "    Enable only the given comma-separated output sections." << std::endl
     << "  --not-sections <section_1,...section_n>" << std::endl
     << "    Disable the given comma-separated output sections." << std::endl
     << "  --no-flush-output\n    Don't flush output stream after every line." << std::endl;
}

bool Solns2Out::processOption(int& i, std::vector<std::string>& argv,
                              const std::string& workingDir) {
  CLOParser cop(i, argv);
  std::string buffer;
  if (cop.getOption("--ozn-file", &buffer)) {
    initFromOzn(FileUtils::file_path(buffer, workingDir));
  } else if (cop.getOption("-o --output-to-file", &buffer)) {
    opt.flagOutputFile = buffer;
  } else if (cop.getOption("--no-flush-output")) {
    opt.flagOutputFlush = false;
  } else if (cop.getOption("--no-output-comments")) {
    opt.flagOutputComments = false;
  } else if (cop.getOption("-i --ignore-lines --ignore-leading-lines",
                           &opt.flagIgnoreLines)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--output-time")) {
    opt.flagOutputTime = true;
  } else if (cop.getOption("--soln-sep --soln-separator --solution-separator",
                           &opt.solutionSeparator)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--soln-comma --solution-comma",
                           &opt.solutionComma)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--unsat-msg --unsatisfiable-msg",
                           &opt.unsatisfiableMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--unbounded-msg",
                           &opt.unboundedMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--unsatorunbnd-msg",
                           &opt.unsatorunbndMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--unknown-msg", &opt.unknownMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--error-msg", &opt.errorMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--search-complete-msg",
                           &opt.searchCompleteMsg)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--unique")) {
    opt.flagUnique = true;
  } else if (cop.getOption("--non-unique")) {
    opt.flagUnique = false;
  } else if (cop.getOption("-c --canonicalize")) {
    opt.flagCanonicalize = true;
  } else if (cop.getOption("--output-non-canonical --output-non-canon",
                           &opt.flagOutputNoncanonical)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--output-raw",
                           &opt.flagOutputRaw)) {  // NOLINT: Allow repeated empty if
    // Parsed by reference
  } else if (cop.getOption("--only-sections", &opt.onlySectionsString)) {
    std::stringstream ss(opt.onlySectionsString);
    while (ss.good()) {
      std::string section;
      getline(ss, section, ',');
      opt.onlySections.insert(section);
    }
  } else if (cop.getOption("--not-sections", &opt.notSectionsString)) {
    std::stringstream ss(opt.notSectionsString);
    while (ss.good()) {
      std::string section;
      getline(ss, section, ',');
      opt.notSections.insert(section);
    }
  } else if (cop.getOption("--is-checker")) {
    opt.flagIsChecker = true;
  } else if (opt.flagStandaloneSolns2Out) {
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
  _env = pE;
  _includePaths.push_back(_stdlibDir + "/std/");
  init();
  return true;
}

void Solns2Out::initFromOzn(const std::string& filename) {
  std::vector<string> filenames(1, filename);

  _includePaths.push_back(_stdlibDir + "/std/");

  for (auto& includePath : _includePaths) {
    if (!FileUtils::directory_exists(includePath)) {
      std::cerr << "solns2out: cannot access include directory " << includePath << "\n";
      std::exit(EXIT_FAILURE);
    }
  }

  {
    _env = new Env();
    std::stringstream errstream;
    if ((_outputModel = parse(*_env, filenames, std::vector<std::string>(), "", "", _includePaths,
                              {}, false, false, false, false, errstream)) != nullptr) {
      std::vector<TypeError> typeErrors;
      _env->model(_outputModel);
      MZN_ASSERT_HARD_MSG(_env, "solns2out: could not allocate Env");
      _envGuard.reset(_env);
      MiniZinc::typecheck(*_env, _outputModel, typeErrors, false, false);
      MiniZinc::register_builtins(*_env);
      _env->envi().swapOutput();
      init();
    } else {
      throw Error(errstream.str());
    }
  }
}

Solns2Out::DE& Solns2Out::findOutputVar(const ASTString& name) {
  declNewOutput();
  auto it = _declmap.find(name);
  MZN_ASSERT_HARD_MSG(_declmap.end() != it, "solns2out_base: unexpected id in output: " << name);
  return it->second;
}

void Solns2Out::restoreDefaults() {
  for (auto& i : *getModel()) {
    if (auto* vdi = i->dynamicCast<VarDeclI>()) {
      if (vdi->e()->id()->idn() != -1 || (vdi->e()->id()->v() != "_mzn_solution_checker" &&
                                          vdi->e()->id()->v() != "_mzn_stats_checker")) {
        GCLock lock;
        auto& de = findOutputVar(vdi->e()->id()->str());
        vdi->e()->e(de.second());
        vdi->e()->evaluated(false);
      }
    }
  }
  _fNewSol2Print = false;
}

void Solns2Out::parseAssignments(string& solution) {
  unique_ptr<Model> sm(parse_from_string(*_env, solution, "solution received from solver",
                                         _includePaths, false, true, false, false, _log));
  if (sm == nullptr) {
    throw Error("solns2out_base: could not parse solution");
  }
  solution = "";
  for (unsigned int i = 0; i < sm->size(); i++) {
    if (auto* ai = (*sm)[i]->dynamicCast<AssignI>()) {
      auto& de = findOutputVar(ai->id());
      if (!ai->e()->isa<BoolLit>() && !ai->e()->isa<IntLit>() && !ai->e()->isa<FloatLit>()) {
        Type de_t = de.first->type();
        de_t.cv(false);
        ai->e()->type(de_t);
      }
      ai->decl(de.first);
      typecheck(*_env, getModel(), ai);
      if (Call* c = ai->e()->dynamicCast<Call>()) {
        // This is an arrayXd call, make sure we get the right builtin
        assert(c->arg(c->argCount() - 1)->isa<ArrayLit>());
        for (unsigned int i = 0; i < c->argCount(); i++) {
          c->arg(i)->type(Type::parsetint());
        }
        c->arg(c->argCount() - 1)->type(de.first->type());
        c->decl(getModel()->matchFn(_env->envi(), c, false));
      }
      de.first->e(ai->e());
    }
  }
  declNewOutput();
}

void Solns2Out::declNewOutput() {
  _fNewSol2Print = true;
  status = SolverInstance::SAT;
}

void Solns2Out::printSolution(std::istream& sol, std::ostream& os, bool outputTime) {
  if (opt.flagEncapsulateJSON) {
    std::string t = opt.flagIsChecker ? "checker" : "solution";
    os << "{\"type\": \"" << t << "\", \"output\": ";
    std::string line;
    while (std::getline(sol, line)) {
      // Remove line breaks from JSON object
      os << line;
    }
    if (outputTime) {
      os << ", \"time\": " << _starttime.ms().count();
    }
    os << "}\n";
  } else {
    os << sol.rdbuf();
    os.clear();  // Writing sol.rdbuf() sets the fail bit if the solution was empty, so clear it
    if (outputTime) {
      os << "% time elapsed: " << _starttime.stoptime() << "\n";
    }
    if (!opt.solutionSeparator.empty()) {
      os << opt.solutionSeparator << '\n';
    }
  }
  if (opt.flagOutputFlush) {
    os.flush();
  }
}

void Solns2Out::parseStatistics(const std::string& stats, std::ostream& os) {
  unique_ptr<Model> sm(parse_from_string(*_env, stats, "statistics received from solver",
                                         _includePaths, false, true, false, false, _log));
  if (sm == nullptr) {
    throw Error("solns2out_base: could not parse statistics");
  }
  StatisticsStream ss(os, opt.flagEncapsulateJSON);
  for (unsigned int i = 0; i < sm->size(); i++) {
    if (auto* ai = (*sm)[i]->dynamicCast<AssignI>()) {
      ss.add(ai->id().c_str(), *(ai->e()));
    }
  }
}

bool Solns2Out::evalOutput() {
  if (!_fNewSol2Print) {
    return true;
  }

  stringstream oss;
  if (!_checkerModel.empty()) {
    auto& checkerStream = _env->envi().checkerOutput;
    checkerStream.clear();
    checkerStream.str("");
    checkSolution(checkerStream);
  }
  if (!evalOutputInternal(oss)) {
    return false;
  }
  bool fNew = true;
  if (opt.flagUnique || opt.flagCanonicalize) {
    auto res = _sSolsCanon.insert(oss.str());
    if (!res.second) {  // repeated solution
      fNew = false;
    }
  }
  if (fNew) {
    {
      auto& checkerStream = _env->envi().checkerOutput;
      checkerStream.flush();
      std::string line;
      if (std::getline(checkerStream, line)) {
        if (opt.flagEncapsulateJSON) {
          _os << line << "\n";
        } else {
          _os << "% Solution checker report:\n";
          _os << "% " << line << "\n";
          while (std::getline(checkerStream, line)) {
            _os << "% " << line << "\n";
          }
        }
      }
    }
    ++stats.nSolns;
    if (opt.flagCanonicalize) {
      if (_outStreamNonCanon != nullptr && _outStreamNonCanon->good()) {
        printSolution(oss, *_outStreamNonCanon, opt.flagOutputTime);
      }
      if (opt.flagOutputTime) {
        // Print time as we get solutions
        if (opt.flagEncapsulateJSON) {
          getOutput() << "{\"type\": \"time\", \"time\": " << _starttime.ms().count() << "}\n";
        } else {
          getOutput() << "% time elapsed: " << _starttime.stoptime() << "\n";
        }
      }
    } else {
      if ((!opt.solutionComma.empty()) && stats.nSolns > 1) {
        getOutput() << opt.solutionComma << '\n';
      }

      printSolution(oss, getOutput(), opt.flagOutputTime);
    }
  }
  restoreDefaults();  // cleans data. evalOutput() should not be called again w/o assigning new
                      // data.
  return true;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static): Appears static without Gecode
void Solns2Out::checkSolution(std::ostream& oss) {
#ifdef HAS_GECODE

  std::ostringstream checker;
  checker << _checkerModel;
  {
    GCLock lock;
    for (auto& i : *getModel()) {
      if (auto* vdi = i->dynamicCast<VarDeclI>()) {
        if (vdi->e()->ann().contains(Constants::constants().ann.mzn_check_var)) {
          checker << vdi->e()->id()->str() << " = ";
          Expression* e = eval_par(getEnv()->envi(), vdi->e()->e());
          auto* al = e->dynamicCast<ArrayLit>();
          std::vector<Id*> enumids;
          if (Call* cev = vdi->e()->ann().getCall(Constants::constants().ann.mzn_check_enum_var)) {
            auto* enumIdsAl = cev->arg(0)->cast<ArrayLit>();
            for (int j = 0; j < enumIdsAl->size(); j++) {
              enumids.push_back((*enumIdsAl)[j]->dynamicCast<Id>());
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

  MznSolver slv(oss, _log, _starttime);
  slv.s2out.opt.solutionSeparator = "";
  try {
    slv.run(checkerArgs(), checker.str(), "minizinc", "checker.mzc");
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

std::vector<std::string> Solns2Out::checkerArgs() const {
  std::vector<std::string> args({"--solver", "org.minizinc.gecode_presolver", "--is-checker"});
  if (opt.flagEncapsulateJSON) {
    args.emplace_back("--json-stream");
  }
  if (!opt.onlySections.empty()) {
    args.emplace_back("--only-sections");
    args.push_back(opt.onlySectionsString);
  }
  if (!opt.notSections.empty()) {
    args.emplace_back("--not-sections");
    args.push_back(opt.notSectionsString);
  }
  return args;
}

// NOLINTNEXTLINE(readability-convert-member-functions-to-static): Appears static without Gecode
void Solns2Out::checkStatistics(std::ostream& oss) {
#ifdef HAS_GECODE

  std::ostringstream checker;
  checker << _statisticsCheckerModel;
  checker << "mzn_stats_failures = " << stats.nFails << ";\n";
  checker << "mzn_stats_solutions = " << stats.nSolns << ";\n";
  checker << "mzn_stats_nodes = " << stats.nNodes << ";\n";
  checker << "mzn_stats_time = " << _starttime.ms().count() << ";\n";

  MznSolver slv(oss, oss, _starttime);
  slv.s2out.opt.solutionSeparator = "";
  try {
    slv.run(checkerArgs(), checker.str(), "minizinc", "checker.mzc");
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

bool Solns2Out::evalOutputInternal(ostream& fout) {
  if (nullptr != _outputExpr) {
    _env->envi().evalOutput(fout, _log);
  }
  return true;
}

bool Solns2Out::evalStatus(SolverInstance::Status status) {
  if (opt.flagCanonicalize) {
    evalOutputFinalInternal(opt.flagOutputFlush);
  }
  evalStatusMsg(status);
  fStatusPrinted = true;
  return true;
}

bool Solns2Out::evalOutputFinalInternal(bool /*b*/) {
  /// Print the canonical list
  for (const auto& sol : _sSolsCanon) {
    if ((!opt.solutionComma.empty()) && &sol != &*_sSolsCanon.begin()) {
      getOutput() << opt.solutionComma << '\n';
    }
    std::stringstream oss;
    oss << sol;
    printSolution(oss, getOutput(), false);
  }
  return true;
}

bool Solns2Out::evalStatusMsg(SolverInstance::Status status) {
  getOutput() << comments;
  comments = "";

  Solns2Out::status = status;
  std::string label;
  switch (status) {
    case SolverInstance::SAT:
      if (opt.flagOutputFlush) {
        getOutput().flush();
      }
      return true;
    case SolverInstance::OPT: {
      if (opt.flagEncapsulateJSON) {
        bool sat = getEnv()->flat()->solveItem()->st() == SolveI::ST_SAT;
        label = sat ? "ALL_SOLUTIONS" : "OPTIMAL_SOLUTION";
      } else {
        label = opt.searchCompleteMsg;
      }
      break;
    }
    case SolverInstance::UNSAT:
      label = opt.flagEncapsulateJSON ? "UNSATISFIABLE" : opt.unsatisfiableMsg;
      break;
    case SolverInstance::UNBND:
      label = opt.flagEncapsulateJSON ? "UNBOUNDED" : opt.unboundedMsg;
      break;
    case SolverInstance::UNSATorUNBND:
      label = opt.flagEncapsulateJSON ? "UNSAT_OR_UNBOUNDED" : opt.unsatorunbndMsg;
      break;
    case SolverInstance::UNKNOWN:
      label = opt.flagEncapsulateJSON ? "UNKNOWN" : opt.unknownMsg;
      break;
    case SolverInstance::ERROR:
      label = opt.flagEncapsulateJSON ? "ERROR" : opt.errorMsg;
      break;
    case SolverInstance::NONE:
      label = "";
      break;
    default:
      assert(false);
  }
  if (opt.flagEncapsulateJSON) {
    if (label.empty()) {
      getOutput() << "{\"type\": \"time\", \"time\": " << _starttime.ms().count() << "}\n";
    } else {
      getOutput() << "{\"type\": \"status\", \"status\": \"" << label << "\"";
      if (opt.flagOutputTime) {
        getOutput() << ", \"time\": " << _starttime.ms().count();
      }
      getOutput() << "}\n";
    }
  } else {
    if (!label.empty()) {
      getOutput() << label << '\n';
    }
    if (opt.flagOutputTime) {
      getOutput() << "% time elapsed: " << _starttime.stoptime() << "\n";
    }
  }

  if (opt.flagOutputFlush) {
    getOutput().flush();
  }
  return true;
}

void Solns2Out::init() {
  getEnv()->envi().onlySections = &opt.onlySections;
  getEnv()->envi().notSections = &opt.notSections;

  _declmap.clear();
  for (auto& i : *getModel()) {
    if (auto* oi = i->dynamicCast<OutputI>()) {
      _outputExpr = oi->e();
    } else if (auto* vdi = i->dynamicCast<VarDeclI>()) {
      if (vdi->e()->id()->idn() == -1 && vdi->e()->id()->v() == "_mzn_solution_checker") {
        _checkerModel = eval_string(getEnv()->envi(), vdi->e()->e());
        if (!_checkerModel.empty() && _checkerModel[0] == '@') {
          _checkerModel = FileUtils::decode_base64(_checkerModel);
          FileUtils::inflate_string(_checkerModel);
        }
      } else if (vdi->e()->id()->idn() == -1 && vdi->e()->id()->v() == "_mzn_stats_checker") {
        _statisticsCheckerModel = eval_string(getEnv()->envi(), vdi->e()->e());
        if (!_statisticsCheckerModel.empty() && _statisticsCheckerModel[0] == '@') {
          _statisticsCheckerModel = FileUtils::decode_base64(_statisticsCheckerModel);
          FileUtils::inflate_string(_statisticsCheckerModel);
        }
      } else {
        _declmap.insert(make_pair(vdi->e()->id()->str(), DE(vdi->e(), vdi->e()->e())));
      }
    }
  }

  /// Main output file
  if (nullptr == _outStream) {
    if (!opt.flagOutputFile.empty()) {
      _outStream.reset(new ofstream(FILE_PATH(opt.flagOutputFile)));
      MZN_ASSERT_HARD_MSG(_outStream.get(),
                          "solns2out_base: could not allocate stream object for file output into "
                              << opt.flagOutputFile);
      check_io_status(_outStream->good(), opt.flagOutputFile);
    }
  }
  /// Non-canonical output
  if (opt.flagCanonicalize && (!opt.flagOutputNoncanonical.empty())) {
    _outStreamNonCanon.reset(new ofstream(FILE_PATH(opt.flagOutputNoncanonical)));
    MZN_ASSERT_HARD_MSG(_outStreamNonCanon.get(),
                        "solns2out_base: could not allocate stream object for non-canon output");
    check_io_status(_outStreamNonCanon->good(), opt.flagOutputNoncanonical, false);
  }
  /// Raw output
  if (!opt.flagOutputRaw.empty()) {
    _outStreamRaw.reset(new ofstream(FILE_PATH(opt.flagOutputRaw)));
    MZN_ASSERT_HARD_MSG(_outStreamRaw.get(),
                        "solns2out_base: could not allocate stream object for raw output");
    check_io_status(_outStreamRaw->good(), opt.flagOutputRaw, false);
  }
  /// Assume all options are set before
  nLinesIgnore = opt.flagIgnoreLines;
}

Solns2Out::Solns2Out(std::ostream& os0, std::ostream& log0, std::string stdlibDir0)
    : _os(os0), _log(log0), _stdlibDir(std::move(stdlibDir0)) {}

Solns2Out::~Solns2Out() {
  getOutput() << comments;
  if (opt.flagOutputFlush) {
    getOutput() << flush;
  }
}

ostream& Solns2Out::getOutput() {
  return (((_outStream != nullptr) && _outStream->good()) ? *_outStream : _os);
}

ostream& Solns2Out::getLog() { return _log; }

bool Solns2Out::feedRawDataChunk(const char* data) {
  istringstream solstream(data);
  while (solstream.good()) {
    string line;
    getline(solstream, line);
    if (!_linePart.empty()) {
      line = _linePart + line;
      _linePart.clear();
    }
    if (solstream.eof()) {  // wait next chunk
      _linePart = line;
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
    if (_mapInputStatus.empty()) {
      createInputMap();
    }
    auto it = _mapInputStatus.find(line);
    if (_mapInputStatus.end() != it) {
      if (!_stats.empty()) {
        // Didn't have %%%mzn-stat-end, but should print stats anyway at this point
        parseStatistics(_stats, getOutput());
        _stats.clear();
      }
      if (SolverInstance::SAT == it->second) {
        parseAssignments(solution);
        evalOutput();
      } else {
        evalStatus(it->second);
      }
    } else {
      solution += line + '\n';
      if (opt.flagOutputComments) {
        std::istringstream iss(line);
        char c = '_';
        iss >> skipws >> c;
        if (iss.good() && '%' == c) {
          bool is_statistic = line.substr(0, 13) == "%%%mzn-stat: ";
          std::ostringstream message;
          if (opt.flagEncapsulateJSON) {
            if (is_statistic) {
              _stats += line.substr(13) + ";";
            } else if (line == "%%%mzn-stat-end") {
              parseStatistics(_stats, message);
              _stats.clear();
            } else {
              message << "{\"type\": \"comment\", \"comment\": \"" << Printer::escapeStringLit(line)
                      << "\"}\n";
            }
          } else {
            message << line << '\n';
          }
          getOutput() << message.str();
          if (opt.flagOutputFlush) {
            getOutput().flush();
          }
          if (_outStreamNonCanon != nullptr && _outStreamNonCanon->good()) {
            (*_outStreamNonCanon) << message.str();
            if (opt.flagOutputFlush) {
              _outStreamNonCanon->flush();
            }
          }

          if (is_statistic && line.size() > 13) {
            if (line.substr(13, 6) == "nodes=") {
              std::istringstream iss(line.substr(19));
              int n_nodes;
              iss >> n_nodes;
              stats.nNodes = n_nodes;
            } else if (line.substr(13, 9) == "failures=") {
              std::istringstream iss(line.substr(22));
              int n_failures;
              iss >> n_failures;
              stats.nFails = n_failures;
            }
          }
        }
      }
    }
  }
  if (_outStreamRaw != nullptr) {
    *_outStreamRaw << data;
    if (opt.flagOutputFlush) {
      _outStreamRaw->flush();
    }
  }
  return true;
}

void Solns2Out::createInputMap() {
  _mapInputStatus[opt.searchCompleteMsgDef] = SolverInstance::OPT;
  _mapInputStatus[opt.solutionSeparatorDef] = SolverInstance::SAT;
  _mapInputStatus[opt.unsatisfiableMsgDef] = SolverInstance::UNSAT;
  _mapInputStatus[opt.unboundedMsgDef] = SolverInstance::UNBND;
  _mapInputStatus[opt.unsatorunbndMsgDef] = SolverInstance::UNSATorUNBND;
  _mapInputStatus[opt.unknownMsgDef] = SolverInstance::UNKNOWN;
  _mapInputStatus[opt.errorMsgDef] = SolverInstance::ERROR;
}

void Solns2Out::printStatistics(ostream& os) {
  if (!_stats.empty()) {
    // Didn't have %%%mzn-stat-end, but should print stats anyway at this point
    parseStatistics(_stats, getOutput());
    _stats.clear();
  }
  StatisticsStream ss(os, opt.flagEncapsulateJSON);
  ss.add("nSolutions", stats.nSolns);
  if (!_statisticsCheckerModel.empty()) {
    std::ostringstream oss;
    checkStatistics(oss);
    ss.add("statisticsCheck", oss.str());
  }
}
