/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astexception.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/model.hh>
#include <minizinc/prettyprinter.hh>

#undef MZN_DEBUG_FUNCTION_REGISTRY

namespace MiniZinc {

Model::FnEntry::FnEntry(FunctionI* fi0) : t(fi0->params().size()), fi(fi0), isPolymorphic(false) {
  for (unsigned int i = 0; i < fi->params().size(); i++) {
    t[i] = fi->params()[i]->type();
    isPolymorphic |= (t[i].bt() == Type::BT_TOP);
  }
}

bool Model::FnEntry::operator<(const Model::FnEntry& f) const {
  assert(!compare(*this, f) || !compare(f, *this));
  return compare(*this, f);
}

bool Model::FnEntry::compare(const Model::FnEntry& e1, const Model::FnEntry& e2) {
  if (e1.t.size() < e2.t.size()) {
    return true;
  }
  if (e1.t.size() == e2.t.size()) {
    for (unsigned int i = 0; i < e1.t.size(); i++) {
      if (e1.t[i] != e2.t[i]) {
        if (e1.t[i].isSubtypeOf(e2.t[i], true)) {
          return true;
        }
        if (e2.t[i].isSubtypeOf(e1.t[i], true)) {
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
/// Return lowest possible base type given other type-inst restrictions
Type::BaseType lowest_bt(const Type& t) {
  if (t.st() == Type::ST_SET && t.ti() == Type::TI_VAR) {
    return Type::BT_INT;
  }
  return Type::BT_BOOL;
}
/// Return highest possible base type given other type-inst restrictions
Type::BaseType highest_bt(const Type& t) {
  if (t.st() == Type::ST_SET && t.ti() == Type::TI_VAR) {
    return Type::BT_INT;
  }
  if (t.ti() == Type::TI_VAR || t.st() == Type::ST_SET) {
    return Type::BT_FLOAT;
  }
  return Type::BT_ANN;
}
}  // namespace

void Model::addPolymorphicInstances(Model::FnEntry& fe, std::vector<FnEntry>& entries) {
  entries.push_back(fe);
  if (fe.isPolymorphic) {
    FnEntry cur = fe;
    std::vector<std::vector<Type*> > type_ids;

    // First step: initialise all type variables to bool
    // and collect them in the stack vector
    for (unsigned int i = 0; i < cur.t.size(); i++) {
      if (cur.t[i].bt() == Type::BT_TOP) {
        std::vector<Type*> t;
        for (unsigned int j = i; j < cur.t.size(); j++) {
          assert(cur.fi->params()[i]->ti()->domain() &&
                 cur.fi->params()[i]->ti()->domain()->isa<TIId>());
          if ((cur.fi->params()[j]->ti()->domain() != nullptr) &&
              cur.fi->params()[j]->ti()->domain()->isa<TIId>()) {
            TIId* id0 = cur.fi->params()[i]->ti()->domain()->cast<TIId>();
            TIId* id1 = cur.fi->params()[j]->ti()->domain()->cast<TIId>();
            if (id0->v() == id1->v()) {
              // Found parameter with same type variable
              // Initialise to lowest concrete base type (bool)
              cur.t[j].bt(lowest_bt(cur.t[j]));
              t.push_back(&cur.t[j]);
            }
          }
        }
        type_ids.push_back(t);
      }
    }

    std::vector<int> stack;
    for (unsigned int i = 0; i < type_ids.size(); i++) {
      stack.push_back(i);
    }
    int final_id = static_cast<int>(type_ids.size()) - 1;

    while (!stack.empty()) {
      if (stack.back() == final_id) {
        // If this instance isn't in entries yet, add it
        bool alreadyDefined = false;
        for (auto& entry : entries) {
          if (entry.t == cur.t) {
            alreadyDefined = true;
            break;
          }
        }
        if (!alreadyDefined) {
          entries.push_back(cur);
        }
      }

      Type& back_t = *type_ids[stack.back()][0];
      if (back_t.bt() == highest_bt(back_t) && back_t.st() == Type::ST_SET) {
        // last type, remove this item
        stack.pop_back();
      } else {
        if (back_t.bt() == highest_bt(back_t)) {
          // Create set type for current item
          for (auto& i : type_ids[stack.back()]) {
            i->st(Type::ST_SET);
            i->bt(lowest_bt(*i));
          }
        } else {
          // Increment type of current item
          auto nextType = static_cast<Type::BaseType>(back_t.bt() + 1);
          for (auto& i : type_ids[stack.back()]) {
            i->bt(nextType);
          }
        }
        // Reset types of all further items and push them
        for (unsigned int i = stack.back() + 1; i < type_ids.size(); i++) {
          for (auto& j : type_ids[i]) {
            j->bt(lowest_bt(*j));
          }
          stack.push_back(i);
        }
      }
    }
  }
}

bool Model::registerFn(EnvI& env, FunctionI* fi, bool keepSorted, bool throwIfDuplicate) {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto i_id = m->_fnmap.find(fi->id());
  if (i_id == m->_fnmap.end()) {
    // new element
    std::vector<FnEntry> v;
    FnEntry fe(fi);
    addPolymorphicInstances(fe, v);
    m->_fnmap.insert(std::pair<ASTString, std::vector<FnEntry> >(fi->id(), v));
  } else {
    // add to list of existing elements
    std::vector<FnEntry>& v = i_id->second;
    for (auto& i : v) {
      if (i.fi == fi) {
        return true;
      }
      if (i.fi->params().size() == fi->params().size()) {
        bool alleq = true;
        for (unsigned int j = 0; j < fi->params().size(); j++) {
          Type t1 = i.fi->params()[j]->type();
          Type t2 = fi->params()[j]->type();
          t1.enumId(0);
          t2.enumId(0);
          if (t1 != t2) {
            alleq = false;
            break;
          }
        }
        if (alleq) {
          if ((i.fi->e() != nullptr) && (fi->e() != nullptr) && !i.isPolymorphic) {
            if (throwIfDuplicate) {
              throw TypeError(
                  env, fi->loc(),
                  "function with the same type already defined in " + i.fi->loc().toString());
            }
            return false;
          }
          if ((fi->e() != nullptr) || i.isPolymorphic) {
            if (Call* deprecated = i.fi->ann().getCall(constants().ann.mzn_deprecated)) {
              fi->ann().add(deprecated);
            }
            i = fi;
          } else if (Call* deprecated = fi->ann().getCall(constants().ann.mzn_deprecated)) {
            i.fi->ann().add(deprecated);
          }
          return true;
        }
      }
    }
    FnEntry fe(fi);
    addPolymorphicInstances(fe, v);
    if (keepSorted) {
      std::sort(i_id->second.begin(), i_id->second.end());
    }
  }
  if (fi->id() == "mzn_reverse_map_var") {
    if (fi->params().size() != 1 || fi->ti()->type() != Type::varbool()) {
      throw TypeError(env, fi->loc(),
                      "functions called `mzn_reverse_map_var` must have a single argument and "
                      "return type var bool");
    }
    Type t = fi->params()[0]->type();
    _revmapmap.insert(std::pair<int, FunctionI*>(t.toInt(), fi));
  }
  return true;
}

FunctionI* Model::matchFn(EnvI& env, const ASTString& id, const std::vector<Type>& t,
                          bool strictEnums) {
  if (id == constants().varRedef->id()) {
    return constants().varRedef;
  }
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  auto i_id = m->_fnmap.find(id);
  if (i_id == m->_fnmap.end()) {
    return nullptr;
  }
  std::vector<FnEntry>& v = i_id->second;
  for (auto& i : v) {
    std::vector<Type>& fi_t = i.t;
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
        return i.fi;
      }
    }
  }
  return nullptr;
}

void Model::mergeStdLib(EnvI& env, Model* m) const {
  for (const auto& it : _fnmap) {
    for (const auto& cit : it.second) {
      if (cit.fi->fromStdLib()) {
        (void)m->registerFn(env, cit.fi);
      }
    }
  }
  m->sortFn();
}

void Model::sortFn() {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  for (auto& it : m->_fnmap) {
    // Sort all functions by type
    std::sort(it.second.begin(), it.second.end());
  }
}

void Model::fixFnMap() {
  Model* m = this;
  while (m->_parent != nullptr) {
    m = m->_parent;
  }
  for (auto& it : m->_fnmap) {
    for (auto& i : it.second) {
      for (unsigned int j = 0; j < i.t.size(); j++) {
        if (i.t[j].isunknown()) {
          i.t[j] = i.fi->params()[j]->type();
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
        if (cur == cmp || cur->params().size() != cmp->params().size()) {
          break;
        }
        bool allEqual = true;
        for (unsigned int i = 0; i < cur->params().size(); i++) {
          Type t1 = cur->params()[i]->type();
          Type t2 = cmp->params()[i]->type();
          t1.enumId(0);
          t2.enumId(0);
          if (t1 != t2) {
            allEqual = false;
            break;
          }
        }
        if (allEqual) {
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
        if (!env.isSubtype(args[j]->type(), fi_t[j], strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
          std::cerr << args[j]->type().toString(env) << " does not match " << fi_t[j].toString(env)
                    << "\n";
#endif
          match = false;
          break;
        }
        if (args[j]->type().isbot() && fi_t[j].bt() != Type::BT_TOP) {
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

FunctionI* Model::matchFn(EnvI& env, const ASTString& id, const std::vector<Expression*>& args,
                          bool strictEnums) const {
  if (id == constants().varRedef->id()) {
    return constants().varRedef;
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
  if (matched.size() == 1) {
    return matched[0];
  }
  Type t = matched[0]->ti()->type();
  t.ti(Type::TI_PAR);
  for (unsigned int i = 1; i < matched.size(); i++) {
    if (!env.isSubtype(t, matched[i]->ti()->type(), strictEnums)) {
      throw TypeError(env, botarg->loc(), "ambiguous overloading on return type of function");
    }
  }
  return matched[0];
}

FunctionI* Model::matchFn(EnvI& env, Call* c, bool strictEnums, bool throwIfNotFound) const {
  if (c->id() == constants().varRedef->id()) {
    return constants().varRedef;
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
      if (mostSimilar.size() > 0) {
        oss << ", did you mean `" << mostSimilar << "'?";
      }
      throw TypeError(env, c->loc(), oss.str());
    }
    return nullptr;
  }
  const std::vector<FnEntry>& v = it->second;
  std::vector<FunctionI*> matched;
  Expression* botarg = nullptr;
  for (const auto& i : v) {
    const std::vector<Type>& fi_t = i.t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
    std::cerr << "try " << *i.fi;
#endif
    if (fi_t.size() == c->argCount()) {
      bool match = true;
      for (unsigned int j = 0; j < c->argCount(); j++) {
        if (!env.isSubtype(c->arg(j)->type(), fi_t[j], strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
          std::cerr << c->arg(j)->type().toString(env) << " does not match "
                    << fi_t[j].toString(env) << "\n";
          std::cerr << "Wrong argument is " << *c->arg(j);
#endif
          match = false;
          break;
        }
        if (c->arg(j)->type().isbot() && fi_t[j].bt() != Type::BT_TOP) {
          botarg = c->arg(j);
        }
      }
      if (match) {
        if (botarg != nullptr) {
          matched.push_back(i.fi);
        } else {
          return i.fi;
        }
      }
    }
  }
  if (matched.empty()) {
    if (throwIfNotFound) {
      std::ostringstream oss;
      oss << "no function or predicate with this signature found: `";
      oss << c->id() << "(";
      for (unsigned int i = 0; i < c->argCount(); i++) {
        oss << c->arg(i)->type().toString(env);
        if (i < c->argCount() - 1) {
          oss << ",";
        }
      }
      oss << ")'\n";
      oss << "Cannot use the following functions or predicates with the same identifier:\n";
      Printer pp(oss, 0, false, &env);
      for (const auto& i : v) {
        const std::vector<Type>& fi_t = i.t;
        Expression* body = i.fi->e();
        i.fi->e(nullptr);
        pp.print(i.fi);
        i.fi->e(body);
        if (fi_t.size() == c->argCount()) {
          for (unsigned int j = 0; j < c->argCount(); j++) {
            if (!env.isSubtype(c->arg(j)->type(), fi_t[j], strictEnums)) {
              oss << "    (argument " << (j + 1) << " expects type " << fi_t[j].toString(env);
              oss << ", but type " << c->arg(j)->type().toString(env) << " given)\n";
            }
            if (c->arg(j)->type().isbot() && fi_t[j].bt() != Type::BT_TOP) {
              botarg = c->arg(j);
            }
          }
        } else {
          oss << "    (requires " << i.fi->params().size() << " argument"
              << (i.fi->params().size() == 1 ? "" : "s") << ", but " << c->argCount()
              << " given)\n";
        }
      }
      throw TypeError(env, c->loc(), oss.str());
    }
    return nullptr;
  }
  if (matched.size() == 1) {
    return matched[0];
  }
  Type t = matched[0]->ti()->type();
  t.ti(Type::TI_PAR);
  for (unsigned int i = 1; i < matched.size(); i++) {
    if (!env.isSubtype(t, matched[i]->ti()->type(), strictEnums)) {
      throw TypeError(env, botarg->loc(), "ambiguous overloading on return type of function");
    }
  }
  return matched[0];
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
  for (; first_i_f <= i_f; first_i_f++, first_i_g++) {
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
  t.enumId(0);
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
