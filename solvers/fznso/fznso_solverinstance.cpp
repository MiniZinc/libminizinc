/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/config.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/solvers/fznso/fznso_model.hh>
#include <minizinc/solvers/fznso_solverinstance.hh>
#include <minizinc/utils.hh>

#include <atomic>
#include <exception>
#include <map>
#include <memory>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

#include <minizinc/_thirdparty/fznso.hpp>

namespace MiniZinc {

namespace {

std::string_view view(FznsoStr s) {
  return s.ptr == nullptr ? std::string_view{} : std::string_view{s.ptr, s.len};
}

/// The identifiers of the options that MiniZinc drives through its own standard
/// flags rather than exposing as `--<ident>`. See the FZnSO option registry.
struct StdOption {
  const char* ident;
  const char* flag;
};
const StdOption STD_OPTIONS[] = {
    {"all_solutions", "-a"}, {"intermediate", "-i"}, {"fixed_search", "-f"}, {"threads", "-p"},
    {"random_seed", "-r"},   {"time_limit", "-t"},   {"verbose", "-v"},
};

const char* std_flag_for(std::string_view ident) {
  for (const StdOption& o : STD_OPTIONS) {
    if (ident == o.ident) {
      return o.flag;
    }
  }
  return nullptr;
}

/// Libraries stay loaded for the lifetime of the process and are shared between
/// the configuration scan and the solver that is finally run, so that a library
/// is opened at most once.
std::map<std::string, std::shared_ptr<fznso::Library>>& library_cache() {
  static std::map<std::string, std::shared_ptr<fznso::Library>> cache;
  return cache;
}

/// Load the solver library \a name: a path if it looks like one, otherwise a
/// name looked up on the FZnSO search path. Throws on failure.
std::shared_ptr<fznso::Library> load_library(const std::string& name) {
  auto it = library_cache().find(name);
  if (it != library_cache().end()) {
    return it->second;
  }
  std::shared_ptr<fznso::Library> lib;
  bool isPath = name.find('/') != std::string::npos || name.find('\\') != std::string::npos ||
                FileUtils::file_exists(name);
  if (isPath) {
    lib = std::shared_ptr<fznso::Library>(new fznso::Library{name.c_str()});
  } else {
    lib = fznso::Library::find(name);
  }
  library_cache().emplace(name, lib);
  return lib;
}

/// The MiniZinc type a solver declares for a constraint argument.
Type type_from(const FznsoType& t) {
  Type mzn;
  switch (t.base) {
    case FznsoTypeBaseBool:
      mzn.bt(Type::BT_BOOL);
      break;
    case FznsoTypeBaseFloat:
      mzn.bt(Type::BT_FLOAT);
      break;
    case FznsoTypeBaseString:
      mzn.bt(Type::BT_STRING);
      break;
    default:
      mzn.bt(Type::BT_INT);
      break;
  }
  mzn.ti(t.decision ? Type::TI_VAR : Type::TI_PAR);
  mzn.st(t.set_of ? Type::ST_SET : Type::ST_PLAIN);
  mzn.ot(t.opt ? Type::OT_OPTIONAL : Type::OT_PRESENT);
  mzn.dim(t.list_of ? 1 : 0);
  return mzn;
}

SolverConfig::ExtraFlag::FlagType flag_type_from(const FznsoType& t) {
  if (t.set_of || t.list_of) {
    return SolverConfig::ExtraFlag::T_STRING;
  }
  switch (t.base) {
    case FznsoTypeBaseBool:
      return SolverConfig::ExtraFlag::T_BOOL;
    case FznsoTypeBaseFloat:
      return SolverConfig::ExtraFlag::T_FLOAT;
    case FznsoTypeBaseString:
      return SolverConfig::ExtraFlag::T_STRING;
    default:
      return SolverConfig::ExtraFlag::T_INT;
  }
}

/// A declared default, as the `.msc` flag description spells it.
std::string default_of(const FznsoOption& o) {
  fznso::Value v{o.arg_def};
  std::ostringstream oss;
  switch (v.kind()) {
    case FznsoValueBool:
      return v.as_bool() ? "true" : "false";
    case FznsoValueInt:
      oss << v.as_int();
      return oss.str();
    case FznsoValueFloat:
      oss << v.as_float();
      return oss.str();
    case FznsoValueString:
      return std::string{v.as_string()};
    default:
      return {};
  }
}

/// Parse \a text as the value \a t describes. Returns absent for an optional
/// option given `none`, which is how a limit is cleared.
fznso::OwnedValue parse_value(const FznsoType& t, const std::string& text) {
  if (t.opt && (text == "none" || text.empty())) {
    return {};
  }
  switch (t.base) {
    case FznsoTypeBaseBool:
      return fznso::OwnedValue{text != "false" && text != "0"};
    case FznsoTypeBaseFloat:
      return fznso::OwnedValue{std::stod(text)};
    case FznsoTypeBaseString:
      return fznso::OwnedValue{text};
    default:
      return fznso::OwnedValue{static_cast<std::int64_t>(std::stoll(text))};
  }
}

class FznsoSolverOptions : public SolverInstanceBase::Options {
public:
  /// The loaded solver library, or null if `--fznso-lib` was never given.
  std::shared_ptr<fznso::Library> library;
  /// Its name, which is also what a model includes to reach its constraints.
  std::string libraryName;
  /// Option identifier and the textual value to set it to, in the order given.
  std::vector<std::pair<std::string, std::string>> optionValues;
  /// How many solutions to report before stopping; 0 means as many as the
  /// solver offers.
  int numSolutions = 0;
  /// Whether `-a` was given, which lifts the one-solution default that a
  /// satisfaction problem otherwise gets.
  bool allSolutions = false;

  /// The declared option \a ident, or null if the solver has none such.
  const FznsoOption* option(std::string_view ident) const {
    if (!library) {
      return nullptr;
    }
    FznsoOptionList options = library->options();
    for (std::size_t i = 0; i < options.len; i++) {
      if (view(options.options[i].ident) == ident) {
        return &options.options[i];
      }
    }
    return nullptr;
  }

  /// Whether a value was already asked for \a ident on the command line.
  bool isSet(std::string_view ident) const {
    for (const auto& set : optionValues) {
      if (set.first == ident) {
        return true;
      }
    }
    return false;
  }
};

/// Runs the flat model through a dynamically loaded FZnSO solver.
class FznsoSolverInstance : public SolverInstanceBase2<false> {
public:
  FznsoSolverInstance(Env& env, std::ostream& log, SolverInstanceBase::Options* opt)
      : SolverInstanceBase2(env, log, opt) {}

  Status next() override { return SolverInstance::ERROR; }
  void resetSolver() override {}
  void processFlatZinc() override;
  Status solve() override;
  void printStatistics() override;

protected:
  Expression* getSolutionValue(Id* id) override;

private:
  FznsoSolverOptions& opts() { return static_cast<FznsoSolverOptions&>(*_options); }

  /// Report \a message from the solver on the scope it named.
  void message(std::string_view scope, const fznso::Value& value);

  std::unique_ptr<FznsoModel> _model;
  std::optional<fznso::DynSolver> _solver;
  /// The solution currently being reported, valid only inside `on_solution`.
  std::optional<fznso::Solution> _solution;
  /// How many solutions to report before asking the solver to stop; 0 for all.
  int _solutionLimit = 0;
  /// Polled concurrently by the solver, so these must be safe to read from any
  /// thread.
  std::atomic<int> _nSolutions{0};
  std::atomic<bool> _stop{false};
  /// What a callback threw, rethrown once the solver has returned. An exception
  /// must not cross the interface, and the solver would report it as an opaque
  /// failure of its own if it did.
  std::exception_ptr _failure;
};

void FznsoSolverInstance::processFlatZinc() {
  FznsoSolverOptions& o = opts();
  if (!o.library) {
    throw Error("No FZnSO solver library given (use --fznso-lib)");
  }

  std::vector<Expression*> searchAnn;
  SolveI* si = env().flat()->solveItem();
  if (si != nullptr) {
    flattenSearchAnnotations(si->ann(), searchAnn);
  }
  _model = std::unique_ptr<FznsoModel>(new FznsoModel(env(), std::move(searchAnn)));

  // MiniZinc reports one solution of a satisfaction problem unless `-a` or `-n`
  // asks for more, so the search is capped through `should_stop`. An
  // optimisation problem is left to the solver, which reports the optimum, or
  // the improving sequence when `intermediate` is set.
  bool satisfaction = _model->objective_ident().empty();
  if (o.numSolutions > 0) {
    _solutionLimit = o.numSolutions;
  } else if (satisfaction && !o.allSolutions) {
    _solutionLimit = 1;
  }

  // Two defaults differ from the interface's own, because MiniZinc's do: a
  // model's search annotation is honoured unless `-f` says otherwise, and
  // asking a satisfaction problem for more than one solution has to lift the
  // solver's own single-solution default.
  if (o.option("fixed_search") != nullptr && !o.isSet("fixed_search")) {
    o.optionValues.emplace_back("fixed_search", "true");
  }
  if (satisfaction && _solutionLimit != 1 && o.option("all_solutions") != nullptr &&
      !o.isSet("all_solutions")) {
    o.optionValues.emplace_back("all_solutions", "true");
  }

  _solver.emplace(o.library->create_solver());
  for (const auto& set : o.optionValues) {
    const FznsoOption* declared = o.option(set.first);
    if (declared == nullptr) {
      throw Error("Solver does not have an option named `" + set.first + "'");
    }
    fznso::OwnedValue value = parse_value(declared->arg_ty, set.second);
    std::optional<std::string> error = _solver->option_set(set.first, fznso::Value{value});
    if (error) {
      throw Error("Could not set solver option `" + set.first + "': " + *error);
    }
  }
}

void FznsoSolverInstance::message(std::string_view scope, const fznso::Value& value) {
  std::ostringstream text;
  switch (value.kind()) {
    case FznsoValueString:
      text << value.as_string();
      break;
    case FznsoValueInt:
      text << value.as_int();
      break;
    case FznsoValueFloat:
      text << value.as_float();
      break;
    case FznsoValueBool:
      text << (value.as_bool() ? "true" : "false");
      break;
    default:
      return;  // nothing this side can render
  }
  // Scopes are dot-separated from least to most specific, so a prefix match is
  // all it takes to recognise a family without knowing every member.
  if (scope.compare(0, 4, "warn") == 0) {
    env().envi().addWarning(text.str());
  } else if (_options->verbose) {
    _log << "% " << scope << ": " << text.str() << std::endl;
  }
}

SolverInstanceBase::Status FznsoSolverInstance::solve() {
  // A callback must not throw across the interface, so anything it raises is
  // held here, the search asked to stop, and the exception rethrown below.
  auto guard = [&](auto&& body) {
    if (_stop) {
      return;
    }
    try {
      body();
    } catch (...) {
      _failure = std::current_exception();
      _stop = true;
    }
  };

  fznso::Status status = _solver->run(
      *_model,
      [&](fznso::Solution solution) {
        guard([&] {
          _solution = solution;
          printSolution();  // assigns to the output, and prints per-solution statistics
          _solution.reset();
          _nSolutions++;
        });
      },
      [&](std::string_view scope, fznso::Value value) { guard([&] { message(scope, value); }); },
      [&]() { return _stop || (_solutionLimit > 0 && _nSolutions >= _solutionLimit); });

  if (_failure) {
    std::rethrow_exception(_failure);
  }

  switch (status.kind) {
    case fznso::Status::Kind::Error:
      throw Error("FZnSO solver failed: " + status.error);
    case fznso::Status::Kind::Complete:
      // The whole search space was explored: an optimisation problem's last
      // solution is optimal, and a satisfaction problem's search is complete.
      return _nSolutions > 0 ? SolverInstance::OPT : SolverInstance::UNSAT;
    case fznso::Status::Kind::Incomplete:
      if (_nSolutions > 0) {
        return SolverInstance::SAT;
      }
      _statusReason = SolverInstance::SR_LIMIT;
      return SolverInstance::UNKNOWN;
  }
  return SolverInstance::ERROR;
}

Expression* FznsoSolverInstance::getSolutionValue(Id* id) {
  VarDecl* vd = id->decl();
  int index = vd != nullptr ? FznsoModel::decisionOf(vd) : -1;
  if (index < 0) {
    // A parameter, or a variable the flattener fixed to a literal: its value is
    // in the model rather than in the solution.
    return vd != nullptr ? vd->e() : nullptr;
  }
  fznso::Value value = (*_solution)[fznso::Decision{static_cast<std::size_t>(index)}];
  switch (value.kind()) {
    case FznsoValueInt:
      return IntLit::a(IntVal(value.as_int()));
    case FznsoValueBool:
      return Constants::constants().boollit(value.as_bool());
    case FznsoValueFloat:
      return FloatLit::a(FloatVal(value.as_float()));
    case FznsoValueSetInt: {
      std::vector<IntSetVal::Range> ranges;
      ranges.reserve(value.size());
      for (std::size_t i = 0; i < value.size(); i++) {
        fznso::Range<std::int64_t> r = value.int_range(i);
        ranges.emplace_back(IntVal(r.min), IntVal(r.max));
      }
      return new SetLit(Location().introduce(), IntSetVal::a(ranges));
    }
    default: {
      std::ostringstream ss;
      ss << "Solver reported no value for variable " << *id;
      throw Error(ss.str());
    }
  }
}

void FznsoSolverInstance::printStatistics() {
  if (!_solver) {
    return;
  }
  auto* s2out = getSolns2Out();
  StatisticsStream ss(s2out->getOutput(), s2out->opt.flagEncapsulateJSON);
  FznsoStatisticList declared = opts().library->statistics();
  for (std::size_t i = 0; i < declared.len; i++) {
    // While a solution is being reported, the counters that go with it are the
    // ones worth printing; the solver's own are only meaningful once it has
    // returned.
    bool fromSolution = _solution.has_value() && declared.stats[i].solution;
    if (!fromSolution && !declared.stats[i].solver) {
      continue;
    }
    std::string name{view(declared.stats[i].ident)};
    fznso::Value value = fromSolution ? _solution->statistic(name) : _solver->statistic(name);
    switch (value.kind()) {
      case FznsoValueInt:
        ss.add(name, static_cast<long long>(value.as_int()));
        break;
      case FznsoValueFloat:
        ss.add(name, value.as_float());
        break;
      case FznsoValueBool:
        ss.add(name, value.as_bool() ? 1 : 0);
        break;
      case FznsoValueString:
        ss.add(name, std::string{value.as_string()});
        break;
      default:
        break;
    }
  }
}

}  // namespace

// --- Factory --------------------------------------------------------------

FZNSOSolverFactory::FZNSOSolverFactory() {
  SolverConfig sc("org.minizinc.mzn-fznso",
                  MZN_VERSION_MAJOR "." MZN_VERSION_MINOR "." MZN_VERSION_PATCH);
  sc.name("Generic FZnSO driver");
  sc.mznlibVersion(1);
  sc.description("MiniZinc generic dynamically loaded solver plugin");
  sc.requiredFlags({"--fznso-lib"});
  sc.tags({"__internal__"});
  SolverConfigs::registerBuiltinSolver(sc);
}

SolverInstanceBase* FZNSOSolverFactory::doCreateSI(Env& env, std::ostream& log,
                                                   SolverInstanceBase::Options* opt) {
  return new FznsoSolverInstance(env, log, opt);
}

SolverInstanceBase::Options* FZNSOSolverFactory::createOptions() { return new FznsoSolverOptions; }

std::string FZNSOSolverFactory::getDescription(SolverInstanceBase::Options* /*opt*/) {
  return "FZnSO solver plugin, compiled  " __DATE__ "  " __TIME__;
}

std::string FZNSOSolverFactory::getVersion(SolverInstanceBase::Options* /*opt*/) {
  return MZN_VERSION_MAJOR;
}

std::string FZNSOSolverFactory::getId() { return "org.minizinc.mzn-fznso"; }

void FZNSOSolverFactory::printHelp(std::ostream& os) {
  os << "MZN-FZnSO plugin options:" << std::endl
     << "  --fznso-lib <name|path>\n     the solver library to load. A bare name is looked up on "
        "the FZnSO search path.\n"
     << "  -n, --num-solutions <n>\n     stop after <n> solutions.\n"
     << "\n  Every option the loaded solver declares is available as --<option>; see\n"
        "  --help <solver id> for the list a particular solver offers.\n";
}

bool FZNSOSolverFactory::processOption(SolverInstanceBase::Options* opt, int& i,
                                       std::vector<std::string>& argv,
                                       const std::string& /*workingDir*/) {
  auto& o = static_cast<FznsoSolverOptions&>(*opt);
  CLOParser cop(i, argv);
  std::string buffer;
  int nn = -1;

  // Setting an option MiniZinc drives itself is only meaningful when the solver
  // declared it; the flag is accepted and dropped otherwise, matching how the
  // FlatZinc driver treats a backend that does not support one.
  auto set = [&](const char* ident, const std::string& value) {
    if (o.option(ident) != nullptr) {
      o.optionValues.emplace_back(ident, value);
    }
    return true;
  };

  if (cop.getOption("--fznso-lib", &buffer)) {
    try {
      o.library = load_library(buffer);
      o.libraryName = fznso::detail::name_from_path(buffer.c_str());
    } catch (const std::exception& e) {
      throw Error("Could not load the FZnSO solver library `" + buffer + "': " + e.what());
    }
    return true;
  }
  if (cop.getOption("-n --num-solutions", &nn)) {
    o.numSolutions = nn;
    return true;
  }
  if (cop.getOption("-s --solver-statistics")) {
    o.printStatistics = true;
    return true;
  }
  if (cop.getOption("-a --all --all-solns --all-solutions")) {
    o.allSolutions = true;
    return set("all_solutions", "true");
  }
  if (cop.getOption("-i --intermediate --intermediate-solutions")) {
    return set("intermediate", "true");
  }
  if (cop.getOption("-f --free-search")) {
    return set("fixed_search", "false");
  }
  if (cop.getOption("-p --parallel", &nn)) {
    return set("threads", std::to_string(nn));
  }
  if (cop.getOption("-r --seed --random-seed", &buffer)) {
    return set("random_seed", buffer);
  }
  if (cop.getOption("-t --solver-time-limit", &nn)) {
    return set("time_limit", std::to_string(nn));
  }
  if (cop.getOption("-v --verbose-solving")) {
    o.verbose = true;
    return set("verbose", "true");
  }

  // Every other option the solver declares, as `--<ident>`. Nothing is declared
  // in the configuration file: the library is the only source.
  if (!o.library) {
    return false;
  }
  FznsoOptionList options = o.library->options();
  for (std::size_t k = 0; k < options.len; k++) {
    const FznsoOption& declared = options.options[k];
    std::string ident{view(declared.ident)};
    if (std_flag_for(ident) != nullptr) {
      continue;
    }
    std::string names("--" + ident);
    bool isBool = !declared.arg_ty.list_of && !declared.arg_ty.set_of &&
                  declared.arg_ty.base == FznsoTypeBaseBool;
    if (isBool) {
      if (cop.getOption(names.c_str())) {
        o.optionValues.emplace_back(ident, "true");
        return true;
      }
    } else if (cop.getOption(names.c_str(), &buffer)) {
      o.optionValues.emplace_back(ident, buffer);
      return true;
    }
  }
  return false;
}

void FZNSOSolverFactory::finaliseSolverConfigs(SolverConfigs& solverConfigs) {
  for (SolverConfig& sc : solverConfigs.allConfigs()) {
    if (sc.inputType() != SolverConfig::O_FZNSO || !sc.extraFlags().empty()) {
      continue;  // not ours, or already described
    }
    std::shared_ptr<fznso::Library> lib;
    try {
      lib =
          load_library(sc.executableResolved().empty() ? sc.executable() : sc.executableResolved());
    } catch (const std::exception&) {
      continue;  // report the failure when the solver is actually selected
    }

    // MiniZinc drives `-n` and `-s` itself, whatever the solver declares.
    std::vector<std::string> stdFlags{"-n", "-s"};
    std::vector<SolverConfig::ExtraFlag> extraFlags;
    FznsoOptionList options = lib->options();
    for (std::size_t i = 0; i < options.len; i++) {
      const FznsoOption& o = options.options[i];
      std::string ident{view(o.ident)};
      if (const char* flag = std_flag_for(ident)) {
        stdFlags.emplace_back(flag);
        continue;
      }
      extraFlags.emplace_back("--" + ident, "solver option `" + ident + "'",
                              flag_type_from(o.arg_ty), std::vector<std::string>{}, default_of(o));
    }
    sc.stdFlags(stdFlags);
    sc.extraFlags(extraFlags);
  }
}

void FZNSOSolverFactory::configureFlattener(SolverInstanceBase::Options* opt, Flattener& flt) {
  auto& o = static_cast<FznsoSolverOptions&>(*opt);
  if (!o.library) {
    return;
  }

  // The overlay reshapes the declarations a global lowers to so that a solver
  // can actually match them: flat one-based arrays and explicit index offsets.
  // It is two layers, searched in this order and both after any configured
  // `mznlib`, which therefore still wins, and before the standard library.
  //
  //   - `fznso_constraints` holds one `fzn_<ident>.mzn` per constraint the
  //     registry defines. That is the whole of what a solver writer overrides,
  //     and each file is named after the constraint rather than after whatever
  //     MiniZinc happens to call it.
  //   - `fznso_rewrite` turns the standard library's own spelling into the
  //     registry's. Nothing in it is a solver's concern.
  //
  // A solver with no set variables gets one more, ahead of the other two: the
  // set constraints written over arrays of Booleans. It shadows the constraints
  // layer file by file, which is why it cannot be `std/nosets.mzn` — that
  // rewrites the builtins, and `fznso_rewrite` already does, so the two would
  // be a duplicate definition rather than an override.
  FznsoTypeList decisions = o.library->decision_types();
  bool hasSets = false;
  for (std::size_t i = 0; i < decisions.len; i++) {
    hasSets = hasSets || (decisions.types[i].set_of && decisions.types[i].decision);
  }
  if (!hasSets) {
    flt.addLibraryPath(flt.stdLibDir() + "/fznso_nosets");
  }
  flt.addLibraryPath(flt.stdLibDir() + "/fznso_constraints");
  flt.addLibraryPath(flt.stdLibDir() + "/fznso_rewrite");
  // `include "<solver>.mzn";` reaches the constraints this solver implements
  // beyond the ones MiniZinc knows.
  flt.setSolverInclude(o.libraryName);

  // A constraint the solver declares is native, so the standard library's
  // decomposition of the matching `fzn_<ident>` must not be used.
  auto native = std::make_shared<NativePredicates>();
  native->libraryDirs = {flt.stdLibDir() + "/std/", flt.stdLibDir() + "/fznso_constraints/",
                         flt.stdLibDir() + "/fznso_rewrite/"};
  FznsoConstraintList constraints = o.library->constraint_types();
  native->predicates.reserve(2 * constraints.len);
  for (std::size_t i = 0; i < constraints.len; i++) {
    const FznsoConstraintType& c = constraints.constraints[i];
    std::vector<Type> params;
    params.reserve(c.arg_len);
    for (std::size_t j = 0; j < c.arg_len; j++) {
      params.push_back(type_from(c.arg_types[j]));
    }
    // A declared constraint enables both the global that lowers to it and the
    // FlatZinc builtin of the same name: the standard library gives some of the
    // latter a decomposition too (`redefinitions-*.mzn`), which a solver that
    // implements them natively must not get.
    std::string ident{view(c.ident)};
    native->predicates.emplace_back("fzn_" + ident, params);
    native->predicates.emplace_back(std::move(ident), std::move(params));
  }
  flt.setNativePredicates(native);
}

void register_fznso_solver() { static FZNSOSolverFactory _fznso_solverfactory; }

}  // namespace MiniZinc
