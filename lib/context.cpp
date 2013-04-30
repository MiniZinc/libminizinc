/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/context.hh>
#include <minizinc/exception.hh>

#include <algorithm>

namespace MiniZinc {

  void
  ASTContext::registerFn(ASTContext& ctx) {
    if (!fnmap.empty())
      throw InternalError("cannot initialize non-empty context");
    fnmap = ctx.fnmap;
  }

  void
  ASTContext::registerFn(FunctionI* fi) {
    FnMap::iterator i_id = fnmap.find(fi->_id);
    if (i_id == fnmap.end()) {
      // new element
      std::vector<FunctionI*> v; v.push_back(fi);
      fnmap.insert(std::pair<CtxStringH,std::vector<FunctionI*> >(fi->_id,v));
    } else {
      // add to list of existing elements
      std::vector<FunctionI*>& v = i_id->second;
      for (unsigned int i=0; i<v.size(); i++) {
        if (v[i]->_params->size() == fi->_params->size()) {
          bool alleq=true;
          for (unsigned int j=0; j<fi->_params->size(); j++) {
            if ((*v[i]->_params)[j]->_type != (*fi->_params)[j]->_type) {
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

  FunctionI* ASTContext::matchFn(const CtxStringH& id,
                                 const std::vector<Type>& t) {
    FnMap::iterator i_id = fnmap.find(id);
    if (i_id == fnmap.end()) {
      assert(false);
      return NULL; // builtin not defined. TODO: should this be an error?
    }
    std::vector<FunctionI*>& v = i_id->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params->size() == t.size()) {
        bool match=true;
        for (unsigned int j=0; j<t.size(); j++) {
          if (!t[j].isSubtypeOf((*fi->_params)[j]->_type)) {
            match=false;
            break;
          }
        }
        if (match)
          return fi;
      }
    }
    assert(false);
    return NULL;
  }

  namespace {
    class FunSort {
    public:
      bool operator()(FunctionI* x, FunctionI* y) const {
        if (x->_params->size() < y->_params->size())
          return true;
        if (x->_params->size() == y->_params->size()) {
          for (unsigned int i=0; i<x->_params->size(); i++) {
            switch ((*x->_params)[i]->_type.cmp((*y->_params)[i]->_type)) {
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
  ASTContext::sortFn(void) {
    FunSort funsort;
    for (FnMap::iterator it=fnmap.begin(); it!=fnmap.end(); ++it) {
      std::sort(it->second.begin(),it->second.end(),funsort);
    }
  }

  FunctionI*
  ASTContext::matchFn(const CtxStringH& id,
                      const std::vector<Expression*>& args) const {
    FnMap::const_iterator it = fnmap.find(id);
    if (it == fnmap.end()) {
      return NULL;
    }
    const std::vector<FunctionI*>& v = it->second;
    for (unsigned int i=0; i<v.size(); i++) {
      FunctionI* fi = v[i];
      if (fi->_params->size() == args.size()) {
        bool match=true;
        for (unsigned int j=0; j<args.size(); j++) {
          if (!args[j]->_type.isSubtypeOf((*fi->_params)[j]->_type)) {
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

  void
  ASTContext::trail(VarDecl* v) {
    vdtrail.push_back(TItem(v,v->_e));
  }
  void
  ASTContext::mark(void) {
    if (!vdtrail.empty())
      vdtrail.back().mark = true;
  }
  void
  ASTContext::untrail(void) {
    while (!vdtrail.empty() && !vdtrail.back().mark) {
      vdtrail.back().v->_e = vdtrail.back().e;
      vdtrail.pop_back();
    }
    if (!vdtrail.empty())
      vdtrail.back().mark = false;
  }
  void
  ASTContext::push_allocator(int a) {
    cur_balloc.push_back(a);
  }
  void
  ASTContext::pop_allocator(void) {
    cur_balloc.pop_back();
  }
}
