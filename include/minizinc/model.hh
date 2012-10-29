/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_MODEL_HH__
#define __MINIZINC_MODEL_HH__

#include <vector>
#include <minizinc/ast.hh>

namespace MiniZinc {
  
  class Model {
  public:
    CtxStringH _filename;
    CtxStringH _filepath;
    Model* _parent;
    std::vector<Item*> _items;
    
    Model(void) : _filename(NULL), _filepath(NULL), _parent(NULL) {}
    
    void addItem(Item* i) { _items.push_back(i); }
    
    Model* parent(void) const { return _parent; }
    void setParent(Model* p) { assert(_parent==NULL); _parent = p; }
    
    CtxStringH filename(void) const { return _filename; }
    CtxStringH filepath(void) const { return _filepath; }
    
    void setFilename(const ASTContext& ctx, const std::string& f) {
      assert(_filename.size()==0);
      _filename = CtxStringH(ctx,f);
    }
    void setFilepath(const ASTContext& ctx, const std::string& f) {
      assert(_filepath.size()==0);
      _filepath = CtxStringH(ctx,f);
    }
  };

  class ItemVisitor {
  public:
    void vVarDeclI(VarDeclI*) {}
    void vAssignI(AssignI*) {}
    void vConstraintI(ConstraintI*) {}
    void vSolveI(SolveI*) {}
    void vOutputI(OutputI*) {}
    void vFunctionI(FunctionI*) {}
  };

  template<class I>
  class ItemIter {
  protected:
    I& i;
  public:
    ItemIter(I& i0) : i(i0) {}
    void run(Model* m) {
      std::vector<Model*> models;
      models.push_back(m);
      while (!models.empty()) {
        Model* cm = models.back();
        models.pop_back();
        for (Item* it : cm->_items) {
          switch (it->_iid) {
          case Item::II_INC:
            if (it->cast<IncludeI>()->_own)
              models.push_back(it->cast<IncludeI>()->_m);
            break;
          case Item::II_VD:
            i.vVarDeclI(it->cast<VarDeclI>());
            break;
          case Item::II_ASN:
            i.vAssignI(it->cast<AssignI>());
            break;
          case Item::II_CON:
            i.vConstraintI(it->cast<ConstraintI>());
            break;
          case Item::II_SOL:
            i.vSolveI(it->cast<SolveI>());
            break;
          case Item::II_OUT:
            i.vOutputI(it->cast<OutputI>());
            break;
          case Item::II_FUN:
            i.vFunctionI(it->cast<FunctionI>());
            break;      
          }
        }
      }
    }
  };
  template<class I>
  void iterItems(I& i, Model* m) {
    ItemIter<I>(i).run(m);
  }
  
}

#endif
