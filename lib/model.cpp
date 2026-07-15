/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/aststring.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>
#include <minizinc/type.hh>

#include <algorithm>
#include <vector>

#undef MZN_DEBUG_FUNCTION_REGISTRY

namespace MiniZinc {

Model::FnEntry::FnEntry(EnvI& env, FunctionI* fi0)
    : t(fi0->paramCount()), fi(fi0), isPolymorphic(false), isPolymorphicVariant(false) {
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    t[i] = fi->param(i)->type();
    if (t[i].structBT() && t[i].typeId() == 0) {
      fi->param(i)->ti()->canonicaliseStruct(env);
      t[i] = fi->param(i)->ti()->type();
      fi->param(i)->type(t[i]);
    }
    isPolymorphic |= checkPoly(env, t[i]);
  }
  if (fi->ti()->type().structBT() && fi->ti()->type().typeId() == 0) {
    fi->ti()->canonicaliseStruct(env);
  }
}

bool Model::FnEntry::checkPoly(const EnvI& env, const Type& t) {
  if (t.bt() == Type::BT_TOP) {
    return true;
  }
  if (t.structBT()) {
    StructType* st = env.getStructType(t);
    for (unsigned int i = 0; i < st->size(); ++i) {
      if (checkPoly(env, (*st)[i])) {
        return true;
      }
    }
  }
  return false;
}

bool Model::FnEntry::compare(const EnvI& env, const Model::FnEntry& e1, const Model::FnEntry& e2) {
  if (e1.t.size() < e2.t.size()) {
    return true;
  }
  if (e1.t.size() == e2.t.size()) {
    for (unsigned int i = 0; i < e1.t.size(); i++) {
      if (e1.t[i] != e2.t[i]) {
        if (e1.t[i].isSubtypeOf(env, e2.t[i], true)) {
          assert(!e2.t[i].isSubtypeOf(env, e1.t[i], true));
          return true;
        }
        if (e2.t[i].isSubtypeOf(env, e1.t[i], true)) {
          return false;
        }
        switch (e1.t[i].cmp(e2.t[i])) {
          case -1:
            return true;
          case 1:
            return false;
          default:
            assert(false);
        }
      }
    }
  }
  return false;
}

Model::Model() : _parent(nullptr), _solveItem(nullptr), _outputItem(nullptr) {}

Model::~Model() {
  for (auto* i : _items) {
    if (auto* ii = i->dynamicCast<IncludeI>()) {
      if (ii->own()) {
        delete ii->m();
        ii->m(nullptr);
      }
    }
  }
}

VarDeclIteratorContainer Model::vardecls() { return VarDeclIteratorContainer(this); }
ConstraintIteratorContainer Model::constraints() { return ConstraintIteratorContainer(this); }
FunctionIteratorContainer Model::functions() { return FunctionIteratorContainer(this); }

VarDeclIterator VarDeclIteratorContainer::begin() { return VarDeclIterator(_m, _m->begin()); }
VarDeclIterator VarDeclIteratorContainer::end() { return VarDeclIterator(_m, _m->end()); }
ConstraintIterator ConstraintIteratorContainer::begin() {
  return ConstraintIterator(_m, _m->begin());
}
ConstraintIterator ConstraintIteratorContainer::end() { return ConstraintIterator(_m, _m->end()); }
FunctionIterator FunctionIteratorContainer::begin() { return FunctionIterator(_m, _m->begin()); }
FunctionIterator FunctionIteratorContainer::end() { return FunctionIterator(_m, _m->end()); }

SolveI* Model::solveItem() { return _solveItem; }

OutputI* Model::outputItem() { return _outputItem; }

void Model::addItem(Item* i) {
  _items.push_back(i);
  if (i->isa<SolveI>()) {
    Model* m = this;
    while (m->_parent != nullptr) {
      m = m->_parent;
    }
    m->_solveItem = i->cast<SolveI>();
  } else if (i->isa<OutputI>()) {
    Model* m = this;
    while (m->_parent != nullptr) {
      m = m->_parent;
    }
    m->_outputItem = i->cast<OutputI>();
  }
}

void Model::setOutputItem(OutputI* oi) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  m->_outputItem = oi;
}

namespace {

enum PossibleBaseTypes { PBT_ANY, PBT_ALL, PBT_BIFS, PBT_BIF, PBT_I };
PossibleBaseTypes pbt_from_type(const Type& t) {
  if (t.any()) {
    return PBT_ANY;
  }
  if (t.isvar()) {
    if (t.isOpt()) {
      return PBT_BIF;
    }
    if (t.isSet()) {
      return PBT_I;
    }
    return PBT_BIFS;
  }
  if (t.isSet()) {
    return PBT_BIF;
  }
  return PBT_ALL;
}
PossibleBaseTypes pbt_join(PossibleBaseTypes pbt0, PossibleBaseTypes pbt1) {
  if (pbt0 == PBT_ANY) {
    return pbt1;
  }
  if (pbt1 == PBT_ANY) {
    return pbt0;
  }
  if (pbt0 == PBT_ALL) {
    return pbt1;
  }
  if (pbt1 == PBT_ALL) {
    return pbt0;
  }
  if (pbt0 == PBT_BIFS) {
    return pbt1;
  }
  if (pbt1 == PBT_BIFS) {
    return pbt0;
  }
  if (pbt0 == PBT_I || pbt1 == PBT_I) {
    return PBT_I;
  }
  assert(pbt0 == PBT_BIF && pbt1 == PBT_BIF);
  return PBT_BIF;
}
void set_to_lowest(Type& t, PossibleBaseTypes pbt) {
  t.any(false);
  if (pbt == PBT_ALL || pbt == PBT_ANY || pbt == PBT_BIFS) {
    t.st(Type::ST_PLAIN);
  }
  if (pbt == PBT_ANY) {
    t.ot(Type::OT_OPTIONAL);
    t.ti(Type::TI_PAR);
  }
  t.bt(pbt == PBT_I ? Type::BT_INT : Type::BT_BOOL);
}
bool can_increment_type(const Type& t, PossibleBaseTypes pbt) {
  switch (pbt) {
    case PBT_ANY:
      return t.ot() == Type::OT_OPTIONAL || t.ti() == Type::TI_PAR || t.st() == Type::ST_PLAIN;
    case PBT_ALL:
      return t.st() == Type::ST_PLAIN || t.bt() != Type::BT_FLOAT;
    case PBT_BIFS:
      return t.st() == Type::ST_PLAIN || t.bt() != Type::BT_INT;
    case PBT_BIF:
      return t.bt() != Type::BT_FLOAT;
    case PBT_I:
      return false;
  }
  throw InternalError("Invalid BaseType");
}
void increment_type(Type& t, PossibleBaseTypes pbt) {
  assert(pbt != PBT_I);
  if (pbt == PBT_ANY) {
    if (t.ti() == Type::TI_PAR) {
      if (t.ot() == Type::OT_OPTIONAL) {
        if (t.st() == Type::ST_PLAIN) {
          if (t.bt() != Type::BT_ANN) {
            t.bt(static_cast<Type::BaseType>(t.bt() + 1));
          } else {
            t.bt(Type::BT_BOOL);
            t.st(Type::ST_SET);
          }
        } else {
          if (t.bt() != Type::BT_ANN) {
            t.bt(static_cast<Type::BaseType>(t.bt() + 1));
          } else {
            t.bt(Type::BT_BOOL);
            t.st(Type::ST_PLAIN);
            t.ot(Type::OT_PRESENT);
          }
        }
      } else {
        if (t.st() == Type::ST_PLAIN) {
          if (t.bt() != Type::BT_ANN) {
            t.bt(static_cast<Type::BaseType>(t.bt() + 1));
          } else {
            t.bt(Type::BT_BOOL);
            t.st(Type::ST_SET);
          }
        } else {
          if (t.bt() != Type::BT_ANN) {
            t.bt(static_cast<Type::BaseType>(t.bt() + 1));
          } else {
            t.bt(Type::BT_BOOL);
            t.st(Type::ST_PLAIN);
            t.ot(Type::OT_OPTIONAL);
            t.ti(Type::TI_VAR);
          }
        }
      }
    } else {
      if (t.ot() == Type::OT_OPTIONAL) {
        if (t.bt() != Type::BT_FLOAT) {
          t.bt(static_cast<Type::BaseType>(t.bt() + 1));
        } else {
          t.bt(Type::BT_BOOL);
          t.ot(Type::OT_PRESENT);
        }
      } else {
        assert(t.st() != Type::ST_SET);
        if (t.bt() != Type::BT_FLOAT) {
          t.bt(static_cast<Type::BaseType>(t.bt() + 1));
        } else {
          t.bt(Type::BT_INT);
          t.st(Type::ST_SET);
        }
      }
    }
  } else if (pbt == PBT_ALL) {
    if (t.bt() != Type::BT_ANN) {
      t.bt(static_cast<Type::BaseType>(t.bt() + 1));
    } else {
      t.bt(Type::BT_BOOL);
      t.st(Type::ST_SET);
    }
  } else {
    if (t.bt() != Type::BT_FLOAT) {
      t.bt(static_cast<Type::BaseType>(t.bt() + 1));
    } else {
      assert(pbt == PBT_BIFS);
      t.bt(Type::BT_INT);
      t.st(Type::ST_SET);
    }
  }
}

}  // namespace

// For each TIId, store pointers to the type objects that refer to this TIId
struct TIIDInfo {
  std::vector<Type*> t;
  PossibleBaseTypes pbt;
  TIIDInfo(std::vector<Type*> t0, PossibleBaseTypes pbt0) : t(std::move(t0)), pbt(pbt0) {}
};

void TypeInst::collectTypeIds(std::unordered_map<ASTString, size_t>& seen_tiids,
                              std::vector<TIIDInfo>& type_ids) const {
  auto* al = Expression::cast<ArrayLit>(domain());
  for (unsigned int i = 0; i < al->size(); i++) {
    auto* ti = Expression::cast<TypeInst>((*al)[i]);
    if (ti->type().bt() == Type::BT_TOP) {
      // If type is top, either this is a TIId, or it is caused by <>
      if (ti->domain() != nullptr) {
        assert(Expression::isa<TIId>(ti->domain()));
        TIId* id0 = Expression::cast<TIId>(ti->domain());
        auto it = seen_tiids.find(id0->v());
        if (it == seen_tiids.end()) {
          type_ids.emplace_back(
              std::vector<Type*>({&ti->_type}),
              pbt_join(ti->type().any() ? PBT_ANY : PBT_ALL, pbt_from_type(ti->type())));
          seen_tiids.emplace(id0->v(), type_ids.size() - 1);
        } else {
          TIIDInfo& info = type_ids[it->second];
          info.pbt = pbt_join(info.pbt, pbt_from_type(ti->type()));
          info.t.push_back(&ti->_type);
        }
      }
    } else if (ti->type().structBT()) {
      size_t size = seen_tiids.size();
      ti->collectTypeIds(seen_tiids, type_ids);
    }
  }
}

namespace {
// Two decls with identical parameter types but different parameter names are
// intentionally distinct overloads under named-arguments (e.g.
// interval(start:, duration:) vs interval(start:, end:)). Anonymous
// parameters get an empty-string id (lib/parser.yxx ti_expr_and_id_or_anon),
// so anon-vs-anon compares equal.
bool sameParameterNames(FunctionI* a, FunctionI* b) {
  assert(a->paramCount() == b->paramCount());
  for (unsigned int i = 0; i < a->paramCount(); i++) {
    Id* ai = a->param(i)->id();
    Id* bi = b->param(i)->id();
    if (!ai->hasStr() || !bi->hasStr()) {
      // Synthetic param ids (e.g. monomorphic copies use numeric idns).
      // Defer to caller's other discrimination — treat as same.
      continue;
    }
    if (ai->v() != bi->v()) {
      return false;
    }
  }
  return true;
}

// Test whether functions \a a and \a b have the exact same sequence of parameters
// (both in terms of types and names).
bool exact_redeclaration(FunctionI* a, FunctionI* b) {
  if (a->paramCount() != b->paramCount()) {
    return false;
  }
  for (unsigned int i = 0; i < a->paramCount(); i++) {
    if (a->param(i)->type() != b->param(i)->type()) {
      return false;
    }
  }
  return sameParameterNames(a, b);
}

// A parameter can be supplied by name at a call site iff it has a real identifier that
// does not start with "_" (lib/parser.yxx rejects `_x: e` as a named argument).
bool is_nameable(VarDecl* p) { return p->id()->hasStr() && !p->id()->v().beginsWith("_"); }

// Normalise a declaration's parameter types to its *signature class*: every parameter
// coerced to par and present, with enum / type-inst-variable identity cleared. Two
// declarations belong to the same signature class - i.e. differ only in var/par or
// optionality (and enum/tyvar identity) - iff their normalised signatures are equal.
// This mirrors the per-parameter normalisation used by isStrictNarrowing in
// checkSiblingParameterNames.
std::vector<Type> normalized_param_sig(EnvI& env, const FunctionI* fi) {
  std::vector<Type> sig(fi->paramCount());
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    Type t = fi->param(i)->type();
    t.mkPar(env);
    t.mkPresentDeep(env);
    t.typeId(0);
    sig[i] = t;
  }
  return sig;
}

// Map every nameable parameter of \a fi to its declaration. Returns false if \a fi cannot
// be the target of a call that supplies all of its arguments by name, either because a
// parameter that must be supplied cannot be named, or because two parameters share a name.
bool nameable_params(FunctionI* fi, ASTStringMap<VarDecl*>& byName) {
  for (unsigned int i = 0; i < fi->paramCount(); i++) {
    VarDecl* p = fi->param(i);
    if (!is_nameable(p)) {
      // A parameter with a default never has to be supplied, so it can stay unnameable.
      if (p->e() == nullptr) {
        return false;
      }
      continue;
    }
    if (!byName.insert({p->id()->v(), p}).second) {
      return false;
    }
  }
  return true;
}

// Named arguments identify an overload by the names a call supplies, not by their
// position. A call that supplies every argument by name, using the name set K, selects a
// declaration D iff required(D) <= K <= nameable(D) and the type of every name in K
// matches. Two declarations are indistinguishable by such a call iff some K is accepted by
// both with the same type for every name in K.
//
// It suffices to test the smallest candidate, K* = required(a) + required(b): every common
// K contains K*, and enlarging K only adds names, hence only adds type constraints. So if
// the types agree on K* then the call K* is ambiguous, and if they disagree on some name in
// K* then so does every larger K.
bool ambiguous_named_call(FunctionI* a, FunctionI* b) {
  ASTStringMap<VarDecl*> pa;
  ASTStringMap<VarDecl*> pb;
  if (!nameable_params(a, pa) || !nameable_params(b, pb)) {
    return false;
  }
  for (FunctionI* fi : {a, b}) {
    for (unsigned int i = 0; i < fi->paramCount(); i++) {
      VarDecl* p = fi->param(i);
      if (p->e() != nullptr) {
        continue;  // has a default, so it is not in K*
      }
      auto ia = pa.find(p->id()->v());
      auto ib = pb.find(p->id()->v());
      if (ia == pa.end() || ib == pb.end()) {
        return false;  // K* is not nameable in both declarations
      }
      if (ia->second->type() != ib->second->type()) {
        return false;
      }
    }
  }
  return true;
}

// Does \a fi share its bucket \a v with a name-only sibling - another overload
// with identical parameter types but at least one differing (name-passable)
// parameter name? Such overloads can only be told apart by named arguments, so
// resolving \a fi through a type-only matchFn (which ignores names) is a latent
// bug: the caller should use the name-aware matchFn(Call*) / matchFnByNames /
// matchReifByNames instead. The type-only matchers throw a clear error rather
// than silently returning \a fi if this ever holds (it cannot for a library
// that passes the name-consistency checks).
bool has_name_only_sibling(const std::vector<Model::FnEntry>& v, FunctionI* fi) {
  for (const auto& fe : v) {
    if (fe.fi == fi || fe.fi->paramCount() != fi->paramCount()) {
      continue;
    }
    bool sameTypes = true;
    for (unsigned int i = 0; i < fi->paramCount(); i++) {
      if (fe.fi->param(i)->type() != fi->param(i)->type()) {
        sameTypes = false;
        break;
      }
    }
    if (sameTypes && !sameParameterNames(fe.fi, fi)) {
      return true;
    }
  }
  return false;
}

// Is \a a at least as specific as \a b on the first \a n parameters? Only the
// parameters an actual call supplies are compared; a defaulted tail is not part
// of the ranking, so gaining a defaulted parameter never makes an overload a
// worse match for the calls it already served.
bool at_least_as_specific(const EnvI& env, const std::vector<Type>& a, const std::vector<Type>& b,
                          unsigned int n) {
  for (unsigned int j = 0; j < n; j++) {
    if (a[j] != b[j] && !a[j].isSubtypeOf(env, b[j], true)) {
      return false;
    }
  }
  return true;
}
}  // namespace

void Model::addPolymorphicInstances(EnvI& env, Model::FnEntry& fe, std::vector<FnEntry>& entries) {
  auto addEntry = [&](Model::FnEntry& toAdd) {
    for (auto& entry : entries) {
      if (entry.t == toAdd.t) {
        // Identical types with different parameter names define distinct
        // named-arguments overloads (e.g. interval(start:, end:) vs
        // interval(start:, duration:)). Skip past such siblings rather
        // than collapsing them; they must coexist for named-argument
        // resolution to disambiguate. Polymorphic variants are excluded —
        // they share parameter names with their polymorphic source by
        // construction.
        if (!entry.isPolymorphicVariant && !toAdd.isPolymorphicVariant &&
            !sameParameterNames(entry.fi, toAdd.fi)) {
          continue;
        }
        bool more_specific = true;
        for (unsigned int i = 0; i < toAdd.fi->paramCount(); i++) {
          // If all parameters of the entry we are adding are subtypes of an
          // existing function, this one should take priority
          //
          // E.g. (bool, int) is preferred over ($T, $U)
          auto oldParamType = entry.fi->param(i)->type();
          auto newParamType = toAdd.fi->param(i)->type();
          if (!newParamType.isSubtypeOf(env, oldParamType, false)) {
            more_specific = false;
            break;
          }
        }
        if (more_specific) {
          entry = toAdd;
        }
        return;
      }
    }
    // Entry not yet added
    entries.push_back(toAdd);
  };

  addEntry(fe);
  if (fe.isPolymorphic) {
    GCLock lock;
    FnEntry cur = fe;
    cur.isPolymorphicVariant = true;

    /*

     Polymorphic functions use type variables $T that stand for concrete types.

     Depending on the inst and opt used with $T, it can stand for different types:

     $T           : any type (including set types)
     opt $T       : any type (including set types)
     var $T       : bool, int, float, set of int
     var opt $T   : bool, int, float
     set of $T    : bool, int, float
     var set of $T: int
     any $T       : any type, both par and var

     The types it can stand for are the intersection of these sets over all uses
     of $T.

     */

    std::unordered_map<ASTString, size_t> type_id_map;
    std::vector<TIIDInfo> type_ids;

    // Create a parameter TypeInst in the format of a tuple TypeInst
    // Immediately copy so the internal TypeInst values can be changed
    auto* paramtuple = Expression::cast<TypeInst>(copy(env, cur.fi->paramTypes()));
    paramtuple->collectTypeIds(type_id_map, type_ids);

    std::vector<size_t> stack;
    for (size_t i = 0; i < type_ids.size(); i++) {
      stack.push_back(i);
      for (auto* j : type_ids[i].t) {
        set_to_lowest(*j, type_ids[i].pbt);
      }
    }
    size_t final_id = type_ids.size() - 1;

    while (!stack.empty()) {
      if (stack.back() == final_id) {
        // New complete instance
        // First, update cur types
        auto* tis = Expression::cast<ArrayLit>(paramtuple->domain());
        for (unsigned int i = 0; i < tis->size(); ++i) {
          cur.t[i] = Expression::type((*tis)[i]);
          if (cur.t[i].bt() == Type::BT_TUPLE && cur.t[i].typeId() == 0) {
            env.registerTupleType(Expression::cast<TypeInst>((*tis)[i]));
            cur.t[i] = Expression::type((*tis)[i]);
          } else if (cur.t[i].bt() == Type::BT_RECORD && cur.t[i].typeId() == 0) {
            env.registerRecordType(Expression::cast<TypeInst>((*tis)[i]));
            cur.t[i] = Expression::type((*tis)[i]);
          }
        }
        addEntry(cur);
      }

      const Type& back_t = *type_ids[stack.back()].t[0];
      if (can_increment_type(back_t, type_ids[stack.back()].pbt)) {
        for (auto* i : type_ids[stack.back()].t) {
          increment_type(*i, type_ids[stack.back()].pbt);
        }
        // Reset types of all further items and push them
        for (size_t i = stack.back() + 1; i < type_ids.size(); i++) {
          for (auto* j : type_ids[i].t) {
            set_to_lowest(*j, type_ids[i].pbt);
          }
          stack.push_back(i);
        }
      } else {
        // last type, remove this item
        stack.pop_back();
      }
    }
  }
}

bool Model::registerFn(EnvI& env, FunctionI* fi, bool keepSorted, bool throwIfDuplicate) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  // A body-less declaration is the name-authority anchor for its overload family.
  // Record it now, before the merge below can replace it with a bodied override of
  // the same type (which would discard its parameter names).
  if (fi->e() == nullptr && !fi->isMonomorphised()) {
    std::vector<FnAnchor>& anchors = m->_fnAnchors[fi->id()];
    bool alreadyRecorded = false;
    for (const auto& a : anchors) {
      if (a.fi == fi) {
        alreadyRecorded = true;
        break;
      }
    }
    if (!alreadyRecorded) {
      anchors.push_back({fi});
    }
  }
  auto i_id = m->_fnmap.find(fi->id());
  if (i_id == m->_fnmap.end()) {
    // new element
    std::vector<FnEntry> v;
    FnEntry fe(env, fi);
    addPolymorphicInstances(env, fe, v);
    m->_fnmap.insert(std::pair<ASTString, std::vector<FnEntry>>(fi->id(), v));
  } else {
    // add to list of existing elements
    std::vector<FnEntry>& v = i_id->second;
    FnEntry fe(env, fi);  // Create now so that struct types get canonicalised
    for (auto& i : v) {
      if (i.fi == fi) {
        return true;
      }
      // Reject overloads that no call using named arguments could tell apart. Not guarded
      // by equal arity: parameter defaults let two such declarations differ in length.
      // Polymorphic variant entries share their FunctionI with the polymorphic declaration
      // they were generated from, which is in the bucket as well, so skipping them loses no
      // coverage. A par version registered by create_par_versions shares the parameter
      // names of the function it was copied from, hence the throwIfDuplicate guard.
      if (throwIfDuplicate && !i.isPolymorphicVariant && !exact_redeclaration(i.fi, fi) &&
          ambiguous_named_call(i.fi, fi)) {
        throw TypeError(
            env, fi->loc(),
            "ambiguous overloading: a call that supplies its arguments by name could match "
            "both this declaration and the one at " +
                i.fi->loc().toString() +
                ", because they agree on the name and type of every parameter that must be "
                "supplied");
      }
      if (i.fi->paramCount() == fi->paramCount()) {
        bool alleq = true;
        bool eqExceptInst = true;
        for (unsigned int j = 0; j < fi->paramCount(); j++) {
          Type t1 = i.fi->param(j)->type();
          Type t2 = fi->param(j)->type();
          if (t1 != t2) {
            alleq = false;
          }
          t1.mkPar(env);
          t2.mkPar(env);
          if (t1 != t2) {
            eqExceptInst = false;
          }
        }
        if (alleq) {
          if ((i.fi->e() != nullptr) && (fi->e() != nullptr) && !i.isPolymorphic) {
            if (!sameParameterNames(i.fi, fi)) {
              // Identical types but different parameter names: a distinct overload
              // under named-arguments. Fall through to addPolymorphicInstances so
              // this decl is inserted as its own entry rather than treated as a
              // duplicate of the existing one.
              continue;
            }
            if (throwIfDuplicate) {
              throw TypeError(
                  env, fi->loc(),
                  "function with the same type already defined in " + i.fi->loc().toString());
            }
            return false;
          }
          if ((fi->e() != nullptr) || i.isPolymorphic) {
            if (Call* deprecated = i.fi->ann().getCall(env.constants.ann.mzn_deprecated)) {
              fi->ann().add(deprecated);
            }
            FunctionI* old_fi = i.fi;
            if (!fe.isPolymorphic) {
              i = fe;
            }
            // If we are replacing a polymorphic function using a new polymorphic function, then
            // replace in all entries generated using addPolymorphicInstances
            if (i.isPolymorphic) {
              for (auto& j : v) {
                if (j.fi == old_fi) {
                  j.fi = fi;
                }
              }
            }
          } else if (Call* deprecated = fi->ann().getCall(env.constants.ann.mzn_deprecated)) {
            i.fi->ann().add(deprecated);
          }
          bool fromStdLib = i.fi->fromStdLib() || fi->fromStdLib();
          i.fi->fromStdLib(fromStdLib);
          fi->fromStdLib(fromStdLib);
          return true;
        }
        if (eqExceptInst) {
          Type t1 = i.fi->ti()->type();
          Type t2 = fi->ti()->type();
          t1.mkPar(env);
          t2.mkPar(env);
          t1.mkPresentDeep(env);
          t2.mkPresentDeep(env);
          if (t1.bt() == Type::BT_INT) {
            t1.typeId(0);
          }
          if (t2.bt() == Type::BT_INT) {
            t2.typeId(0);
          }
          if (t1 != t2) {
            throw TypeError(env, fi->loc(),
                            "function with same type up to par/var but different return type "
                            "already defined in " +
                                i.fi->loc().toString());
          }
        }
      }
    }
    addPolymorphicInstances(env, fe, v);
    if (keepSorted) {
      std::sort(v.begin(), v.end(), [&env](const Model::FnEntry& e1, const Model::FnEntry& e2) {
        return Model::FnEntry::compare(env, e1, e2);
      });
    }
  }
  if (fi->id() == env.constants.ids.mzn_reverse_map_var) {
    if (fi->paramCount() != 1 || fi->ti()->type() != Type::varbool()) {
      throw TypeError(env, fi->loc(),
                      "functions called `mzn_reverse_map_var` must have a single argument and "
                      "return type var bool");
    }
    Type t = fi->param(0)->type();
    _revmapmap.insert(std::pair<int, FunctionI*>(t.toInt(), fi));
  }
  return true;
}

bool Model::fnExists(EnvI& env, const ASTString& id) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto i_id = m->_fnmap.find(id);
  return i_id != m->_fnmap.end();
}

bool Model::isFnAnchored(EnvI& env, const FunctionI* fi) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnAnchors.find(fi->id());
  if (it == m->_fnAnchors.end()) {
    return false;
  }
  std::vector<Type> sig = normalized_param_sig(env, fi);
  for (const auto& a : it->second) {
    if (a.fi->paramCount() == fi->paramCount() && normalized_param_sig(env, a.fi) == sig) {
      return true;
    }
  }
  return false;
}

bool Model::hasAnchoredArityMatch(const ASTString& id, unsigned int nArgs) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnAnchors.find(id);
  if (it == m->_fnAnchors.end()) {
    return false;
  }
  for (const auto& a : it->second) {
    if (a.fi->paramCount() < nArgs) {
      continue;
    }
    // Any parameter past the supplied arguments must be defaulted for the call
    // to target this overload by arity.
    bool tailDefaulted = true;
    for (unsigned int j = nArgs; j < a.fi->paramCount(); j++) {
      if (a.fi->param(j)->e() == nullptr) {
        tailDefaulted = false;
        break;
      }
    }
    if (tailDefaulted) {
      return true;
    }
  }
  return false;
}

// NOTE: this type-only overload ignores parameter names. It is for resolving
// FRESH/internal calls only (compiler-minted builtins, registration). To
// re-resolve an EXISTING user call - which carries a decl whose parameter names
// matter under named-argument overloading - use matchFn(Call*), matchFnNamed,
// matchReifByNames or matchFnByNames instead, otherwise a name-only sibling can
// be silently substituted. The guard below throws if this overload ever lands
// on a name-only-overloaded function (a malformed library); see the note at
// has_name_only_sibling.
FunctionI* Model::matchFn(EnvI& env, const ASTString& id, const std::vector<Type>& t,
                          bool strictEnums) const {
  if (id == env.constants.varRedef->id()) {
    return env.constants.varRedef;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto i_id = m->_fnmap.find(id);
  if (i_id == m->_fnmap.end()) {
    return nullptr;
  }
  const std::vector<FnEntry>& v = i_id->second;
  for (const auto& i : v) {
    const std::vector<Type>& fi_t = i.t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
    std::cerr << "try " << *i.fi;
#endif
    if (fi_t.size() == t.size()) {
      bool match = true;
      for (unsigned int j = 0; j < t.size(); j++) {
        if (!env.isSubtype(t[j], fi_t[j], strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
          std::cerr << t[j].toString(env) << " does not match " << fi_t[j].toString(env) << "\n";
#endif
          match = false;
          break;
        }
      }
      if (match) {
        if (has_name_only_sibling(v, i.fi)) {
          throw InternalError(
              "type-only matchFn(id, types) resolved a name-only-overloaded function `" +
              std::string(id.c_str()) +
              "'; an existing-call re-match must go through a name-aware matcher. This indicates "
              "a library with name-only overloads that fail the parameter-name consistency "
              "checks.");
        }
        return i.fi;
      }
    }
  }
  return nullptr;
}

void Model::mergeStdLib(EnvI& env, Model* m) const {
  for (const auto& it : _fnmap) {
    for (const auto& cit : it.second) {
      if (cit.fi->fromStdLib() && !cit.isPolymorphicVariant) {
        (void)m->registerFn(env, cit.fi);
      }
    }
  }
  m->sortFn(env);
}

void Model::sortFn(const EnvI& env, FunctionI* fi) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto& it = *m->_fnmap.find(fi->id());
  // Sort all functions by type
  std::sort(it.second.begin(), it.second.end(),
            [&env](const Model::FnEntry& e1, const Model::FnEntry& e2) {
              return Model::FnEntry::compare(env, e1, e2);
            });
}

void Model::sortFn(const EnvI& env) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  for (auto& it : m->_fnmap) {
    // Sort all functions by type
    std::sort(it.second.begin(), it.second.end(),
              [&env](const Model::FnEntry& e1, const Model::FnEntry& e2) {
                return Model::FnEntry::compare(env, e1, e2);
              });
  }
}

void Model::fixFnMap(FunctionI* fi) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }

  auto& it = *m->_fnmap.find(fi->id());
  for (auto& i : it.second) {
    if (i.fi == fi) {
      for (unsigned int j = 0; j < i.t.size(); j++) {
        if (i.t[j].isunknown() || i.t[j].structBT()) {
          i.t[j] = i.fi->param(j)->type();
        }
      }
    }
  }
}

void Model::checkFnValid(EnvI& env, std::vector<TypeError>& errors) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  // Walk through registered functions check that functions that contain a non-FlatZinc type
  // argument are either a builtin (registed with a C++ function), or have a body.
  for (const auto& it : m->_fnmap) {
    const std::vector<FnEntry>& variants = it.second;
    for (const auto& fe : variants) {
      FunctionI* fi = fe.fi;
      Type ret = fi->ti()->type();
      if (fi->e() != nullptr || fi->builtins.e != nullptr || fi->builtins.b != nullptr ||
          fi->builtins.f != nullptr || fi->builtins.i != nullptr || fi->builtins.s != nullptr ||
          fi->builtins.fs != nullptr || fi->builtins.str != nullptr ||
          fi->ann().contains(env.constants.ann.mzn_internal_representation)) {
        continue;
      }
      if (fi->ann().contains(env.constants.ann.output_only)) {
        std::vector<Type> tys(fi->paramCount());
        for (unsigned int i = 0; i < fi->paramCount(); ++i) {
          tys[i] = fi->param(i)->type();
          tys[i].mkPar(env);
        }
        fi = matchFn(env, fi->id(), tys, true);
        ret.mkPar(env);
        if (fi != nullptr && ret == fi->ti()->type()) {
          continue;
        }
        errors.emplace_back(env, fe.fi->loc(),
                            "Missing parameter type variant of output only function");
        continue;
      }
      // Par functions that are not implemented in the compiler should have an implementation
      if (!ret.isAnn() && ret.isPar()) {
        errors.emplace_back(env, fi->loc(),
                            "Parameter type function is missing its implementation");
        continue;
      }
      if (!ret.isAnn() && !ret.isvarbool()) {
        errors.emplace_back(
            env, fi->loc(),
            "FlatZinc builtin functions must be predicates (i.e., have `var bool` return type)");
        continue;
      }
      for (unsigned int i = 0; i < fi->paramCount(); ++i) {
        const Type& t = fi->param(i)->type();
        if (t.isOpt() || t.structBT() || t.bt() == Type::BT_TOP) {
          errors.emplace_back(
              env, Expression::loc(fi->param(i)),
              "FlatZinc builtins are not allowed to have arguments of type " + t.toString(env));
          break;  // Break from parameter, but does continue in FnMap
        }
        if (t.dim() > 1) {
          errors.emplace_back(env, Expression::loc(fi->param(i)),
                              "Type " + t.toString(env) +
                                  " is not allowed in as a FlatZinc builtin argument, arrays must "
                                  "be one dimensional");
          break;  // Break from parameter, but does continue in FnMap
        }
      }
    }
  }
}

void Model::checkFnOverloading(EnvI& env) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  for (auto& it : m->_fnmap) {
    std::vector<FnEntry>& fs = it.second;
    for (unsigned int i = 0; i < fs.size() - 1; i++) {
      FunctionI* cur = fs[i].fi;
      for (unsigned int j = i + 1; j < fs.size(); j++) {
        FunctionI* cmp = fs[j].fi;
        if (cur == cmp || cur->paramCount() != cmp->paramCount()) {
          break;
        }
        bool allEqual = true;
        for (unsigned int i = 0; i < cur->paramCount(); i++) {
          Type t1 = cur->param(i)->type();
          Type t2 = cmp->param(i)->type();
          if (t1.bt() == Type::BT_INT) {
            t1.typeId(0);
          }
          if (t2.bt() == Type::BT_INT) {
            t2.typeId(0);
          }
          if (t1 != t2) {
            allEqual = false;
            break;
          }
        }
        if (allEqual && sameParameterNames(cur, cmp)) {
          throw TypeError(env, cur->loc(),
                          "unsupported type of overloading. \nFunction/predicate with equivalent "
                          "signature defined in " +
                              cmp->loc().toString());
        }
      }
    }
  }
}

namespace {
int match_idx(std::vector<FunctionI*>& matched, Expression*& botarg, EnvI& env,
              const std::vector<Model::FnEntry>& v, const std::vector<Expression*>& args,
              bool strictEnums) {
  botarg = nullptr;
  for (unsigned int i = 0; i < v.size(); i++) {
    const std::vector<Type>& fi_t = v[i].t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
    std::cerr << "try " << *v[i].fi;
#endif
    if (fi_t.size() == args.size()) {
      bool match = true;
      for (unsigned int j = 0; j < args.size(); j++) {
        if (!env.isSubtype(Expression::type(args[j]), fi_t[j], strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
          std::cerr << Expression::type(args[j]).toString(env) << " does not match "
                    << fi_t[j].toString(env) << "\n";
#endif
          match = false;
          break;
        }
        if (Expression::type(args[j]).isbot() && fi_t[j].bt() != Type::BT_TOP) {
          botarg = args[j];
        }
      }
      if (match) {
        matched.push_back(v[i].fi);
        if (botarg == nullptr) {
          return static_cast<int>(i);
        }
      }
    }
  }
  return -1;
}
}  // namespace

FunctionI* Model::matchReification(EnvI& env, const ASTString& id,
                                   const std::vector<Expression*>& args, bool canHalfReify,
                                   bool strictEnums) const {
  std::vector<Type> t;
  t.reserve(args.size());
  for (const auto* e : args) {
    t.push_back(Expression::type(e));
  }
  return this->matchReification(env, id, t, canHalfReify, strictEnums);
}

FunctionI* Model::matchReification(EnvI& env, const ASTString& id, const std::vector<Type>& t,
                                   bool canHalfReify, bool strictEnums) const {
  ASTString reif_id = env.reifyId(id);
  FunctionI* reif_decl = this->matchFn(env, reif_id, t, strictEnums);
  if (canHalfReify) {
    ASTString imp_id = EnvI::halfReifyId(id);
    if (FunctionI* imp_decl = this->matchFn(env, imp_id, t, strictEnums)) {
      // If reification failed, then go with half-reification immediately
      if (reif_decl == nullptr) {
        return imp_decl;
      }

      // If there is both a reification and a half-reification, then make sure the half-reification
      // is at least as specific as the reification. That is the parameters of the matching
      // half-reification declaration should be the same or subtypes of the matching reification.
      assert(imp_decl->paramCount() == reif_decl->paramCount());
      for (unsigned int i = 0; i < imp_decl->paramCount(); ++i) {
        Type a = imp_decl->param(i)->ti()->type();
        Type b = reif_decl->param(i)->ti()->type();
        if (!env.isSubtype(a, b, strictEnums)) {
          return reif_decl;
        }
      }
      return imp_decl;
    }
  }
  return reif_decl;
}

FunctionI* Model::matchReifByNames(EnvI& env, const Call* c, bool canHalfReify,
                                   bool strictEnums) const {
  if (c->decl() == nullptr) {
    return nullptr;
  }
  // An anchored (builtin) family is positional-only: its reif/imp variants are
  // resolved by type alone. Return null so the caller's type-only
  // matchReification fallback takes over, reproducing the pre-named-args
  // behaviour regardless of any solver-library renaming.
  if (isFnAnchored(env, c->decl())) {
    return nullptr;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  FunctionI* baseDecl = c->decl();
  unsigned int baseArity = baseDecl->paramCount();
  // A reif/imp variant is chosen only when it matches the base declaration on BOTH the
  // leading parameter names and the argument types. The name match keeps name-only sibling
  // overloads apart (a differently-named variant belongs to a different sibling); the type
  // match must also hold, otherwise a same-named but type-incompatible variant would be
  // returned and later fail to resolve (e.g. a non-opt `int_eq_imp` picked for an opt call).
  auto pickNameMatch = [&](const ASTString& bucket_id) -> FunctionI* {
    auto it = m->_fnmap.find(bucket_id);
    if (it == m->_fnmap.end()) {
      return nullptr;
    }
    for (const auto& fe : it->second) {
      if (fe.fi->paramCount() != baseArity + 1) {
        continue;
      }
      bool nameMatch = true;
      for (unsigned int i = 0; i < baseArity; i++) {
        Id* a = fe.fi->param(i)->id();
        Id* b = baseDecl->param(i)->id();
        if (!a->hasStr() || !b->hasStr()) {
          continue;
        }
        if (a->v() != b->v()) {
          nameMatch = false;
          break;
        }
      }
      if (!nameMatch) {
        continue;
      }
      bool typeMatch = true;
      for (unsigned int i = 0; i < baseArity; i++) {
        if (!env.isSubtype(Expression::type(c->arg(i)), fe.t[i], strictEnums)) {
          typeMatch = false;
          break;
        }
      }
      if (!typeMatch) {
        continue;
      }
      return fe.fi;
    }
    return nullptr;
  };
  ASTString reif_id = env.reifyId(c->id());
  FunctionI* reif_decl = pickNameMatch(reif_id);
  if (canHalfReify) {
    ASTString imp_id = EnvI::halfReifyId(c->id());
    if (FunctionI* imp_decl = pickNameMatch(imp_id)) {
      if (reif_decl == nullptr) {
        return imp_decl;
      }
      assert(imp_decl->paramCount() == reif_decl->paramCount());
      for (unsigned int i = 0; i < imp_decl->paramCount(); ++i) {
        Type a = imp_decl->param(i)->ti()->type();
        Type b = reif_decl->param(i)->ti()->type();
        if (!env.isSubtype(a, b, strictEnums)) {
          return reif_decl;
        }
      }
      return imp_decl;
    }
  }
  return reif_decl;
}

FunctionI* Model::matchFnByNames(EnvI& env, const ASTString& id, FunctionI* baseDecl,
                                 const std::vector<Type>& t, bool strictEnums) const {
  // Re-resolve an existing call by parameter NAMES, not by type alone. Given the
  // call's resolved \a baseDecl, find the overload registered under \a id whose
  // leading baseDecl->paramCount() parameter names match baseDecl's and whose
  // registered types accept \a t. This is the name-aware counterpart of the
  // type-only matchFn(id, t) used when a call is rewritten into a wider form
  // (function->relation conversion appends a result argument; par-version
  // construction keeps the arity) and the original decl must be honoured so a
  // name-only sibling is not silently substituted. Names are read directly off
  // baseDecl - no name vector is materialised. \a t may be longer than baseDecl
  // (the trailing positions, e.g. an appended result, are matched on type only).
  // Returns nullptr if baseDecl is null or nothing matches; falls back to a
  // name-matching-but-not-type-matching candidate only as a last resort,
  // mirroring matchReifByNames.
  if (baseDecl == nullptr) {
    return nullptr;
  }
  // An anchored (builtin) family is positional-only: its wider (relational or
  // par) forms are resolved by type alone. Return null so the caller's
  // type-only matchFn fallback takes over, reproducing the pre-named-args
  // behaviour regardless of any solver-library renaming.
  if (isFnAnchored(env, baseDecl)) {
    return nullptr;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(id);
  if (it == m->_fnmap.end()) {
    return nullptr;
  }
  const unsigned int baseArity = baseDecl->paramCount();
  // A candidate is chosen only when it matches the base declaration on BOTH the leading
  // parameter names and the argument types. The name match keeps name-only sibling
  // overloads apart; the type match must also hold, otherwise a same-named but
  // type-incompatible candidate would be returned and later fail to resolve.
  for (const auto& fe : it->second) {
    if (fe.fi->paramCount() != t.size() || fe.fi->paramCount() < baseArity) {
      continue;
    }
    bool nameMatch = true;
    for (unsigned int i = 0; i < baseArity; i++) {
      Id* a = fe.fi->param(i)->id();
      Id* b = baseDecl->param(i)->id();
      if (!a->hasStr() || !b->hasStr()) {
        // Anonymous / synthetic ids cannot be passed by name; defer.
        continue;
      }
      if (a->v() != b->v()) {
        nameMatch = false;
        break;
      }
    }
    if (!nameMatch) {
      continue;
    }
    bool typeMatch = true;
    for (unsigned int i = 0; i < t.size(); i++) {
      if (!env.isSubtype(t[i], fe.t[i], strictEnums)) {
        typeMatch = false;
        break;
      }
    }
    if (!typeMatch) {
      continue;
    }
    return fe.fi;
  }
  return nullptr;
}

FunctionI* Model::matchFnByNames(EnvI& env, const ASTString& id, FunctionI* baseDecl,
                                 const std::vector<Expression*>& args, bool strictEnums) const {
  std::vector<Type> t;
  t.reserve(args.size());
  for (const auto* e : args) {
    t.push_back(Expression::type(e));
  }
  return this->matchFnByNames(env, id, baseDecl, t, strictEnums);
}

FunctionI* Model::matchParVersion(EnvI& env, FunctionI* f, const std::vector<Type>& tv,
                                  bool strictEnums) const {
  // Find the par version of \a f - the overload under f's identifier whose
  // (par-coerced) types \a tv accept. This is a type match (so f's genuine
  // coercion par sibling, which differs in inst and may even differ in
  // parameter names, is found) with one correction: a candidate that has
  // *identical* parameter types to f but different parameter names is a
  // name-only sibling - a distinct overload with its own body, dispatched via
  // named arguments - not a par version of f, so it is redirected to f. That
  // keeps name-only siblings keyed separately in the par-version pass instead
  // of collapsing onto one shared copy. Always returns a non-null result (at
  // worst f itself, which accepts its own par-coerced types).
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(f->id());
  if (it == m->_fnmap.end()) {
    return f;
  }
  for (const auto& fe : it->second) {
    if (fe.t.size() != tv.size()) {
      continue;
    }
    bool match = true;
    for (unsigned int j = 0; j < tv.size(); j++) {
      if (!env.isSubtype(tv[j], fe.t[j], strictEnums)) {
        match = false;
        break;
      }
    }
    if (!match) {
      continue;
    }
    FunctionI* fi_par = fe.fi;
    if (fi_par != f && fi_par->paramCount() == f->paramCount()) {
      bool sameTypes = true;
      for (unsigned int j = 0; j < f->paramCount(); j++) {
        if (fi_par->param(j)->type() != f->param(j)->type()) {
          sameTypes = false;
          break;
        }
      }
      if (sameTypes && !sameParameterNames(fi_par, f)) {
        fi_par = f;
      }
    }
    return fi_par;
  }
  return f;
}

// NOTE: type-only overload - see the matchFn(id, types) note above. For
// FRESH/internal calls only; existing-call re-matches must use a name-aware
// matcher. The debug-build assert trips on a name-only-overloaded resolution.
FunctionI* Model::matchFn(EnvI& env, const ASTString& id, const std::vector<Expression*>& args,
                          bool strictEnums) const {
  if (id == env.constants.varRedef->id()) {
    return env.constants.varRedef;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(id);
  if (it == m->_fnmap.end()) {
    return nullptr;
  }
  const std::vector<FnEntry>& v = it->second;
  std::vector<FunctionI*> matched;
  Expression* botarg;
  (void)match_idx(matched, botarg, env, v, args, strictEnums);
  if (matched.empty()) {
    return nullptr;
  }
  if (has_name_only_sibling(v, matched[0])) {
    throw InternalError(
        "type-only matchFn(id, args) resolved a name-only-overloaded function `" +
        std::string(id.c_str()) +
        "'; an existing-call re-match must go through a name-aware matcher. This indicates a "
        "library with name-only overloads that fail the parameter-name consistency checks.");
  }
  if (matched.size() == 1) {
    return matched[0];
  }
  auto t = matched[0]->rtype(env, args, nullptr, false);
  t.mkPar(env);
  for (unsigned int i = 1; i < matched.size(); i++) {
    auto rt = matched[i]->rtype(env, args, nullptr, false);
    if (!env.isSubtype(t, rt, strictEnums)) {
      throw TypeError(env, Expression::loc(botarg),
                      "ambiguous overloading on return type of function");
    }
  }
  return matched[0];
}

FunctionI* Model::matchFn(EnvI& env, Call* c, bool strictEnums, bool throwIfNotFound) const {
  if (c->id() == env.constants.varRedef->id()) {
    return env.constants.varRedef;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(c->id());
  if (it == m->_fnmap.end()) {
    if (throwIfNotFound) {
      std::ostringstream oss;
      oss << "no function or predicate with name `";
      oss << c->id() << "' found";

      ASTString mostSimilar;
      int minEdits = 3;
      for (const auto& decls : m->_fnmap) {
        if (std::abs(static_cast<int>(c->id().size()) - static_cast<int>(decls.first.size())) <=
            3) {
          int edits = c->id().levenshteinDistance(decls.first);
          if (edits < minEdits && edits < std::min(c->id().size(), decls.first.size())) {
            minEdits = edits;
            mostSimilar = decls.first;
          }
        }
      }
      if (!mostSimilar.empty()) {
        oss << ", did you mean `" << mostSimilar << "'?";
      }
      throw TypeError(env, Expression::loc(c), oss.str());
    }
    return nullptr;
  }
  const std::vector<FnEntry>& v = it->second;
  if (v.size() == 1 && v[0].fi == c->decl()) {
    return c->decl();
  }
  // An anchored (builtin) family is positional-only: re-resolve it by type
  // alone. Such a family has no name-only siblings (the checks forbid them), so
  // the name-skip and name-tie-break branches below are already inert for it;
  // the guard states that invariant explicitly and keeps a solver-library
  // renaming resolving exactly as it did before named arguments existed.
  const bool anchored = c->decl() != nullptr && isFnAnchored(env, c->decl());
  std::vector<FunctionI*> matched;
  // Most specific viable candidate so far, ranked on the supplied arguments
  // only. The bucket is sorted by FnEntry::compare, whose primary key is arity,
  // so bucket order alone would let a more general overload with exactly the
  // call's arity win over a more specific one whose trailing parameters have
  // defaults. Arity decides viability; specificity decides the winner.
  const FnEntry* best = nullptr;
  // Ranking only has to look past the first match when some overload can absorb
  // extra arguments through defaults, i.e. when one declares more parameters
  // than the call supplies. Arity is the sort's primary key, so the last entry
  // has the largest one and this settles it in O(1). Without such an overload
  // every viable candidate has exactly the call's arity, and the bucket already
  // orders those most-concrete-first, so the first match is the most specific
  // and the scan can stop there as it always did.
  const bool mayDefaultFill = !v.empty() && v.back().t.size() > c->argCount();
  Expression* botarg = nullptr;
  for (const auto& i : v) {
    if (!anchored && c->decl() != nullptr && i.fi != c->decl() &&
        i.fi->paramCount() == c->decl()->paramCount() && !sameParameterNames(i.fi, c->decl())) {
      // Skip a candidate that is a genuine name-only sibling of the call's
      // resolved decl (identical parameter types, different parameter names):
      // re-resolution by type must not silently switch to it. (Candidates that
      // differ in var/par or optionality - e.g. the auto-generated par copy of
      // a sibling - are not skipped here but tie-broken by name on the return
      // path below.)
      bool sameTypes = true;
      for (unsigned int j = 0; j < i.fi->paramCount(); j++) {
        if (i.fi->param(j)->type() != c->decl()->param(j)->type()) {
          sameTypes = false;
          break;
        }
      }
      if (sameTypes) {
        continue;
      }
    }
    const std::vector<Type>& fi_t = i.t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
    std::cerr << "try " << *i.fi;
#endif
    // A candidate matches when it has at least as many parameters as the call
    // has arguments, and every parameter beyond the supplied arguments has a
    // default. Defaults are spliced into the call by the typechecker (vCall),
    // so re-resolution after flattening sees a full-arity call and takes the
    // exact-arity path.
    bool defaultedTail = fi_t.size() >= c->argCount();
    for (unsigned int j = c->argCount(); defaultedTail && j < i.fi->paramCount(); j++) {
      if (i.fi->param(j)->e() == nullptr) {
        defaultedTail = false;
      }
    }
    if (defaultedTail) {
      bool match = true;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        if (!env.isSubtype(Expression::type(c->arg(j)), fi_t[j], strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
          std::cerr << Expression::type(c->arg(j)).toString(env) << " does not match "
                    << fi_t[j].toString(env) << "\n";
          std::cerr << "Wrong argument is " << *c->arg(j) << "\n";
#endif
          match = false;
          break;
        }
        if (Expression::type(c->arg(j)).isbot() && fi_t[j].bt() != Type::BT_TOP) {
          botarg = c->arg(j);
        }
      }
      if (match) {
        if (botarg != nullptr) {
          matched.push_back(i.fi);
        } else if (best == nullptr) {
          best = &i;
          if (!mayDefaultFill) {
            break;
          }
        } else if (at_least_as_specific(env, i.t, best->t, c->argCount()) &&
                   !at_least_as_specific(env, best->t, i.t, c->argCount())) {
          // Strictly more specific than the incumbent on the supplied
          // arguments. Incomparable candidates leave the incumbent in place, so
          // bucket order still breaks ties exactly as it used to.
          best = &i;
        }
      }
    }
  }
  if (best != nullptr) {
    const auto& i = *best;
        // Tie-break by parameter name among identically-typed candidates: if
        // the first (most concrete) match has different parameter names than
        // the call's resolved decl, prefer an equally-typed sibling whose
        // names match decl. These are name-only overloads and their
        // auto-generated par/present copies (e.g. two par copies, both
        // `(par int)`, differing only in a parameter name); the bucket-order
        // first match could otherwise run the wrong sibling's body. The
        // alternative must have identical registered types, so concreteness
        // is never overridden; if none exists the first match is returned
        // unchanged, leaving genuinely name-disagreeing overloads untouched.
        if (!anchored && c->decl() != nullptr && i.fi != c->decl() &&
            i.fi->paramCount() == c->decl()->paramCount() &&
            !sameParameterNames(i.fi, c->decl())) {
          for (const auto& i2 : v) {
            if (i2.fi != i.fi && i2.t == i.t &&
                i2.fi->paramCount() == c->decl()->paramCount() &&
                sameParameterNames(i2.fi, c->decl())) {
              return i2.fi;
            }
          }
        }
        if (throwIfNotFound && c->decl() == nullptr) {
          // Fresh resolution of a positional call that did not name enough
          // arguments to be unambiguous: if another overload has identical
          // parameter types but different parameter names (a name-only
          // sibling), the call could run either body and the choice would
          // depend on registration order. Such overloads are only
          // distinguishable through named arguments, so require them rather
          // than silently picking one.
          for (const auto& i2 : v) {
            if (i2.fi != i.fi && i2.t == i.t && !sameParameterNames(i2.fi, i.fi)) {
              std::ostringstream oss;
              oss << "call to `" << c->id()
                  << "' is ambiguous: it matches overloads that differ only in "
                     "parameter names (defined in "
                  << i.fi->loc().toString() << " and " << i2.fi->loc().toString()
                  << "). Use named arguments to select one.";
              throw TypeError(env, Expression::loc(c), oss.str());
            }
          }
        }
    return i.fi;
  }
  if (matched.empty()) {
    if (throwIfNotFound) {
      std::ostringstream oss;
      oss << "no function or predicate with this signature found: `";
      oss << c->id() << "(";
      for (unsigned int i = 0; i < c->argCount(); i++) {
        oss << Expression::type(c->arg(i)).toString(env);
        if (i < c->argCount() - 1) {
          oss << ",";
        }
      }
      oss << ")'\n";
      oss << "Cannot use the following functions or predicates with the same identifier:\n";
      Printer pp(oss, 0, false, &env);
      for (const auto& i : v) {
        if (i.fi->isMonomorphised() || i.isPolymorphicVariant) {
          continue;
        }
        const std::vector<Type>& fi_t = i.t;
        Expression* body = i.fi->e();
        i.fi->e(nullptr);
        pp.print(i.fi);
        i.fi->e(body);
        if (fi_t.size() == c->argCount()) {
          for (unsigned int j = 0; j < c->argCount(); j++) {
            if (!env.isSubtype(Expression::type(c->arg(j)), fi_t[j], strictEnums)) {
              oss << "    (argument " << (j + 1) << " expects type " << fi_t[j].toString(env);
              oss << ", but type " << Expression::type(c->arg(j)).toString(env) << " given)\n";
            }
          }
        } else {
          oss << "    (requires " << i.fi->paramCount() << " argument"
              << (i.fi->paramCount() == 1 ? "" : "s") << ", but " << c->argCount() << " given)\n";
        }
      }
      throw TypeError(env, Expression::loc(c), oss.str());
    }
    return nullptr;
  }
  if (matched.size() == 1) {
    return matched[0];
  }
  Type t = matched[0]->ti()->type();
  t.mkPar(env);
  for (unsigned int i = 1; i < matched.size(); i++) {
    if (!env.isSubtype(t, matched[i]->ti()->type(), strictEnums)) {
      throw TypeError(env, Expression::loc(botarg),
                      "ambiguous overloading on return type of function");
    }
  }
  return matched[0];
}

namespace {
// Find the index of the parameter named \a name in \a fi at positions
// [from, fi->paramCount()). Returns -1 if not found.
int find_param_named(FunctionI* fi, const ASTString& name, unsigned int from) {
  for (unsigned int i = from; i < fi->paramCount(); i++) {
    if (fi->param(i)->id()->v() == name) {
      return static_cast<int>(i);
    }
  }
  return -1;
}
// Check whether any parameter in \a fi at positions [0, until) is named
// \a name.
bool positional_prefix_has_name(FunctionI* fi, const ASTString& name, unsigned int until) {
  for (unsigned int i = 0; i < until; i++) {
    if (fi->param(i)->id()->v() == name) {
      return true;
    }
  }
  return false;
}
}  // namespace

FunctionI* Model::matchFnNamed(EnvI& env, Call* c, const std::vector<Expression*>& positional,
                               const std::vector<std::pair<ASTString, Expression*>>& named,
                               bool strictEnums, bool throwIfNotFound, bool skipAnchored) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(c->id());
  if (it == m->_fnmap.end()) {
    if (throwIfNotFound) {
      std::ostringstream oss;
      oss << "no function or predicate with name `" << c->id() << "' found";
      throw TypeError(env, Expression::loc(c), oss.str());
    }
    return nullptr;
  }
  const std::vector<FnEntry>& v = it->second;
  const unsigned int k = static_cast<unsigned int>(positional.size());
  const unsigned int total = k + static_cast<unsigned int>(named.size());

  std::vector<FunctionI*> matched;
  Expression* botarg = nullptr;
  bool anyNameCompatible = false;
  // A candidate is name-compatible when (a) it has at least as many parameters
  // as the call supplies, (b) every supplied named name is at some index >= k
  // and does not collide with the positional prefix, and (c) every parameter
  // not bound by position or name has a default (the typechecker's vCall
  // splices those defaults into the call). This depends only on names/arity,
  // not argument types, so name-only siblings differ on it.
  auto nameCompatible = [&](FunctionI* fi) -> bool {
    if (fi->paramCount() < total) {
      return false;
    }
    for (const auto& np : named) {
      if (positional_prefix_has_name(fi, np.first, k)) {
        return false;
      }
      if (find_param_named(fi, np.first, k) < 0) {
        return false;
      }
    }
    for (unsigned int j = k; j < fi->paramCount(); j++) {
      bool boundByName = false;
      for (const auto& np : named) {
        if (fi->param(j)->id()->v() == np.first) {
          boundByName = true;
          break;
        }
      }
      if (!boundByName && fi->param(j)->e() == nullptr) {
        return false;
      }
    }
    return true;
  };
  // Is \a a at least as specific as \a b on the arguments this call supplies?
  // Mirrors the positional matchFn's ranking, but a named argument may sit at a
  // different index in each candidate, so each is asked where the name binds.
  // Parameters left to their defaults are not part of the ranking.
  auto atLeastAsSpecific = [&](const FnEntry& a, const FnEntry& b) -> bool {
    for (unsigned int i = 0; i < k; i++) {
      if (a.t[i] != b.t[i] && !a.t[i].isSubtypeOf(env, b.t[i], true)) {
        return false;
      }
    }
    for (const auto& np : named) {
      const int ia = find_param_named(a.fi, np.first, k);
      const int ib = find_param_named(b.fi, np.first, k);
      if (ia < 0 || ib < 0) {
        return false;
      }
      if (a.t[ia] != b.t[ib] && !a.t[ia].isSubtypeOf(env, b.t[ib], true)) {
        return false;
      }
    }
    return true;
  };
  // Most specific name-compatible candidate so far; see the positional matchFn
  // for why bucket order alone is not a ranking, and for the O(1) guard.
  const FnEntry* best = nullptr;
  const bool mayDefaultFill = !v.empty() && v.back().t.size() > total;
  for (const auto& fe : v) {
    FunctionI* fi = fe.fi;
    // Builtins (body-less-anchored families) are positional-only: skip them so
    // the call resolves only among user and bodied-library overloads. The
    // front-end gate rejects the call with a clear message if nothing else
    // matches.
    if (skipAnchored && isFnAnchored(env, fi)) {
      continue;
    }
    if (!nameCompatible(fi)) {
      continue;
    }
    anyNameCompatible = true;
    // Subtyping: positional[i] against fe.t[i]; named[(n, e)] against
    // fe.t[idx_of(n)].
    bool subOk = true;
    Expression* localBotarg = nullptr;
    for (unsigned int i = 0; i < k; i++) {
      if (!env.isSubtype(Expression::type(positional[i]), fe.t[i], strictEnums)) {
        subOk = false;
        break;
      }
      if (Expression::type(positional[i]).isbot() && fe.t[i].bt() != Type::BT_TOP) {
        localBotarg = positional[i];
      }
    }
    if (!subOk) {
      continue;
    }
    for (const auto& np : named) {
      int idx = find_param_named(fi, np.first, k);
      assert(idx >= 0);
      if (!env.isSubtype(Expression::type(np.second), fe.t[idx], strictEnums)) {
        subOk = false;
        break;
      }
      if (Expression::type(np.second).isbot() && fe.t[idx].bt() != Type::BT_TOP) {
        localBotarg = np.second;
      }
    }
    if (!subOk) {
      continue;
    }
    if (localBotarg == nullptr) {
      if (best == nullptr) {
        best = &fe;
        if (!mayDefaultFill) {
          break;
        }
      } else if (atLeastAsSpecific(fe, *best) && !atLeastAsSpecific(*best, fe)) {
        best = &fe;
      }
      continue;
    }
    botarg = localBotarg;
    matched.push_back(fi);
  }
  if (best != nullptr) {
    FunctionI* fi = best->fi;
    const auto& fe = *best;
      // An ambiguous named call is always a hard error, independent of
      // throwIfNotFound: if another candidate has identical parameter types but
      // different names (a name-only sibling) and is equally name-compatible
      // with this call, the choice between them depends only on registration
      // order. Require enough named arguments to disambiguate instead of
      // silently picking one. (Identical types make the subtype check above
      // redundant for the sibling, so only its name-compatibility needs
      // re-checking.) This must not be gated by throwIfNotFound: the front-end
      // gate probes with throwIfNotFound=false, and an ambiguous call must still
      // fail rather than resolve to whichever sibling comes first. A skipAnchored
      // probe ignores builtin siblings, which cannot be named anyway.
      for (const auto& fe2 : v) {
        if (fe2.fi != fi && fe2.t == fe.t && !sameParameterNames(fe2.fi, fi) &&
            nameCompatible(fe2.fi) && !(skipAnchored && isFnAnchored(env, fe2.fi))) {
          std::ostringstream oss;
          oss << "call to `" << c->id()
              << "' is ambiguous: it matches overloads that differ only in "
                 "parameter names (defined in "
              << fi->loc().toString() << " and " << fe2.fi->loc().toString()
              << "). Name more arguments to select one.";
          throw TypeError(env, Expression::loc(c), oss.str());
        }
      }
    return fi;
  }
  if (matched.empty()) {
    if (throwIfNotFound) {
      std::ostringstream oss;
      oss << "no function or predicate with this signature found: `" << c->id() << "(";
      bool first = true;
      for (unsigned int i = 0; i < k; i++) {
        if (!first) {
          oss << ",";
        }
        oss << Expression::type(positional[i]).toString(env);
        first = false;
      }
      for (const auto& np : named) {
        if (!first) {
          oss << ",";
        }
        oss << np.first << ": " << Expression::type(np.second).toString(env);
        first = false;
      }
      oss << ")'";
      if (!anyNameCompatible) {
        oss << "\nNo overload of `" << c->id()
            << "' has parameters matching the supplied named arguments.";
      }
      throw TypeError(env, Expression::loc(c), oss.str());
    }
    return nullptr;
  }
  if (matched.size() == 1) {
    return matched[0];
  }
  Type t = matched[0]->ti()->type();
  t.mkPar(env);
  for (unsigned int i = 1; i < matched.size(); i++) {
    if (!env.isSubtype(t, matched[i]->ti()->type(), strictEnums)) {
      throw TypeError(env,
                      Expression::loc(botarg != nullptr ? botarg : static_cast<Expression*>(c)),
                      "ambiguous overloading on return type of function");
    }
  }
  return matched[0];
}

void Model::checkSiblingParameterNames(EnvI& env) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }

  // Do two equal-arity decls disagree on any name-passable parameter?
  auto namesDisagree = [](const FunctionI* fa, const FunctionI* fb) {
    for (unsigned int j = 0; j < fa->paramCount(); j++) {
      const Id* a = fa->param(j)->id();
      const Id* b = fb->param(j)->id();
      // Anonymous / integer-id parameters cannot be passed by name, so a
      // name disagreement on them is irrelevant.
      if (a->hasStr() && b->hasStr() && a->v() != b->v()) {
        return true;
      }
    }
    return false;
  };
  // Exact per-parameter type equality (inst/opt/enum included, names ignored).
  auto typesEqual = [](const FunctionI* fa, const FunctionI* fb) {
    for (unsigned int j = 0; j < fa->paramCount(); j++) {
      if (fa->param(j)->type() != fb->param(j)->type()) {
        return false;
      }
    }
    return true;
  };
  // Is `y` a *strict narrowing* of `x`, i.e. are they coercion siblings (equal
  // up to inst/opt and enum/type-inst identity) with `y` everywhere at least as
  // fixed as `x` (var->par, opt->present), strictly so in at least one
  // parameter? Such a `y` is what a call to `x` is re-resolved to when its
  // arguments are narrowed during flattening. Precondition: equal paramCount.
  auto isStrictNarrowing = [&env](const FunctionI* x, const FunctionI* y) {
    bool strict = false;
    for (unsigned int j = 0; j < x->paramCount(); j++) {
      Type tx = x->param(j)->type();
      Type ty = y->param(j)->type();
      Type nx = tx;
      Type ny = ty;
      nx.mkPar(env);
      ny.mkPar(env);
      nx.mkPresentDeep(env);
      ny.mkPresentDeep(env);
      // Normalise away enum / type-inst-variable identity: each decl's
      // `$$E`/`$T`/enum carries a distinct typeId, which is not an inst or
      // optionality difference. Clearing it lets polymorphic siblings
      // (e.g. `min(set of $$E)` par vs var) compare equal.
      nx.typeId(0);
      ny.typeId(0);
      if (nx != ny) {
        return false;  // not a coercion sibling
      }
      if (tx.isPar() && ty.isvar()) {
        return false;  // y less fixed -> not a narrowing
      }
      if (!tx.isOpt() && ty.isOpt()) {
        return false;  // y less present -> not a narrowing
      }
      if ((tx.isvar() && ty.isPar()) || (tx.isOpt() && !ty.isOpt())) {
        strict = true;
      }
    }
    return strict;
  };

  for (const auto& bucket : m->_fnmap) {
    // Compare only the source declarations, skipping generated polymorphic
    // instances (which would duplicate warnings and point at synthetic decls).
    // The bucket is sorted by FnEntry::compare: arity ascending (primary key),
    // then more-concrete-types-first. Filtering preserves that order.
    std::vector<FunctionI*> decls;
    for (const auto& fe : bucket.second) {
      if (!fe.isPolymorphicVariant && !fe.fi->isMonomorphised()) {
        decls.push_back(fe.fi);
      }
    }
    // If a body-less anchor governs this identifier, checkAuthoritativeParameterNames
    // reports every divergence in its signature class against the canonical names, so
    // narrowing pairs in that class are handled there. Skip them here to avoid warning
    // twice. A strict narrowing has the same normalised signature as its target, so a
    // single anchor lookup per declaration classifies the whole pair.
    auto anchorIt = m->_fnAnchors.find(bucket.first);
    auto governedByAnchor = [&](FunctionI* d) {
      if (anchorIt == m->_fnAnchors.end()) {
        return false;
      }
      std::vector<Type> dsig = normalized_param_sig(env, d);
      for (const auto& a : anchorIt->second) {
        if (a.fi->paramCount() == d->paramCount() && normalized_param_sig(env, a.fi) == dsig) {
          return true;
        }
      }
      return false;
    };
    for (unsigned int xi = 0; xi < decls.size(); xi++) {
      FunctionI* x = decls[xi];
      const unsigned int arity = x->paramCount();
      if (governedByAnchor(x)) {
        continue;
      }
      // A strict narrowing `y` of `x` is more concrete than `x`, so it sorts
      // before `x`; so does any name-compatible rewrite target `z` (which
      // shares `y`'s types). Both therefore lie in the contiguous run of
      // equal-arity decls immediately preceding `x`. Find its start `lo`.
      unsigned int lo = xi;
      while (lo > 0 && decls[lo - 1]->paramCount() == arity) {
        lo--;
      }
      for (unsigned int yi = lo; yi < xi; yi++) {
        FunctionI* y = decls[yi];
        // Narrowing `x` to `y`'s types is only a hazard if their parameter
        // names disagree (otherwise the rewrite binds the same names).
        if (!isStrictNarrowing(x, y) || !namesDisagree(x, y)) {
          continue;
        }
        // Safe iff a name-compatible target exists: a decl with `y`'s exact
        // types but `x`'s parameter names, which the type-and-name-aware
        // matcher picks instead. It shares `y`'s types, so it is in this window
        // too. (For e.g. `cumulatives` that is the same-bound-name sibling.)
        bool hasNameCompatibleTarget = false;
        for (unsigned int zi = lo; zi < xi; zi++) {
          FunctionI* z = decls[zi];
          if (z != y && typesEqual(z, y) && !namesDisagree(z, x)) {
            hasNameCompatibleTarget = true;
            break;
          }
        }
        if (hasNameCompatibleTarget) {
          continue;
        }
        std::ostringstream mism;
        bool any = false;
        for (unsigned int j = 0; j < arity; j++) {
          const Id* a = x->param(j)->id();
          const Id* b = y->param(j)->id();
          if (!a->hasStr() || !b->hasStr() || a->v() == b->v()) {
            continue;
          }
          if (any) {
            mism << ", ";
          }
          mism << (j + 1) << " (`" << a->v() << "' vs `" << b->v() << "')";
          any = true;
        }
        std::ostringstream oss;
        oss << "overloads of `" << x->id()
            << "' that differ only in var/par or optionality disagree on parameter name(s): "
            << mism.str() << ". Defined at " << x->loc().toString() << " and "
            << y->loc().toString()
            << ". When an argument is narrowed (var to par, or opt to present) during flattening "
               "the call is re-resolved by type, and with no name-compatible sibling it cannot "
               "rewrite to the equivalent declaration.";
        env.addWarning(x->loc(), oss.str(), false);
      }
    }
  }
}

void Model::checkReifParameterNames(EnvI& env) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  // Deriving the base id (ASTString) and emitting warnings allocates GC objects.
  GCLock lock;

  // Exact per-parameter type equality over the leading `n` parameters (names ignored).
  auto leadingTypesEqual = [](const FunctionI* rf, const FunctionI* bf, unsigned int n) {
    for (unsigned int j = 0; j < n; j++) {
      if (rf->param(j)->type() != bf->param(j)->type()) {
        return false;
      }
    }
    return true;
  };
  // Do the leading `n` name-passable parameters agree? (Anonymous / synthetic ids cannot
  // be passed by name, so a disagreement on them does not matter.)
  auto leadingNamesMatch = [](const FunctionI* rf, const FunctionI* bf, unsigned int n) {
    for (unsigned int j = 0; j < n; j++) {
      const Id* a = rf->param(j)->id();
      const Id* b = bf->param(j)->id();
      if (a->hasStr() && b->hasStr() && a->v() != b->v()) {
        return false;
      }
    }
    return true;
  };

  for (const auto& bucket : m->_fnmap) {
    const ASTString& rid = bucket.first;
    size_t suffixLen = 0;
    if (rid.endsWith("_reif")) {
      suffixLen = 5;
    } else if (rid.endsWith("_imp")) {
      suffixLen = 4;
    } else {
      continue;
    }
    auto bit = m->_fnmap.find(ASTString(rid.substr(0, rid.size() - suffixLen)));
    if (bit == m->_fnmap.end()) {
      continue;
    }
    const ASTString& baseId = bit->first;
    for (const auto& rfe : bucket.second) {
      // Skip generated instances, which would duplicate warnings and point at synthetic decls.
      if (rfe.isPolymorphicVariant || rfe.fi->isMonomorphised()) {
        continue;
      }
      FunctionI* rf = rfe.fi;
      if (rf->paramCount() == 0) {
        continue;
      }
      const unsigned int lead = rf->paramCount() - 1;  // trailing parameter is the reif bool
      // A call is re-resolved to its reified/half-reified version by matching the base
      // declaration's leading parameter names (see matchReifByNames). The base a reif/imp
      // corresponds to is the one with exactly the same leading parameter types. If such a
      // base exists but none of them shares this reif/imp's leading names, the reification
      // can never be found by name. Subtyping is allowed at match time, but the name check
      // is only meaningful against an exact-type base, so we require exact type equality here.
      FunctionI* exactBase = nullptr;
      bool nameCompatibleBaseExists = false;
      for (const auto& bfe : bit->second) {
        if (bfe.isPolymorphicVariant || bfe.fi->isMonomorphised()) {
          continue;
        }
        FunctionI* bf = bfe.fi;
        if (bf->paramCount() != lead || !leadingTypesEqual(rf, bf, lead)) {
          continue;
        }
        exactBase = bf;
        if (leadingNamesMatch(rf, bf, lead)) {
          nameCompatibleBaseExists = true;
          break;
        }
      }
      if (exactBase == nullptr || nameCompatibleBaseExists) {
        continue;
      }
      // If the base's family is anchored (a body-less builtin), the reif/imp re-match falls
      // back to a type-only lookup (see matchReifByNames / isFnAnchored), so a name
      // disagreement is harmless: the reification is still found by type. Such families are
      // positional-only this release, so by default we stay silent about them, matching the
      // opt-in treatment of the authoritative-name check. Only when the implementer opts in
      // via --warn-non-authoritative-names do we also report these. Non-anchored families are
      // always reported, because there a name disagreement can genuinely hide the reification.
      if (!env.warnNonAuthoritativeNames && isFnAnchored(env, exactBase)) {
        continue;
      }
      std::ostringstream mism;
      bool any = false;
      for (unsigned int j = 0; j < lead; j++) {
        const Id* a = rf->param(j)->id();
        const Id* b = exactBase->param(j)->id();
        if (!a->hasStr() || !b->hasStr() || a->v() == b->v()) {
          continue;
        }
        if (any) {
          mism << ", ";
        }
        mism << (j + 1) << " (`" << a->v() << "' vs `" << b->v() << "')";
        any = true;
      }
      std::ostringstream oss;
      oss << "reified/half-reified predicate `" << rid << "' disagrees with its base predicate `"
          << baseId << "' on parameter name(s): " << mism.str() << ". Defined at "
          << rf->loc().toString() << " and " << exactBase->loc().toString()
          << ". A call is re-resolved to its reified or half-reified version by matching the base "
             "predicate's parameter names, so the names must agree or the reification cannot be "
             "found.";
      env.addWarning(rf->loc(), oss.str(), false);
    }
  }
}

void Model::checkAuthoritativeParameterNames(EnvI& env) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  // Deriving normalised signatures and emitting warnings allocates GC objects.
  GCLock lock;

  for (const auto& bucket : m->_fnmap) {
    auto ait = m->_fnAnchors.find(bucket.first);
    if (ait == m->_fnAnchors.end()) {
      continue;
    }
    const std::vector<FnAnchor>& anchors = ait->second;
    std::vector<std::vector<Type>> anchorSig(anchors.size());
    for (size_t k = 0; k < anchors.size(); k++) {
      anchorSig[k] = normalized_param_sig(env, anchors[k].fi);
    }

    // A par version generated by create_par_versions is a copy of its source, sharing its
    // location and parameter names, and coexists with the source in the bucket. Report each
    // source location only once so such a pair does not warn twice.
    std::unordered_set<std::string> warnedLocs;
    for (const auto& fe : bucket.second) {
      // Generated instances duplicate their source's names; the source is checked directly.
      if (fe.isPolymorphicVariant || fe.fi->isMonomorphised()) {
        continue;
      }
      FunctionI* d = fe.fi;
      std::vector<Type> dsig = normalized_param_sig(env, d);

      // The authoritative anchor for `d` is the body-less declaration in `d`'s signature
      // class. The override contract makes source rank irrelevant: a solver may declare
      // body-less only what the standard library already declared body-less, so every
      // anchor of a signature class carries the same canonical names. Ties are broken by
      // source location so the choice - and thus the warning - is deterministic regardless
      // of registration order.
      const FnAnchor* best = nullptr;
      for (size_t k = 0; k < anchors.size(); k++) {
        if (anchors[k].fi->paramCount() != d->paramCount() || anchorSig[k] != dsig) {
          continue;
        }
        if (best == nullptr || anchors[k].fi->loc().toString() < best->fi->loc().toString()) {
          best = &anchors[k];
        }
      }
      if (best == nullptr || best->fi == d) {
        continue;
      }

      std::ostringstream mism;
      bool any = false;
      for (unsigned int j = 0; j < d->paramCount(); j++) {
        VarDecl* pd = d->param(j);
        VarDecl* pa = best->fi->param(j);
        // Only positions callable by name in both declarations matter.
        if (!is_nameable(pd) || !is_nameable(pa) || pd->id()->v() == pa->id()->v()) {
          continue;
        }
        if (any) {
          mism << ", ";
        }
        mism << (j + 1) << " (`" << pd->id()->v() << "' vs `" << pa->id()->v() << "')";
        any = true;
      }
      if (!any) {
        continue;
      }
      if (!warnedLocs.insert(d->loc().toString()).second) {
        continue;
      }
      std::ostringstream oss;
      oss << "declaration of `" << d->id()
          << "' disagrees on parameter name(s) with the body-less declaration that defines the "
             "canonical names for this overload family: "
          << mism.str() << ". Defined at " << d->loc().toString() << "; canonical names from "
          << best->fi->loc().toString()
          << ". A body-less declaration is a callable builtin, so it fixes the names by which its "
             "overload family can be called; a call using those names cannot resolve to this "
             "declaration.";
      env.addWarning(d->loc(), oss.str(), false);
    }
  }
}

std::vector<FunctionI*> Model::potentialOverloads(EnvI& env, Call* c) const {
  if (c->id() == env.constants.varRedef->id()) {
    return {env.constants.varRedef};
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it = m->_fnmap.find(c->id());
  if (it == m->_fnmap.end()) {
    std::ostringstream oss;
    oss << "no function or predicate with name `";
    oss << c->id() << "' found";

    ASTString mostSimilar;
    int minEdits = 3;
    for (const auto& decls : m->_fnmap) {
      if (std::abs(static_cast<int>(c->id().size()) - static_cast<int>(decls.first.size())) <= 3) {
        int edits = c->id().levenshteinDistance(decls.first);
        if (edits < minEdits && edits < std::min(c->id().size(), decls.first.size())) {
          minEdits = edits;
          mostSimilar = decls.first;
        }
      }
    }
    if (!mostSimilar.empty()) {
      oss << ", did you mean `" << mostSimilar << "'?";
    }
    throw TypeError(env, Expression::loc(c), oss.str());
  }

  const std::vector<FnEntry>& v = it->second;
  std::vector<FunctionI*> matched;
  for (const auto& i : v) {
    if (i.t.size() == c->argCount()) {
      matched.push_back(i.fi);
    }
  }
  if (matched.empty()) {
    std::ostringstream oss;
    oss << "no function or predicate with this signature found: `";
    oss << c->id() << "(";
    for (unsigned int i = 0; i < c->argCount(); i++) {
      oss << Expression::type(c->arg(i)).toString(env);
      if (i < c->argCount() - 1) {
        oss << ",";
      }
    }
    oss << ")'\n";
    throw TypeError(env, Expression::loc(c), oss.str());
  }

  return matched;
}

namespace {
int first_overloaded(EnvI& env, const std::vector<Model::FnEntry>& v_f, int i_f) {
  int first_i_f = i_f;
  for (; (first_i_f--) != 0;) {
    // find first instance overloaded on subtypes
    if (v_f[first_i_f].t.size() != v_f[i_f].t.size()) {
      break;
    }
    bool allSubtypes = true;
    for (unsigned int i = 0; i < v_f[first_i_f].t.size(); i++) {
      if (!env.isSubtype(v_f[first_i_f].t[i], v_f[i_f].t[i], false)) {
        allSubtypes = false;
        break;
      }
    }
    if (!allSubtypes) {
      break;
    }
  }
  return first_i_f + 1;
}
}  // namespace

bool Model::sameOverloading(EnvI& env, const std::vector<Expression*>& args, FunctionI* f,
                            FunctionI* g) const {
  if (f->isMonomorphised() || g->isMonomorphised() || f->id() == env.constants.varRedef->id() ||
      g->id() == env.constants.varRedef->id()) {
    return false;
  }
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto it_f = m->_fnmap.find(f->id());
  auto it_g = m->_fnmap.find(g->id());
  assert(it_f != m->_fnmap.end());
  assert(it_g != m->_fnmap.end());
  const std::vector<FnEntry>& v_f = it_f->second;
  const std::vector<FnEntry>& v_g = it_g->second;

  std::vector<FunctionI*> dummyMatched;
  Expression* dummyBotarg;
  int i_f = match_idx(dummyMatched, dummyBotarg, env, v_f, args, true);
  if (i_f == -1) {
    return false;
  }
  int i_g = match_idx(dummyMatched, dummyBotarg, env, v_g, args, true);
  if (i_g == -1) {
    return false;
  }
  assert(i_f < v_f.size());
  assert(i_g < v_g.size());
  unsigned int first_i_f = first_overloaded(env, v_f, i_f);
  unsigned int first_i_g = first_overloaded(env, v_g, i_g);
  if (i_f - first_i_f != i_g - first_i_g) {
    // not the same number of overloaded versions
    return false;
  }
  for (; first_i_f <= static_cast<unsigned int>(i_f); first_i_f++, first_i_g++) {
    if (!(v_f[first_i_f].t == v_g[first_i_g].t)) {
      // one of the overloaded versions does not agree in the types
      return false;
    }
  }
  return true;
}

FunctionI* Model::matchRevMap(EnvI& env, const Type& t0) const {
  const Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  Type t = t0;
  if (t.bt() == Type::BT_INT) {
    t.typeId(0);
  }
  auto it = _revmapmap.find(t.toInt());
  if (it != _revmapmap.end()) {
    return it->second;
  }
  return nullptr;
}

Item*& Model::operator[](unsigned int i) {
  assert(i < _items.size());
  return _items[i];
}
const Item* Model::operator[](unsigned int i) const {
  assert(i < _items.size());
  return _items[i];
}
unsigned int Model::size() const { return static_cast<unsigned int>(_items.size()); }
bool Model::empty() const { return _items.empty(); }

std::vector<Item*>::iterator Model::begin() { return _items.begin(); }

std::vector<Item*>::const_iterator Model::begin() const { return _items.begin(); }

std::vector<Item*>::iterator Model::end() { return _items.end(); }

std::vector<Item*>::const_iterator Model::end() const { return _items.end(); }

void Model::compact() {
  struct {
    bool operator()(const Item* i) { return i->removed(); }
  } isremoved;
  _items.erase(remove_if(_items.begin(), _items.end(), isremoved), _items.end());
}

}  // namespace MiniZinc
