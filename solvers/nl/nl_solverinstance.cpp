/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <minizinc/file_utils.hh>
#include <minizinc/process.hh>
#include <minizinc/solvers/nl/nl_file.hh>
#include <minizinc/solvers/nl/nl_solreader.hh>
#include <minizinc/solvers/nl/nl_solverinstance.hh>

#include <cstdio>
#include <cstring>
#include <fstream>

using namespace std;

namespace MiniZinc {

/** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
 * **/
// Solver Factory

NLSolverFactory::NLSolverFactory() {
  SolverConfig sc("org.minizinc.mzn-nl",
                  MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
  sc.name("Generic Non Linear driver");
  sc.mznlibVersion(1);
  sc.description("MiniZinc generic Non Linear solver plugin");
  sc.supportsFzn(false);
  sc.supportsNL(true);
  sc.requiredFlags({"--nl-cmd"});
  sc.tags({"__internal__"});
  SolverConfigs::registerBuiltinSolver(sc);
}

string NLSolverFactory::getDescription(SolverInstanceBase::Options* /*opt*/) {
  string v = "NL solver plugin, compiled  " __DATE__ "  " __TIME__;
  return v;
}

string NLSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return MZN_VERSION_MAJOR;
}

string NLSolverFactory::getId() { return "org.minizinc.mzn-nl"; }

void NLSolverFactory::printHelp(ostream& os) {
  os << "MZN-NL plugin options" << std::endl
     << "  --nl-cmd , --nonlinear-cmd <exe>\n     The backend solver filename.\n"
     << "  --nl-flags <options>, --backend-flags <options>\n"
        "     Specify option to be passed to the NL solver.\n"
     << "  --nl-flag <option>, --backend-flag <option>\n"
        "     As above, but for a single option string that needs to be quoted in a shell.\n"
     << "  --hexafloat\n     Use hexadecimal format when communicating floating points with the "
        "solver.\n"
     << "  --keepfile\n     Write the nl and sol files next to the input file and don't remove "
        "them.\n"
      // << "  --nl-sigint\n     Send SIGINT instead of SIGTERM.\n"
      // << "  -t <ms>, --solver-time-limit <ms>, --fzn-time-limit <ms>\n     Set time limit (in
      // milliseconds) for solving.\n"
      // << "  -p <n>, --parallel <n>\n     Use <n> threads during search. The default is
      // solver-dependent.\n"
      // << "  -r <n>, --seed <n>, --random-seed <n>\n     For compatibility only: use solver flags
      // instead.\n"
      ;
}

SolverInstanceBase::Options* NLSolverFactory::createOptions() { return new NLSolverOptions; }

SolverInstanceBase* NLSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                SolverInstanceBase::Options* opt) {
  return new NLSolverInstance(env, log, opt);
}

bool NLSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                    std::vector<std::string>& argv, const std::string& workingDir) {
  auto& _opt = static_cast<NLSolverOptions&>(*opt);
  CLOParser cop(i, argv);
  string buffer;

  int nn = -1;
  if (cop.getOption("--nl-cmd --nonlinear-cmd", &buffer)) {
    _opt.nlSolver = buffer;
  } else if (cop.getOption("--hexafloat")) {
    _opt.doHexafloat = true;
  } else if (cop.getOption("--nl-flags --backend-flags", &buffer)) {
    auto args = FileUtils::parse_cmd_line(buffer);
    for (const auto& arg : args) {
      _opt.nlFlags.push_back(arg);
    }
  } else if (cop.getOption("--nl-flag --backend-flag", &buffer)) {
    _opt.nlFlags.push_back(buffer);
  } else if (cop.getOption("--keepfile")) {
    _opt.doKeepfile = true;
  } else if (cop.getOption("-s --solver-statistics")) {
    // ignore statistics flags for now
  } else if (cop.getOption("-v --verbose-solving")) {
    _opt.verbose = true;
  } else {
    for (auto& fznf : _opt.nlSolverFlags) {
      if (fznf.t == MZNFZNSolverFlag::FT_ARG && cop.getOption(fznf.n.c_str(), &buffer)) {
        _opt.nlFlags.push_back(fznf.n);
        _opt.nlFlags.push_back(buffer);
        return true;
      }
      if (fznf.t == MZNFZNSolverFlag::FT_NOARG && cop.getOption(fznf.n.c_str())) {
        _opt.nlFlags.push_back(fznf.n);
        return true;
      }
    }
    return false;
  }

  return true;
}

/** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
 * **/
// Solver Instance

NLSolverInstance::NLSolverInstance(Env& env, std::ostream& log,
                                   SolverInstanceBase::Options* options)
    : SolverInstanceBase(env, log, options), _fzn(env.flat()), _ozn(env.output()) {}

NLSolverInstance::~NLSolverInstance() {}

void NLSolverInstance::processFlatZinc() {}

void NLSolverInstance::resetSolver() {}

SolverInstance::Status NLSolverInstance::solve() {
  // Get the options
  auto& opt = static_cast<NLSolverOptions&>(*_options);

  // --- --- --- Prepare the files
  string file_nl;   // Output for the NL, will be the input for the solver
  string file_sol;  // Ouput of the solver
  FileUtils::TmpDir* tmpdir = nullptr;

  if (opt.doKeepfile) {
    // Keep file: output next to original file
    ASTString file_mzn = _env.envi().originalModel != nullptr
                             ? _env.envi().originalModel->filepath()
                             : _env.envi().model->filepath();
    string file_base = std::string(file_mzn.substr(0, file_mzn.findLastOf('.')));
    file_nl = file_base + ".nl";
    file_sol = file_base + ".sol";
  } else {
    // Don't keep file: create a temp directory
    tmpdir = new FileUtils::TmpDir();
    file_nl = tmpdir->name() + "/model.nl";
    file_sol = tmpdir->name() + "/model.sol";
  }
  std::ofstream outfile(FILE_PATH(file_nl));
  // Configure floating point output
  if (opt.doHexafloat) {
    outfile << hexfloat;
  } else {
    outfile.precision(numeric_limits<double>::digits10 + 2);
  }

  // --- --- --- Result of the try/catch block
  // Use to talk back to minizinc
  auto* out = getSolns2Out();
  // Manage status
  int exitStatus = -1;

  // --- --- --- All the NL operations in one try/catch
  try {
    // --- --- --- Create the NL data
    // Analyse the variable declarations
    for (VarDeclIterator it = _fzn->vardecls().begin(); it != _fzn->vardecls().end(); ++it) {
      if (!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }
    // Analyse the contraints
    for (ConstraintIterator it = _fzn->constraints().begin(); it != _fzn->constraints().end();
         ++it) {
      if (!it->removed()) {
        Item& item = *it;
        analyse(&item);
      }
    }
    // Analyse the goal
    analyse(_fzn->solveItem());
    // Phase 2
    _nlFile.phase2();
    // Print to the files
    _nlFile.printToStream(outfile);

    // --- --- --- Call the solver
    NLSolns2Out s2o = NLSolns2Out(out, _nlFile, opt.verbose);
    vector<string> cmd_line;

    if (opt.nlSolver.empty()) {
      delete tmpdir;
      tmpdir = nullptr;
      outfile.close();
      throw InternalError("No NL solver specified");
    }

    cmd_line.push_back(opt.nlSolver);
    cmd_line.push_back(file_nl);
    cmd_line.emplace_back("-AMPL");

    for (const auto& arg : opt.nlFlags) {
      cmd_line.push_back(arg);
    }

    Process<NLSolns2Out> proc(cmd_line, &s2o, 0, true);
    exitStatus = proc.run();

    if (exitStatus == 0) {
      // Parse the result
      s2o.parseSolution(file_sol);
    }

  } catch (const NLException& e) {
    out->getLog() << e.what();
    exitStatus = -2;
  }

  // --- --- --- Cleanup and exit
  delete tmpdir;
  tmpdir = nullptr;
  outfile.close();
  return exitStatus == 0 ? out->status : Status::ERROR;
}

// Unused
Expression* NLSolverInstance::getSolutionValue(Id* id) {
  assert(false);
  return nullptr;
}

/** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** *** ***
 * **/
/** Analyse an item
 * An item is a "top node" in the ast.
 * In flatzinc, we can only have the cases 'variable declaration' and 'constraint'.
 */
void NLSolverInstance::analyse(const Item* i) {
  // Guard
  if (i == nullptr) {
    return;
  }

  // Switch on the id of item
  switch (i->iid()) {
    case Item::II_INC: {
      should_not_happen("include \"" << i->cast<IncludeI>()->f() << "\")");
    } break;

    // Case of the variable declaration.
    // Because it is a variable declaration, the expression associated to the item is necessary a
    // VarDecl. From the VarDecl, we can obtain the type and the RHS expression. Use this to analyse
    // further.
    case Item::II_VD: {
      DEBUG_MSG("II_VD: Variable Declaration.");

      const VarDecl& vd = *i->cast<VarDeclI>()->e();
      const TypeInst& ti = *vd.ti()->cast<TypeInst>();
      const Expression& rhs = *vd.e();
      _nlFile.addVarDecl(vd, ti, rhs);
    } break;

    case Item::II_ASN: {
      should_not_happen("item II_ASN should not be present in NL's input.");
    } break;

    // Case of the constraint.
    // Constraint are expressed through builtin calls.
    // Hence, the expression associated to the item must be a E_CALL.
    case Item::II_CON: {
      DEBUG_MSG("II_CON: Constraint.");
      Expression* e = i->cast<ConstraintI>()->e();
      if (e->eid() == Expression::E_CALL) {
        const Call& c = *e->cast<Call>();
        DEBUG_MSG("     " << c.id() << " ");
        _nlFile.analyseConstraint(c);
      } else {
        DEBUG_MSG("     Contraint is not a builtin call.");
        assert(false);
      }
    } break;

    // Case of the 'solve' directive
    case Item::II_SOL: {
      const SolveI& si = *i->cast<SolveI>();
      _nlFile.addSolve(si.st(), si.e());
    } break;

    case Item::II_OUT: {
      should_not_happen("Item II_OUT should not be present in NL's input.");
    } break;

    case Item::II_FUN: {
      should_not_happen("Item II_FUN should not be present in NL's input.");
    } break;
  }  // END OF SWITCH
}

}  // namespace MiniZinc
