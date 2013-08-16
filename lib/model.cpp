/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/model.hh>
#include <minizinc/exception.hh>

namespace MiniZinc {
  
  Model::Model(void) : _parent(NULL) {
    GC::add(this);
  }

  Model::~Model(void) {
    for (unsigned int j=0; j<_items.size(); j++) {
      Item* i = _items[j];
      if (IncludeI* ii = i->dyn_cast<IncludeI>()) {
        if (ii->own() && ii->_m) {
          delete ii->_m;
          ii->_m = NULL;
        }
      }
    }
    GC::remove(this);
  }

  void
  Model::registerFn(FunctionI* fi) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::iterator i_id = m->fnmap.find(fi->_id);
    if (i_id == m->fnmap.end()) {
      // new element
      std::vector<FunctionI*> v; v.push_back(fi);
      m->fnmap.insert(std::pair<ASTString,std::vector<FunctionI*> >(fi->_id,v));
    } else {
      // add to list of existing elements
      std::vector<FunctionI*>& v = i_id->second;
      for (unsigned int i=0; i<v.size(); i++) {
        if (v[i]->_params.size() == fi->_params.size()) {
          bool alleq=true;
          for (unsigned int j=0; j<fi->_params.size(); j++) {
            if (v[i]->_params[j]->_type != fi->_params[j]->_type) {
              alleq=false; break;
            }
          }
          if (alleq) {
            throw TypeError(fi->_loc,
              "function with the same type already defined in "
              +v[i]->_loc.toString());
          }
        }
      }
      v.push_back(fi);
    }
  }

  FunctionI*
  Model::matchFn(const ASTString& id,
                 const std::vector<Type>& t) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::iterator i_id = m->fnmap.find(id);
    if (i_id == m->fnmap.end()) {
      assert(false);
      return NULL; // builtin not defined. TODO: should this be an error?
    }
    std::vector<FunctionI*>& v = i_id->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params.size() == t.size()) {
        bool match=true;
        for (unsigned int j=0; j<t.size(); j++) {
          if (!t[j].isSubtypeOf(fi->_params[j]->_type)) {
            match=false;
            break;
          }
        }
        if (match) {
          return fi;
        }
      }
    }
    assert(false);
    return NULL;
  }

  namespace {
    class FunSort {
    public:
      bool operator()(FunctionI* x, FunctionI* y) const {
        if (x->_params.size() < y->_params.size())
          return true;
        if (x->_params.size() == y->_params.size()) {
          for (unsigned int i=0; i<x->_params.size(); i++) {
            switch (x->_params[i]->_type.cmp(y->_params[i]->_type)) {
            case -1: return true;
            case 1: return false;
            }
          }
        }
        return false;
      }
    };
  }

  void
  Model::sortFn(void) {
    Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FunSort funsort;
    for (FnMap::iterator it=m->fnmap.begin(); it!=m->fnmap.end(); ++it) {
      std::sort(it->second.begin(),it->second.end(),funsort);
    }
  }

  FunctionI*
  Model::matchFn(const ASTString& id,
                 const std::vector<Expression*>& args) const {
    const Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::const_iterator it = m->fnmap.find(id);
    if (it == m->fnmap.end()) {
      return NULL;
    }
    const std::vector<FunctionI*>& v = it->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params.size() == args.size()) {
        bool match=true;
        for (unsigned int j=0; j<args.size(); j++) {
          if (!args[j]->_type.isSubtypeOf(fi->_params[j]->_type)) {
            match=false;
            break;
          }
        }
        if (match) {
          return fi;
        }
      }
    }
    return NULL;
  }
  
  FunctionI*
  Model::matchFn(Call* c) const {
    const Model* m = this;
    while (m->_parent)
      m = m->_parent;
    FnMap::const_iterator it = m->fnmap.find(c->_id.str());
    if (it == m->fnmap.end()) {
      return NULL;
    }
    const std::vector<FunctionI*>& v = it->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params.size() == c->_args.size()) {
        bool match=true;
        for (unsigned int j=0; j<c->_args.size(); j++) {
          if (!c->_args[j]->_type.isSubtypeOf(fi->_params[j]->_type)) {
            match=false;
            break;
          }
        }
        if (match) {
          return fi;
        }
      }
    }
    return NULL;
  }
}
