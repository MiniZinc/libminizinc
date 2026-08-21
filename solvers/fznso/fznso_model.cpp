/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/hash.hh>
#include <minizinc/solvers/fznso/fznso_model.hh>

#include <cstdint>
#include <limits>
#include <utility>

namespace MiniZinc {

namespace {

/// A borrowed view of an `ASTString`. The characters live in the model, so the
/// view is valid for as long as it is.
std::string_view view(const ASTString& s) {
  return s.size() == 0 ? std::string_view{} : std::string_view{s.c_str(), s.size()};
}

// --- Values ---------------------------------------------------------------
//
// A value handed across the interface *is* the MiniZinc expression: the handle
// is the `Expression*` and the method table below reads it where it lies.
// `value_ref` resolves an identifier before handing it over, so by the time a
// table sees an expression it already holds the payload — which is what lets
// `get_element` recurse with no context of its own.
//
// A decision is the exception: its handle carries the index rather than
// addressing anything, which is the "virtual value" the interface is built for.

const Expression* as_expr(const FznsoValue* v) { return static_cast<const Expression*>(v); }

FznsoValueRef value_ref(const Expression* e);

FznsoValueKind expr_kind(const FznsoValue* v) {
  const Expression* e = as_expr(v);
  if (e == nullptr) {
    return FznsoValueAbsent;
  }
  switch (Expression::eid(e)) {
    case Expression::E_INTLIT:
      return FznsoValueInt;
    case Expression::E_FLOATLIT:
      return FznsoValueFloat;
    case Expression::E_BOOLLIT:
      return FznsoValueBool;
    case Expression::E_STRINGLIT:
      return FznsoValueString;
    case Expression::E_SETLIT:
      return Expression::type(e).bt() == Type::BT_FLOAT ? FznsoValueSetFloat : FznsoValueSetInt;
    case Expression::E_ARRAYLIT:
      return FznsoValueList;
    // An identifier that survived `value_ref` names no declaration, so it is an
    // annotation atom such as `input_order`. A call reaching here is a nested
    // annotation, which the interface has no value kind for, so it too is
    // handed over as its bare name.
    case Expression::E_ID:
    case Expression::E_CALL:
      return FznsoValueString;
    default:
      return FznsoValueAbsent;
  }
}

/// The identifier a string-valued expression reports, or an empty string.
ASTString expr_string(const Expression* e) {
  switch (Expression::eid(e)) {
    case Expression::E_STRINGLIT:
      return Expression::cast<StringLit>(e)->v();
    case Expression::E_ID: {
      const auto* id = Expression::cast<Id>(e);
      return id->hasStr() ? id->v() : ASTString();
    }
    case Expression::E_CALL:
      return Expression::cast<Call>(e)->id();
    default:
      return {};
  }
}

std::size_t expr_len(const FznsoValue* v) {
  const Expression* e = as_expr(v);
  if (e == nullptr) {
    return 0;
  }
  switch (Expression::eid(e)) {
    case Expression::E_ARRAYLIT:
      return Expression::cast<ArrayLit>(e)->size();
    case Expression::E_SETLIT: {
      const auto* sl = Expression::cast<SetLit>(e);
      if (sl->isv() != nullptr) {
        return sl->isv()->size();
      }
      return sl->fsv() != nullptr ? sl->fsv()->size() : 0;
    }
    default:
      return expr_string(e).size();
  }
}

const char* expr_as_string(const FznsoValue* v) {
  ASTString s = expr_string(as_expr(v));
  return s.size() == 0 ? nullptr : s.c_str();
}

std::int64_t expr_as_int(const FznsoValue* v) {
  return IntLit::v(Expression::cast<IntLit>(as_expr(v))).toInt();
}

double expr_as_float(const FznsoValue* v) {
  FloatVal f = FloatLit::v(Expression::cast<FloatLit>(as_expr(v)));
  return f.isFinite() ? f.toDouble()
                      : (f.isPlusInfinity() ? std::numeric_limits<double>::infinity()
                                            : -std::numeric_limits<double>::infinity());
}

bool expr_as_bool(const FznsoValue* v) { return Expression::cast<BoolLit>(as_expr(v))->v(); }

// A bound MiniZinc leaves open becomes the widest the interface can carry.
// `toInt`/`toDouble` throw on an infinite value, and an exception must not
// cross the interface.
std::int64_t int_bound(IntVal i, bool upper) {
  if (i.isFinite()) {
    return i.toInt();
  }
  return upper ? std::numeric_limits<std::int64_t>::max()
               : std::numeric_limits<std::int64_t>::min();
}

double float_bound(FloatVal f, bool upper) {
  if (f.isFinite()) {
    return f.toDouble();
  }
  return upper ? std::numeric_limits<double>::infinity() : -std::numeric_limits<double>::infinity();
}

FznsoIntRange expr_range_int(const FznsoValue* v, std::size_t index) {
  IntSetVal* isv = Expression::cast<SetLit>(as_expr(v))->isv();
  auto i = static_cast<unsigned int>(index);
  return FznsoIntRange{int_bound(isv->min(i), false), int_bound(isv->max(i), true)};
}

FznsoFloatRange expr_range_float(const FznsoValue* v, std::size_t index) {
  FloatSetVal* fsv = Expression::cast<SetLit>(as_expr(v))->fsv();
  auto i = static_cast<unsigned int>(index);
  return FznsoFloatRange{float_bound(fsv->min(i), false), float_bound(fsv->max(i), true)};
}

FznsoValueRef expr_element(const FznsoValue* v, std::size_t index) {
  const auto* al = Expression::cast<ArrayLit>(as_expr(v));
  return value_ref((*al)[static_cast<unsigned int>(index)]);
}

// The interface never calls an accessor that does not match the kind, so the
// ones a shape cannot answer just report nothing rather than aborting.
FznsoDecisionIdx no_decision(const FznsoValue* /*v*/) { return 0; }
FznsoConstraintIdx no_constraint(const FznsoValue* /*v*/) { return 0; }

const FznsoValueMethods EXPR_METHODS = {
    expr_kind,      expr_len,     no_decision,    no_constraint,    expr_as_int,  expr_as_float,
    expr_as_string, expr_as_bool, expr_range_int, expr_range_float, expr_element,
};

// --- Decisions ------------------------------------------------------------
//
// The handle holds the index rather than addressing it, so a decision needs no
// storage at all. It is stored as `index + 1` because a null handle is the
// natural absent value.

FznsoValueKind decision_kind(const FznsoValue* /*v*/) { return FznsoValueDecision; }
std::size_t decision_len(const FznsoValue* /*v*/) { return 0; }
FznsoDecisionIdx decision_index(const FznsoValue* v) {
  return reinterpret_cast<std::uintptr_t>(v) - 1;
}
std::int64_t decision_as_int(const FznsoValue* /*v*/) { return 0; }
double decision_as_float(const FznsoValue* /*v*/) { return 0.0; }
const char* decision_as_string(const FznsoValue* /*v*/) { return nullptr; }
bool decision_as_bool(const FznsoValue* /*v*/) { return false; }
FznsoIntRange decision_range_int(const FznsoValue* /*v*/, std::size_t /*i*/) { return {0, 0}; }
FznsoFloatRange decision_range_float(const FznsoValue* /*v*/, std::size_t /*i*/) { return {0, 0}; }
FznsoValueRef decision_element(const FznsoValue* /*v*/, std::size_t /*i*/) { return {}; }

const FznsoValueMethods DECISION_METHODS = {
    decision_kind,      decision_len,         decision_index,     no_constraint,
    decision_as_int,    decision_as_float,    decision_as_string, decision_as_bool,
    decision_range_int, decision_range_float, decision_element,
};

FznsoValueRef decision_ref(int index) {
  return FznsoValueRef{reinterpret_cast<const FznsoValue*>(static_cast<std::uintptr_t>(index) + 1),
                       &DECISION_METHODS};
}

/// Hand \a e across the interface, following identifiers to whatever actually
/// holds the value: a decision variable, an array literal, or a parameter's
/// right hand side. A null expression is the absent value.
FznsoValueRef value_ref(const Expression* e) {
  while (e != nullptr && Expression::isa<Id>(e)) {
    const auto* id = Expression::cast<Id>(e);
    VarDecl* vd = id->decl();
    if (vd == nullptr) {
      break;  // an annotation atom, handed over as its name
    }
    int decision = FznsoModel::decisionOf(vd);
    if (decision >= 0) {
      return decision_ref(decision);
    }
    if (vd->e() == nullptr || vd->e() == e) {
      // Nothing stands behind the declaration, so the identifier is all there
      // is to hand over: this is how a search annotation names `first_fail` and
      // the other selection strategies, which are declared but never defined.
      break;
    }
    e = vd->e();
  }
  return FznsoValueRef{static_cast<const FznsoValue*>(e), &EXPR_METHODS};
}

fznso::Value value_of(const Expression* e) { return fznso::Value{value_ref(e)}; }

// --- Annotations ----------------------------------------------------------
//
// The annotation handle is likewise the MiniZinc expression: a `Call` for
// `int_search(...)`, or an `Id` for a bare atom.

FznsoStr annotation_ident(const FznsoAnnotation* a) {
  ASTString id = expr_string(static_cast<const Expression*>(a));
  return FznsoStr{id.size() == 0 ? nullptr : id.c_str(), id.size()};
}

std::size_t annotation_argument_len(const FznsoAnnotation* a) {
  const auto* e = static_cast<const Expression*>(a);
  return (e != nullptr && Expression::isa<Call>(e)) ? Expression::cast<Call>(e)->argCount() : 0;
}

FznsoValueRef annotation_argument(const FznsoAnnotation* a, std::size_t index) {
  const auto* e = static_cast<const Expression*>(a);
  return value_ref(Expression::cast<Call>(e)->arg(static_cast<unsigned int>(index)));
}

const FznsoAnnotationMethods ANNOTATION_METHODS = {annotation_ident, annotation_argument_len,
                                                   annotation_argument};

fznso::AnnotationRef annotation_ref(const Expression* e) {
  return fznso::AnnotationRef{
      FznsoAnnotationRef{static_cast<const FznsoAnnotation*>(e), &ANNOTATION_METHODS}};
}

/// The \a index-th annotation of \a ann. The set is tiny, so walking it beats
/// building an index for it.
Expression* annotation_at(const Annotation& ann, std::size_t index) {
  std::size_t i = 0;
  for (ExpressionSetIter it = ann.begin(); it != ann.end(); ++it, ++i) {
    if (i == index) {
      return *it;
    }
  }
  return nullptr;
}

FznsoType type_of(const Type& t) {
  FznsoTypeBase base = FznsoTypeBaseInt;
  switch (t.bt()) {
    case Type::BT_BOOL:
      base = FznsoTypeBaseBool;
      break;
    case Type::BT_FLOAT:
      base = FznsoTypeBaseFloat;
      break;
    case Type::BT_STRING:
      base = FznsoTypeBaseString;
      break;
    default:
      break;
  }
  return fznso::Type{base}
      .list(t.dim() != 0)
      .decision(t.isvar())
      .set(t.st() == Type::ST_SET)
      .opt(t.isOpt());
}

}  // namespace

int FznsoModel::decisionOf(const VarDecl* vd) {
  // The decision index lives in the declaration's payload, so that a callback
  // holding nothing but an expression can still resolve an identifier to it.
  // Flattening only uses the payload as a marker of its own while it builds the
  // flat model (see `cleanup_flat_model`); nothing reads it once the model is
  // handed to a solver, and the constructor below overwrites every one of them.
  return vd->type().isvar() && vd->type().dim() == 0 ? vd->payload() : -1;
}

FznsoModel::FznsoModel(Env& env, std::vector<Expression*> searchAnn)
    : _objectiveAnn(std::move(searchAnn)) {
  MiniZinc::Model* flat = env.flat();

  // Clear every payload first, including those of items the flattener removed:
  // an identifier may still point at one, and a value left over from
  // flattening would read back as a decision index.
  for (Item* item : *flat) {
    if (auto* vdi = item->dynamicCast<VarDeclI>()) {
      vdi->e()->payload(-1);
    }
  }

  // Pass one: give every decision variable an index. A variable defined to be
  // another one is an alias and shares its index; one defined to be a literal
  // is not a decision at all, and resolves to that literal instead.
  std::vector<VarDecl*> aliases;
  for (VarDeclIterator it = flat->vardecls().begin(); it != flat->vardecls().end(); ++it) {
    VarDecl* vd = it->e();
    if (!vd->type().isvar() || vd->type().dim() != 0) {
      continue;
    }
    if (vd->e() == nullptr) {
      vd->payload(static_cast<int>(_decisions.size()));
      _decisions.push_back(vd);
    } else if (Expression::isa<Id>(vd->e())) {
      aliases.push_back(vd);
    }
  }
  // Pass two: an alias may name a variable declared after it, so resolve them
  // only once every index exists. Chains are followed to their end.
  for (VarDecl* vd : aliases) {
    Expression* target = vd->e();
    while (target != nullptr && Expression::isa<Id>(target)) {
      VarDecl* decl = Expression::cast<Id>(target)->decl();
      if (decl == nullptr || decl == vd) {
        break;
      }
      if (decl->payload() >= 0) {
        vd->payload(decl->payload());
        break;
      }
      target = decl->e();
    }
  }

  // Mark every variable the output reads a value back for, which is the same
  // set `SolverInstanceBase2::assignSolutionToOutput` collects: the ones
  // annotated `output_var`, and the elements of an `output_array`. That is what
  // the solver is told it must assign.
  //
  // Names are collected alongside, but only so that a solver has something
  // legible to print: what a variable is called says nothing about whether a
  // value is needed for it.
  GCLock lock;
  _names.assign(_decisions.size(), std::string());
  _inSolution.assign(_decisions.size(), 0);
  auto needed = [&](Expression* e) {
    Id* id = Expression::dynamicCast<Id>(e);
    VarDecl* vd = id != nullptr ? id->decl() : nullptr;
    int index = vd != nullptr ? decisionOf(vd) : -1;
    if (index < 0) {
      return;
    }
    _inSolution[index] = 1;
    if (_names[index].empty()) {
      ASTString s = id->str();
      _names[index].assign(s.c_str(), s.size());
    }
  };
  for (std::size_t i = 0; i < _decisions.size(); i++) {
    Id* id = _decisions[i]->id();
    if (id != nullptr && id->hasStr()) {
      _names[i].assign(id->v().c_str(), id->v().size());
    }
  }
  const Constants& constants = Constants::constants();
  for (VarDeclIterator it = flat->vardecls().begin(); it != flat->vardecls().end(); ++it) {
    VarDecl* vd = it->e();
    if (Expression::ann(vd).isEmpty()) {
      continue;
    }
    if (Expression::ann(vd).contains(constants.ann.output_var)) {
      needed(vd->id());
    } else if (Expression::ann(vd).containsCall(constants.ann.output_array.aststr())) {
      if (auto* al = Expression::dynamicCast<ArrayLit>(vd->e())) {
        for (unsigned int j = 0; j < al->size(); j++) {
          needed((*al)[j]);
        }
      }
    }
  }

  for (ConstraintIterator it = flat->constraints().begin(); it != flat->constraints().end(); ++it) {
    if (Expression::isa<Call>(it->e())) {
      _constraints.push_back(Expression::cast<Call>(it->e()));
    }
  }

  SolveI* si = flat->solveItem();
  if (si != nullptr && si->st() != SolveI::ST_SAT) {
    _objectiveArg = si->e();
    bool isFloat =
        _objectiveArg != nullptr && Expression::type(_objectiveArg).bt() == Type::BT_FLOAT;
    // The registry names an objective for the type it optimises, and puts that
    // type first: `int_minimize`, `float_maximize`.
    _objectiveIdent = isFloat ? "float_" : "int_";
    _objectiveIdent += si->st() == SolveI::ST_MIN ? "minimize" : "maximize";
  }
}

FznsoType FznsoModel::decision_type(fznso::Decision decision) const {
  return type_of(_decisions[decision.index]->type());
}

fznso::Value FznsoModel::decision_domain(fznso::Decision decision) const {
  return value_of(_decisions[decision.index]->ti()->domain());
}

std::optional<std::string_view> FznsoModel::decision_name(fznso::Decision decision) const {
  const std::string& name = _names[decision.index];
  if (name.empty()) {
    return std::nullopt;
  }
  return std::string_view{name};
}

bool FznsoModel::decision_in_solution(fznso::Decision decision) const {
  return _inSolution[decision.index] != 0;
}

bool FznsoModel::decision_defined(fznso::Decision decision) const {
  return Expression::ann(_decisions[decision.index])
      .contains(Constants::constants().ann.is_defined_var);
}

std::size_t FznsoModel::decision_annotation_count(fznso::Decision decision) const {
  return Expression::ann(_decisions[decision.index]).size();
}

fznso::AnnotationRef FznsoModel::decision_annotation(fznso::Decision decision,
                                                     std::size_t index) const {
  return annotation_ref(annotation_at(Expression::ann(_decisions[decision.index]), index));
}

std::string_view FznsoModel::constraint_ident(fznso::Constraint constraint) const {
  return view(_constraints[constraint.index]->id());
}

std::size_t FznsoModel::constraint_argument_count(fznso::Constraint constraint) const {
  return _constraints[constraint.index]->argCount();
}

fznso::Value FznsoModel::constraint_argument(fznso::Constraint constraint,
                                             std::size_t index) const {
  return value_of(_constraints[constraint.index]->arg(static_cast<unsigned int>(index)));
}

std::optional<fznso::Decision> FznsoModel::constraint_defines(fznso::Constraint constraint) const {
  Call* defines = Expression::ann(_constraints[constraint.index])
                      .getCall(Constants::constants().ann.defines_var);
  if (defines == nullptr || defines->argCount() == 0 || !Expression::isa<Id>(defines->arg(0))) {
    return std::nullopt;
  }
  VarDecl* vd = Expression::cast<Id>(defines->arg(0))->decl();
  int index = vd != nullptr ? decisionOf(vd) : -1;
  if (index < 0) {
    return std::nullopt;
  }
  return fznso::Decision{static_cast<std::size_t>(index)};
}

std::size_t FznsoModel::constraint_annotation_count(fznso::Constraint constraint) const {
  return Expression::ann(_constraints[constraint.index]).size();
}

fznso::AnnotationRef FznsoModel::constraint_annotation(fznso::Constraint constraint,
                                                       std::size_t index) const {
  return annotation_ref(annotation_at(Expression::ann(_constraints[constraint.index]), index));
}

fznso::Value FznsoModel::objective_arg() const { return value_of(_objectiveArg); }

fznso::AnnotationRef FznsoModel::objective_annotation(std::size_t index) const {
  return annotation_ref(_objectiveAnn[index]);
}

}  // namespace MiniZinc
