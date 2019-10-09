/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/model.hh>
#include <minizinc/flatten_internal.hh>
#include <minizinc/astexception.hh>
#include <minizinc/prettyprinter.hh>

#undef MZN_DEBUG_FUNCTION_REGISTRY

namespace MiniZinc {
  
  Model::FnEntry::FnEntry(FunctionI* fi0) : t(fi0->params().size()), fi(fi0), isPolymorphic(false) {
    for (unsigned int i=0; i<fi->params().size(); i++) {
      t[i] = fi->params()[i]->type();
      isPolymorphic |= (t[i].bt()==Type::BT_TOP);
    }
  }

  bool
  Model::FnEntry::operator<(const Model::FnEntry& f) const {
    assert(!compare(*this,f) || !compare(f,*this));
    return compare(*this, f);
  }

  bool
  Model::FnEntry::compare(const Model::FnEntry& e1, const Model::FnEntry& e2) {
    if (e1.t.size() < e2.t.size()) {
      return true;
    }
    if (e1.t.size() == e2.t.size()) {
      for (unsigned int i=0; i<e1.t.size(); i++) {
        if (e1.t[i] != e2.t[i]) {
          if (e1.t[i].isSubtypeOf(e2.t[i], true)) {
            return true;
          } else {
            if (e2.t[i].isSubtypeOf(e1.t[i], true))
              return false;
            switch (e1.t[i].cmp(e2.t[i])) {
              case -1: return true;
              case 1: return false;
              default: assert(false);
            }
          }
        }
      }
    }
    return false;
  }

  Model::Model(void) : _parent(NULL), _solveItem(NULL), _outputItem(NULL) {
    GC::add(this);
  }

  Model::~Model(void) {
    for (unsigned int j=0; j<_items.size(); j++) {
      Item* i = _items[j];
      if (IncludeI* ii = i->dyn_cast<IncludeI>()) {
        if (ii->own() && ii->m()) {
          delete ii->m();
          ii->m(NULL);
        }
      }
    }
    GC::remove(this);
  }

  VarDeclIterator
  Model::begin_vardecls(void) {
    return VarDeclIterator(this, begin());
  }
  VarDeclIterator
  Model::end_vardecls(void) {
    return VarDeclIterator(this, end());
  }
  ConstraintIterator
  Model::begin_constraints(void) {
    return ConstraintIterator(this, begin());
  }
  ConstraintIterator
  Model::end_constraints(void) {
    return ConstraintIterator(this, end());
  }
  FunctionIterator
  Model::begin_functions(void) {
    return FunctionIterator(this, begin());
  }
  FunctionIterator
  Model::end_functions(void) {
    return FunctionIterator(this, end());
  }

  SolveI*
  Model::solveItem() {
    return _solveItem;
  }
  
  OutputI*
  Model::outputItem() {
    return _outputItem;
  }

  void
  Model::addItem(Item* i) {
    _items.push_back(i);
    if (i->isa<SolveI>()) {
      Model* m = this;
      while (m->_parent)
        m = m->_parent;
      m->_solveItem = i->cast<SolveI>();
    } else if (i->isa<OutputI>()) {
      Model* m = this;
      while (m->_parent)
        m = m->_parent;
      m->_outputItem = i->cast<OutputI>();
    }
  }

  void
  Model::setOutputItem(OutputI* oi) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    m->_outputItem = oi;
  }
  
  namespace {
    /// Return lowest possible base type given other type-inst restrictions
    Type::BaseType lowestBt(const Type& t) {
      if (t.st()==Type::ST_SET && t.ti()==Type::TI_VAR)
        return Type::BT_INT;
      return Type::BT_BOOL;
    }
    /// Return highest possible base type given other type-inst restrictions
    Type::BaseType highestBt(const Type& t) {
      if (t.st()==Type::ST_SET && t.ti()==Type::TI_VAR)
        return Type::BT_INT;
      if (t.ti()==Type::TI_VAR || t.st()==Type::ST_SET)
        return Type::BT_FLOAT;
      return Type::BT_ANN;
    }
  }
  
  void
  Model::addPolymorphicInstances(Model::FnEntry& fe, std::vector<FnEntry>& entries) {
    entries.push_back(fe);
    if (fe.isPolymorphic) {
      FnEntry cur = fe;
      std::vector<std::vector<Type*> > type_ids;
      
      // First step: initialise all type variables to bool
      // and collect them in the stack vector
      for (unsigned int i=0; i<cur.t.size(); i++) {
        if (cur.t[i].bt()==Type::BT_TOP) {
          std::vector<Type*> t;
          for (unsigned int j=i; j<cur.t.size(); j++) {
            assert(cur.fi->params()[i]->ti()->domain() && cur.fi->params()[i]->ti()->domain()->isa<TIId>());
            if (cur.fi->params()[j]->ti()->domain() && cur.fi->params()[j]->ti()->domain()->isa<TIId>()) {
              TIId* id0 = cur.fi->params()[i]->ti()->domain()->cast<TIId>();
              TIId* id1 = cur.fi->params()[j]->ti()->domain()->cast<TIId>();
              if (id0->v()==id1->v()) {
                // Found parameter with same type variable
                // Initialise to lowest concrete base type (bool)
                cur.t[j].bt(lowestBt(cur.t[j]));
                t.push_back(&cur.t[j]);
              }
            }
          }
          type_ids.push_back(t);
        }
      }
      
      std::vector<int> stack;
      for (unsigned int i=0; i<type_ids.size(); i++)
        stack.push_back(i);
      int final_id = static_cast<int>(type_ids.size())-1;
      
      while (!stack.empty()) {
        
        if (stack.back()==final_id) {
          // If this instance isn't in entries yet, add it
          bool alreadyDefined = false;
          for (unsigned int i=0; i<entries.size(); i++) {
            if (entries[i].t == cur.t) {
              alreadyDefined = true;
              break;
            }
          }
          if (!alreadyDefined) {
            entries.push_back(cur);
          }
        }
        
        Type& back_t = *type_ids[stack.back()][0];
        if (back_t.bt() == highestBt(back_t) && back_t.st() == Type::ST_SET) {
          // last type, remove this item
          stack.pop_back();
        } else {
          if (back_t.bt() == highestBt(back_t)) {
            // Create set type for current item
            for (unsigned int i=0; i<type_ids[stack.back()].size(); i++) {
              type_ids[stack.back()][i]->st(Type::ST_SET);
              type_ids[stack.back()][i]->bt(lowestBt(*type_ids[stack.back()][i]));
            }
          } else {
            // Increment type of current item
            Type::BaseType nextType = static_cast<Type::BaseType>(back_t.bt()+1);
            for (unsigned int i=0; i<type_ids[stack.back()].size(); i++) {
              type_ids[stack.back()][i]->bt(nextType);
            }
          }
          // Reset types of all further items and push them
          for (unsigned int i=stack.back()+1; i<type_ids.size(); i++) {
            for (unsigned int j=0; j<type_ids[i].size(); j++) {
              type_ids[i][j]->bt(lowestBt(*type_ids[i][j]));
            }
            stack.push_back(i);
          }
        }
        
      }
      
      
      
    }
  }
  
  void
  Model::registerFn(EnvI& env, FunctionI* fi) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::iterator i_id = m->fnmap.find(fi->id());
    if (i_id == m->fnmap.end()) {
      // new element
      std::vector<FnEntry> v;
      FnEntry fe(fi);
      addPolymorphicInstances(fe, v);
      m->fnmap.insert(std::pair<ASTString,std::vector<FnEntry> >(fi->id(),v));
    } else {
      // add to list of existing elements
      std::vector<FnEntry>& v = i_id->second;
      for (unsigned int i=0; i<v.size(); i++) {
        if (v[i].fi->params().size() == fi->params().size()) {
          bool alleq=true;
          for (unsigned int j=0; j<fi->params().size(); j++) {
            Type t1 = v[i].fi->params()[j]->type();
            Type t2 = fi->params()[j]->type();
            t1.enumId(0);
            t2.enumId(0);
            if (t1 != t2) {
              alleq=false; break;
            }
          }
          if (alleq) {
            if (v[i].fi==fi) {
              return;
            } else if (v[i].fi->e() && fi->e() && !v[i].isPolymorphic) {
              throw TypeError(env, fi->loc(),
                              "function with the same type already defined in "
                              +v[i].fi->loc().toString());
            } else {
              if (fi->e() || v[i].isPolymorphic)
                v[i] = fi;
              return;
            }
          }
        }
      }
      FnEntry fe(fi);
      addPolymorphicInstances(fe, v);
    }
    if (fi->id()=="mzn_reverse_map_var") {
      if (fi->params().size() != 1 || fi->ti()->type() != Type::varbool()) {
        throw TypeError(env, fi->loc(), "functions called `mzn_reverse_map_var` must have a single argument and return type var bool");
      }
      Type t = fi->params()[0]->type();
      revmapmap.insert(std::pair<int,FunctionI*>(t.toInt(),fi));
    }
  }

  FunctionI*
  Model::matchFn(EnvI& env, const ASTString& id,
                 const std::vector<Type>& t,
                 bool strictEnums) {
    if (id==constants().var_redef->id())
      return constants().var_redef;
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::iterator i_id = m->fnmap.find(id);
    if (i_id == m->fnmap.end()) {
      return NULL;
    }
    std::vector<FnEntry>& v = i_id->second;
    for (unsigned int i=0; i<v.size(); i++) {
      std::vector<Type>& fi_t = v[i].t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
      std::cerr << "try " << *v[i].fi;
#endif
      if (fi_t.size() == t.size()) {
        bool match=true;
        for (unsigned int j=0; j<t.size(); j++) {
          if (!env.isSubtype(t[j],fi_t[j],strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
            std::cerr << t[j].toString(env) << " does not match "
            << fi_t[j].toString(env) << "\n";
#endif
            match=false;
            break;
          }
        }
        if (match) {
          return v[i].fi;
        }
      }
    }
    return NULL;
  }

  void
  Model::mergeStdLib(EnvI &env, Model *m) const {
    for (FnMap::const_iterator it=fnmap.begin(); it != fnmap.end(); ++it) {
      for (std::vector<FnEntry>::const_iterator cit = it->second.begin(); cit != it->second.end(); ++cit) {
        if ((*cit).fi->from_stdlib()) {
          m->registerFn(env, (*cit).fi);
        }
      }
    }
  }
  
  void
  Model::sortFn(void) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    for (FnMap::iterator it=m->fnmap.begin(); it!=m->fnmap.end(); ++it) {
      // Sort all functions by type
      std::sort(it->second.begin(),it->second.end());
    }
  }

  void
  Model::fixFnMap(void) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    for (FnMap::iterator it=m->fnmap.begin(); it!=m->fnmap.end(); ++it) {
      for (unsigned int i=0; i<it->second.size(); i++) {
        for (unsigned int j=0; j<it->second[i].t.size(); j++) {
          if (it->second[i].t[j].isunknown()) {
            it->second[i].t[j] = it->second[i].fi->params()[j]->type();
          }
        }
      }
    }
  }
  
  void
  Model::checkFnOverloading(EnvI& env) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    for (FnMap::iterator it=m->fnmap.begin(); it!=m->fnmap.end(); ++it) {
      std::vector<FnEntry>& fs = it->second;
      for (unsigned int i=0; i<fs.size()-1; i++) {
        FunctionI* cur = fs[i].fi;
        for (unsigned int j=i+1; j<fs.size(); j++) {
          FunctionI* cmp = fs[j].fi;
          if (cur==cmp || cur->params().size() != cmp->params().size())
            break;
          bool allEqual = true;
          for (unsigned int i=0; i<cur->params().size(); i++) {
            Type t1 = cur->params()[i]->type();
            Type t2 = cmp->params()[i]->type();
            t1.enumId(0);
            t2.enumId(0);
            if (t1!=t2) {
              allEqual = false;
              break;
            }
          }
          if (allEqual)
            throw TypeError(env,cur->loc(),
                            "unsupported type of overloading. \nFunction/predicate with equivalent signature defined in "+cmp->loc().toString());
        }
      }
    }
  }
  
  FunctionI*
  Model::matchFn(EnvI& env, const ASTString& id,
                 const std::vector<Expression*>& args,
                 bool strictEnums) const {
    if (id==constants().var_redef->id())
      return constants().var_redef;
    const Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::const_iterator it = m->fnmap.find(id);
    if (it == m->fnmap.end()) {
      return NULL;
    }
    const std::vector<FnEntry>& v = it->second;
    std::vector<FunctionI*> matched;
    Expression* botarg = NULL;
    for (unsigned int i=0; i<v.size(); i++) {
      const std::vector<Type>& fi_t = v[i].t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
      std::cerr << "try " << *v[i].fi;
#endif
      if (fi_t.size() == args.size()) {
        bool match=true;
        for (unsigned int j=0; j<args.size(); j++) {
          if (!env.isSubtype(args[j]->type(),fi_t[j],strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
            std::cerr << args[j]->type().toString(env) << " does not match "
            << fi_t[j].toString(env) << "\n";
#endif
            match=false;
            break;
          }
          if (args[j]->type().isbot() && fi_t[j].bt()!=Type::BT_TOP) {
            botarg = args[j];
          }
        }
        if (match) {
          if (botarg)
            matched.push_back(v[i].fi);
          else
            return v[i].fi;
        }
      }
    }
    if (matched.empty())
      return NULL;
    if (matched.size()==1)
      return matched[0];
    Type t = matched[0]->ti()->type();
    t.ti(Type::TI_PAR);
    for (unsigned int i=1; i<matched.size(); i++) {
      if (!env.isSubtype(t,matched[i]->ti()->type(),strictEnums))
        throw TypeError(env, botarg->loc(), "ambiguous overloading on return type of function");
    }
    return matched[0];
  }
  
  FunctionI*
  Model::matchFn(EnvI& env, Call* c, bool strictEnums) const {
    if (c->id()==constants().var_redef->id())
      return constants().var_redef;
    const Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::const_iterator it = m->fnmap.find(c->id());
    if (it == m->fnmap.end()) {
      return NULL;
    }
    const std::vector<FnEntry>& v = it->second;
    std::vector<FunctionI*> matched;
    Expression* botarg = NULL;
    for (unsigned int i=0; i<v.size(); i++) {
      const std::vector<Type>& fi_t = v[i].t;
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
      std::cerr << "try " << *v[i].fi;
#endif
      if (fi_t.size() == c->n_args()) {
        bool match=true;
        for (unsigned int j=0; j<c->n_args(); j++) {
          if (!env.isSubtype(c->arg(j)->type(),fi_t[j],strictEnums)) {
#ifdef MZN_DEBUG_FUNCTION_REGISTRY
            std::cerr << c->arg(j)->type().toString(env) << " does not match "
            << fi_t[j].toString(env) << "\n";
            std::cerr << "Wrong argument is " << *c->arg(j);
#endif
            match=false;
            break;
          }
          if (c->arg(j)->type().isbot() && fi_t[j].bt()!=Type::BT_TOP) {
            botarg = c->arg(j);
          }
        }
        if (match) {
          if (botarg)
            matched.push_back(v[i].fi);
          else
            return v[i].fi;
        }
      }
    }
    if (matched.empty())
      return NULL;
    if (matched.size()==1)
      return matched[0];
    Type t = matched[0]->ti()->type();
    t.ti(Type::TI_PAR);
    for (unsigned int i=1; i<matched.size(); i++) {
      if (!env.isSubtype(t,matched[i]->ti()->type(),strictEnums))
        throw TypeError(env, botarg->loc(), "ambiguous overloading on return type of function");
    }
    return matched[0];
  }

  FunctionI*
  Model::matchRevMap(EnvI& env, const Type& t0) const {
    const Model* m = this;
    while (m->_parent)
      m = m->_parent;
    Type t = t0;
    t.enumId(0);
    RevMapperMap::const_iterator it = revmapmap.find(t.toInt());
    if (it != revmapmap.end()) {
      return it->second;
    } else {
      return NULL;
    }
  }
  
  Item*&
  Model::operator[] (int i) { assert(i < _items.size()); return _items[i]; }
  const Item*
  Model::operator[] (int i) const { assert(i < _items.size()); return _items[i]; }
  unsigned int
  Model::size(void) const { return static_cast<unsigned int>(_items.size()); }
  
  std::vector<Item*>::iterator
  Model::begin(void) { return _items.begin(); }

  std::vector<Item*>::const_iterator
  Model::begin(void) const { return _items.begin(); }

  std::vector<Item*>::iterator
  Model::end(void) { return _items.end(); }

  std::vector<Item*>::const_iterator
  Model::end(void) const { return _items.end(); }
  
  void
  Model::compact(void) {
    struct { bool operator() (const Item* i) {
      return i->removed();
    }} isremoved;
    _items.erase(remove_if(_items.begin(),_items.end(),isremoved),
                 _items.end());
  }
  
}
