/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astiterator.hh>
#include <minizinc/copy.hh>
#include <minizinc/eval_par.hh>
#include <minizinc/file_utils.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/native_predicates.hh>

#include <algorithm>
#include <cstring>
#include <string>
#include <map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace MiniZinc {

namespace {

/// How a signature a solver declares relates to a candidate declaration.
enum class NativeMatch {
  /// Not the same constraint.
  None,
  /// The same, so the decomposition is simply not needed.
  Exact,
  /// The same but for parameters the solver only takes fixed. The
  /// decomposition is still needed for the calls that do not fix them.
  Fixed,
};

/// Compare what a solver declares against a candidate declaration's parameters.
///
/// Types are compared field-wise rather than with `Type::operator==` because a
/// declared type carries no enum identity: a solver posting
/// `fzn_all_different_int` accepts an array of any enumerated integer just as it
/// accepts a plain one. The dimension has to match exactly, though: a solver
/// declaring a list means a one-dimensional array, so the standard library's
/// `fzn_table_int` — whose parameter is `array[int,int] of int` — is a different
/// constraint from the `table_int` a solver posts.
NativeMatch native_match(const std::vector<Type>& declared, FunctionI* fi) {
  if (fi->paramCount() != declared.size()) {
    return NativeMatch::None;
  }
  bool fixed = false;
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    const Type& d = declared[i];
    Type a = fi->param(i)->type();
    if (d.bt() != a.bt() || d.st() != a.st() || d.isOpt() != a.isOpt() || d.dim() != a.dim()) {
      return NativeMatch::None;
    }
    // A solver declaring a variable where only a value is ever passed is no
    // obstacle: a FlatZinc variable position accepts a literal. The other way
    // round is the interesting one — the solver only implements the fixed case.
    if (!d.isvar() && a.isvar()) {
      fixed = true;
    }
  }
  return fixed ? NativeMatch::Fixed : NativeMatch::Exact;
}

/// Give \a fi a second declaration taking \a declared, which the solver
/// implements, and make its body dispatch to it whenever the arguments it only
/// takes fixed turn out to be fixed:
///
/// ```
/// predicate p(var int: x, int: y);                    % added, no body
/// predicate p(var int: x, var int: y) =
///   if is_fixed(y) then p(x, fix(y)) else <body> endif;
/// ```
///
/// This is what a solver library writes by hand for every such constraint. It
/// is generated here instead, because any argument of any constraint can be
/// declared fixed and writing out the combinations does not scale.
void add_fixed_form(EnvI& env, Model* m, FunctionI* fi, const std::vector<Type>& declared) {
  GCLock lock;
  static const ASTString isFixed("is_fixed");

  std::vector<VarDecl*> params(fi->paramCount());
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    VarDecl* p = fi->param(i);
    // The parameter names carry over, because overload resolution filters on
    // them; only the instantiation differs.
    auto* ti =
        new TypeInst(Location().introduce(), declared[i], p->ti()->ranges(), p->ti()->domain());
    params[i] = new VarDecl(Location().introduce(), ti, p->id()->str());
    params[i]->toplevel(false);
  }
  auto* native =
      new FunctionI(Location().introduce(), fi->id(), fi->ti(), params, nullptr, fi->fromStdLib());
  // Sorted, so that the overload family stays ordered most-specific first: that
  // is what makes the dispatch below resolve to this form rather than back to
  // the one it is the fixed case of.
  if (!m->registerFn(env, native, true, false)) {
    return;  // something already declares this exact form
  }
  m->addItem(native);

  std::vector<Expression*> args(fi->paramCount());
  Expression* guard = nullptr;
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    VarDecl* p = fi->param(i);
    auto* arg = new Id(Location().introduce(), p->id()->str(), p);
    Expression::type(arg, p->type());
    if (declared[i].isvar() == p->type().isvar()) {
      args[i] = arg;
      continue;
    }
    std::vector<Expression*> one{arg};
    Call* fixed = Call::a(Location().introduce(), env.constants.ids.fix, one);
    fixed->decl(m->matchFn(env, fixed, false, true));
    fixed->type(fixed->decl()->rtype(env, one, nullptr, false));
    args[i] = fixed;

    Call* test = Call::a(Location().introduce(), isFixed, one);
    test->decl(m->matchFn(env, test, false, true));
    test->type(Type::parbool());
    if (guard == nullptr) {
      guard = test;
    } else {
      guard = new BinOp(Location().introduce(), guard, BOT_AND, test);
      Expression::type(guard, Type::parbool());
    }
  }

  Call* nativeCall = Call::a(Location().introduce(), fi->id(), args);
  nativeCall->decl(native);
  nativeCall->type(native->rtype(env, args, nullptr, false));
  auto* dispatch = new ITE(Location().introduce(), {guard, nativeCall}, fi->e());
  Expression::type(dispatch, Expression::type(fi->e()));
  fi->e(dispatch);
}

/// Whether \a fi holds a decomposition MiniZinc itself supplies, and so one a
/// solver's own implementation is allowed to replace.
///
/// A solver's MiniZinc library lives outside \a libraryDirs, so an override it
/// provides stays. So does `nosets.mzn`, which a model includes to say it wants
/// no set variables at all — a solver saying it accepts `set_lt` is not an
/// answer to that.
bool is_library_decomposition(FunctionI* fi, const std::vector<std::string>& prefixes) {
  ASTString file = fi->loc().filename();
  if (file.endsWith("nosets.mzn") || file.endsWith("set2bool.mzn")) {
    return false;
  }
  for (const std::string& prefix : prefixes) {
    if (file.size() >= prefix.size() &&
        std::strncmp(file.c_str(), prefix.c_str(), prefix.size()) == 0) {
      return true;
    }
  }
  return false;
}

}  // namespace

/// Drop the decomposition of every declaration \a np says the solver implements,
/// and give the ones it only implements for fixed arguments a form that says so.
///
/// Driven from the declared list rather than from the model: each declaration is
/// one lookup in the function map, which already holds every declaration the
/// model and its includes brought in.
void enable_native_predicates(Env& env, const NativePredicates& np) {
  GCLock lock;
  Model* m = env.model();
  EnvI& envi = env.envi();

  std::vector<std::string> prefixes;
  for (const std::string& dir : np.libraryDirs) {
    prefixes.push_back(FileUtils::file_path(dir));
    if (!prefixes.back().empty() && prefixes.back().back() != '/') {
      prefixes.back() += '/';
    }
  }

  auto report = [&](FunctionI* fi, const char* how) {
    if (envi.fopts.verbose) {
      envi.errstream << "\tSolver implements " << fi->id() << " (" << how << ")\n";
    }
  };

  // A solver may declare one constraint several ways — the plain form and a
  // form fixing some arguments — and they resolve to the same declaration, so
  // the verdicts are collected before any of them is acted on.
  std::vector<FunctionI*> exact;
  std::vector<std::pair<FunctionI*, const std::vector<Type>*>> fixed;
  for (const auto& p : np.predicates) {
    FunctionI* fi = m->matchFn(envi, ASTString(p.first), p.second, false);
    if (fi == nullptr || fi->e() == nullptr) {
      continue;  // not declared here, or already native
    }
    // Only a `var bool` predicate can become a constraint in the flat model.
    // This also skips the parameter-type variants monomorphisation makes of a
    // predicate: those are evaluated by the compiler, so they must keep their
    // body — `Model::checkFnValid` rejects a body-less function returning par.
    if (!fi->ti()->type().isvarbool() || !is_library_decomposition(fi, prefixes)) {
      continue;
    }
    switch (native_match(p.second, fi)) {
      case NativeMatch::Exact:
        exact.push_back(fi);
        break;
      case NativeMatch::Fixed: {
        // The form fixing the most arguments is the one to dispatch to: it is
        // the hardest to satisfy, and the others are reachable from the
        // decomposition it falls back to.
        std::size_t n = 0;
        for (const Type& t : p.second) {
          n += static_cast<std::size_t>(!t.isvar());
        }
        auto* best = &fixed;
        auto it = std::find_if(best->begin(), best->end(),
                               [&](const auto& e) { return e.first == fi; });
        if (it == best->end()) {
          best->emplace_back(fi, &p.second);
        } else {
          std::size_t have = 0;
          for (const Type& t : *it->second) {
            have += static_cast<std::size_t>(!t.isvar());
          }
          if (n > have) {
            it->second = &p.second;
          }
        }
        break;
      }
      case NativeMatch::None:
        break;
    }
  }

  for (FunctionI* fi : exact) {
    report(fi, "native");
    fi->e(nullptr);
  }
  for (const auto& f : fixed) {
    if (f.first->e() == nullptr) {
      continue;  // an exact declaration of the same constraint already won
    }
    report(f.first, "native when its arguments are fixed");
    add_fixed_form(envi, m, f.first, *f.second);
  }
}

void declare_solver_constraints(Env& env, const NativePredicates& np,
                                const std::string& includeFile) {
  GCLock lock;
  Model* m = env.model();

  /// Whether the model asked for the solver's own constraints, and which
  /// predicates it already has a declaration for.
  class Scan {
  public:
    Scan(const std::string& includeFile) : _includeFile(includeFile) {}
    bool enterModel(Model* /*m*/) { return true; }
    bool enter(Item* /*i*/) { return true; }
    void vVarDeclI(VarDeclI* /*i*/) {}
    void vAssignI(AssignI* /*i*/) {}
    void vConstraintI(ConstraintI* /*i*/) {}
    void vSolveI(SolveI* /*i*/) {}
    void vOutputI(OutputI* /*i*/) {}
    void vIncludeI(IncludeI* i) { asked = asked || i->f() == _includeFile; }
    void vFunctionI(FunctionI* fi) {
      std::string name(fi->id().c_str());
      declared.insert(name);
      // Kept by arity as well as by name: a reified form takes the parameters
      // of the overload with one fewer argument than itself.
      byArity[name][fi->paramCount()] = fi;
      // Collected here rather than by walking the model afterwards: a rewrite
      // lives in an included file, and iterating the model reaches only its own
      // items.
      if (fi->e() != nullptr && fi->paramCount() > 0 && name.size() > 5 &&
          name.compare(name.size() - 5, 5, "_reif") == 0) {
        reified.push_back(fi);
      }
    }

    bool asked = false;
    std::unordered_set<std::string> declared;
    std::unordered_map<std::string, std::map<unsigned int, FunctionI*>> byArity;
    std::vector<FunctionI*> reified;

  private:
    const std::string& _includeFile;
  };

  Scan scan(includeFile);
  iter_items(scan, m);

  // Every name the solver declares, so that a reified form can be recognised by
  // the constraint it reifies even when that one is not MiniZinc's either.
  std::unordered_set<std::string> solverDeclares;
  for (const auto& p : np.predicates) {
    solverDeclares.insert(p.first);
  }

  /// Whether MiniZinc might emit \a name itself, rather than it being something
  /// only a model can ask for.
  ///
  /// A reified or half-reified form is derived by the flattener from the
  /// constraint it reifies, so it is never written in a model, and a solver
  /// declaring one is telling MiniZinc it may reify that way. The standard
  /// library says nothing at all about the `_imp` forms, so without this the
  /// only way to enable half-reification would be a solver library full of
  /// body-less declarations — which is exactly what the declared list replaces.
  auto emittedByMiniZinc = [&](const std::string& name) {
    for (const char* suffix : {"_reif", "_imp"}) {
      std::size_t n = std::strlen(suffix);
      if (name.size() > n && name.compare(name.size() - n, n, suffix) == 0) {
        std::string base = name.substr(0, name.size() - n);
        return scan.declared.count(base) != 0 || solverDeclares.count(base) != 0;
      }
    }
    return false;
  };

  for (const auto& p : np.predicates) {
    if (scan.declared.count(p.first) != 0) {
      continue;
    }
    // `fzn_` names are how MiniZinc lowers its own globals, not something a
    // model should reach for, so they are normally left to whatever library
    // file declares them. The exception is a form the library deliberately has
    // no file for: `fznso_constraints/` holds no `_imp`, because a file there
    // would stand in the way of falling back to `_reif` and offer only what it
    // could write itself. A solver that implements half-reification natively
    // still needs that name to exist for the rewrite layer to lower onto, and
    // with no file to declare it this is the only place it can come from.
    if (p.first.compare(0, 4, "fzn_") == 0 && !emittedByMiniZinc(p.first)) {
      continue;
    }
    // Anything else is the solver's own vocabulary, which a model has to ask
    // for by name so that its dependence on that solver is visible.
    if (!scan.asked && !emittedByMiniZinc(p.first)) {
      continue;
    }
    // A reified or half-reified form is re-resolved from the constraint it
    // reifies by matching parameter names, so a synthesised one has to carry
    // the base predicate's. Inventing `x1`, `x2`, … would leave the reification
    // unfindable — and the base is where the meaningful names already are.
    // Taken from what was parsed rather than through `matchFn`: this runs
    // before typechecking, so the function registry is not built yet.
    FunctionI* base = nullptr;
    for (const char* suffix : {"_reif", "_imp"}) {
      std::size_t n = std::strlen(suffix);
      if (p.second.empty() || p.first.size() <= n ||
          p.first.compare(p.first.size() - n, n, suffix) != 0) {
        continue;
      }
      auto it = scan.byArity.find(p.first.substr(0, p.first.size() - n));
      if (it != scan.byArity.end()) {
        auto a = it->second.find(static_cast<unsigned int>(p.second.size()) - 1);
        if (a != it->second.end()) {
          base = a->second;
        }
      }
      break;
    }

    std::vector<VarDecl*> params(p.second.size());
    for (std::size_t i = 0; i < p.second.size(); i++) {
      Type t = p.second[i];
      std::vector<TypeInst*> ranges;
      if (t.dim() != 0) {
        ranges.push_back(new TypeInst(Location().introduce(), Type::parint()));
      }
      auto* ti = new TypeInst(Location().introduce(), t, ASTExprVec<TypeInst>(ranges), nullptr);
      ASTString name;
      if (base != nullptr && i < base->paramCount()) {
        name = base->param(static_cast<unsigned int>(i))->id()->str();
      } else if (base != nullptr && i + 1 == p.second.size()) {
        // The Boolean the reified form adds, which the base has no name for.
        name = ASTString("b");
        for (std::size_t j = 0; j + 1 < p.second.size(); j++) {
          if (params[j]->id()->str() == name) {
            name = ASTString("r");
            break;
          }
        }
      } else {
        name = ASTString("x" + std::to_string(i + 1));
      }
      params[i] = new VarDecl(Location().introduce(), ti, name);
      params[i]->toplevel(false);
    }
    auto* fi = new FunctionI(Location().introduce(), ASTString(p.first),
                             new TypeInst(Location().introduce(), Type::varbool()), params);
    if (m->registerFn(env.envi(), fi, true, false)) {
      m->addItem(fi);
    }
  }

  // Give each rewritten builtin the half-reified form its target has.
  //
  // A rewrite layer renames a FlatZinc builtin onto the name a solver declares,
  // and writes the reified form as that same rename with a Boolean on the end.
  // The half-reified form cannot be written beside it: the body would have to
  // name `<target>_imp`, and for a solver that does not implement half
  // reification no such declaration exists, so every model would fail to
  // typecheck on a predicate it never calls. It is derived here instead — from
  // the reified rewrite that is already there, and only for the solvers whose
  // declared list has the target.
  //
  // Without it `Model::matchReification` finds `X_reif`, finds no `X_imp`, and
  // returns the reified form. The solver's half-reified propagator is never
  // reached, and the flattener pays for an equivalence where an implication
  // would have done.
  std::vector<FunctionI*> derived;
  for (FunctionI* fi : scan.reified) {
    std::string name(fi->id().c_str());
    // The rewrite has to be a plain rename: one call, and the Boolean this form
    // adds passed straight through as the target's last argument. Anything else
    // says something the half-reified form does not.
    // The arity may differ: a rename that reshapes, such as `int_eq_reif` onto
    // `int_lin_eq_reif`, adds the coefficients and the constant. What has to
    // hold is that the Boolean this form adds is passed straight through as the
    // target's last argument, so that the half-reified target means the same
    // thing about the same variable.
    Call* body = Expression::dynamicCast<Call>(fi->e());
    if (body == nullptr || body->argCount() == 0) {
      continue;
    }
    std::string target(body->id().c_str());
    if (target.size() <= 5 || target.compare(target.size() - 5, 5, "_reif") != 0) {
      continue;
    }
    Id* last = Expression::dynamicCast<Id>(body->arg(body->argCount() - 1));
    if (last == nullptr || last->str() != fi->param(fi->paramCount() - 1)->id()->str()) {
      continue;
    }
    std::string targetImp = target.substr(0, target.size() - 5) + "_imp";
    if (solverDeclares.count(targetImp) == 0) {
      continue;
    }
    std::string nameImp = name.substr(0, name.size() - 5) + "_imp";
    if (scan.declared.count(nameImp) != 0) {
      continue;
    }
    // Copied rather than shared: the two declarations are typechecked
    // separately, and an argument expression cannot belong to both.
    auto* ci = copy(env.envi(), fi)->cast<FunctionI>();
    std::vector<VarDecl*> params(ci->paramCount());
    for (unsigned int i = 0; i < ci->paramCount(); i++) {
      params[i] = ci->param(i);
    }
    Call* cbody = Expression::cast<Call>(ci->e());
    std::vector<Expression*> args(cbody->argCount());
    for (unsigned int i = 0; i < cbody->argCount(); i++) {
      args[i] = cbody->arg(i);
    }
    auto* nf = new FunctionI(Location().introduce(), ASTString(nameImp), ci->ti(), params,
                             Call::a(Location().introduce(), ASTString(targetImp), args));
    derived.push_back(nf);
  }
  for (FunctionI* fi : derived) {
    if (m->registerFn(env.envi(), fi, true, false)) {
      m->addItem(fi);
    }
  }
}

}  // namespace MiniZinc
