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
#include <minizinc/gc.hh>
#include <minizinc/ast.hh>

namespace MiniZinc {
  
  /// A MiniZinc model
  class Model {
    friend class GC;
  protected:
    /// Previous model in root set list
    Model* _roots_prev;
    /// Next model in root set list
    Model* _roots_next;

    /// Type of map from identifiers to function declarations
    typedef ASTStringMap<std::vector<FunctionI*> >::t FnMap;
    /// Map from identifiers to function declarations
    FnMap fnmap;

    /// Filename of the model
    ASTString _filename;
    /// Path of the model
    ASTString _filepath;
    /// Parent model if model was included
    Model* _parent;
    /// Items in the model
    std::vector<Item*> _items;
  public:
    
    /// Construct empty model
    Model(void);
    /// Destructor
    ~Model(void);
    
    /// Add \a i to the model
    void addItem(Item* i) { _items.push_back(i); }
    
    /// Get parent model
    Model* parent(void) const { return _parent; }
    /// Set parent model to \a p
    void setParent(Model* p) { assert(_parent==NULL); _parent = p; }
    
    /// Get file name
    ASTString filename(void) const { return _filename; }
    /// Get file path
    ASTString filepath(void) const { return _filepath; }
    /// Set file name
    void setFilename(const std::string& f) {
      assert(_filename.size()==0);
      _filename = ASTString(f);
    }
    /// Set file path
    void setFilepath(const std::string& f) {
      assert(_filepath.size()==0);
      _filepath = ASTString(f);
    }

    /// Register a builtin function item
    void registerFn(FunctionI* fi);
    /// Sort functions by type
    void sortFn(void);
    /// Return function declaration for \a id matching \a args
    FunctionI* matchFn(const ASTString& id,
                       const std::vector<Expression*>& args) const;
    /// Return function declaration for \a id matching types \a t
    FunctionI* matchFn(const ASTString& id, const std::vector<Type>& t);
    /// Return function declaration matching call \a c
    FunctionI* matchFn(Call* c) const;

    /// Return item \a i
    Item*& operator[] (int i);
    /// Return item \a i
    const Item* operator[] (int i) const;
    /// Return number of items
    unsigned int size(void) const;
    
    typedef std::vector<Item*>::iterator iterator;
    typedef std::vector<Item*>::const_iterator const_iterator;
    
    /// Iterator for beginning of items
    iterator begin(void);
    /// Iterator for beginning of items
    const_iterator begin(void) const;
    /// Iterator for end of items
    iterator end(void);
    /// Iterator for end of items
    const_iterator end(void) const;

    /// Remove all items marked as removed
    void compact(void);
  };

  /// Visitor for model items
  class ItemVisitor {
  public:
    /// Visit variable declaration
    void vVarDeclI(VarDeclI*) {}
    /// Visit assign item
    void vAssignI(AssignI*) {}
    /// Visit constraint item
    void vConstraintI(ConstraintI*) {}
    /// Visit solve item
    void vSolveI(SolveI*) {}
    /// Visit output item
    void vOutputI(OutputI*) {}
    /// Visit function item
    void vFunctionI(FunctionI*) {}
  };

  /// Iterator over items in a model and all its included models
  template<class I>
  class ItemIter {
  protected:
    I& iter;
  public:
    ItemIter(I& iter0) : iter(iter0) {}
    void run(Model* m) {
      UNORDERED_NAMESPACE::unordered_set<Model*> seen;
      std::vector<Model*> models;
      models.push_back(m);
      seen.insert(m);
      while (!models.empty()) {
        Model* cm = models.back();
        models.pop_back();
        for (unsigned int i=0; i<cm->size(); i++) {
          if ((*cm)[i]->removed())
            continue;
          switch ((*cm)[i]->iid()) {
          case Item::II_INC:
            if (seen.find((*cm)[i]->cast<IncludeI>()->m()) == seen.end()) {
              models.push_back((*cm)[i]->cast<IncludeI>()->m());
              seen.insert((*cm)[i]->cast<IncludeI>()->m());
            }
            break;
          case Item::II_VD:
            iter.vVarDeclI((*cm)[i]->cast<VarDeclI>());
            break;
          case Item::II_ASN:
            iter.vAssignI((*cm)[i]->cast<AssignI>());
            break;
          case Item::II_CON:
            iter.vConstraintI((*cm)[i]->cast<ConstraintI>());
            break;
          case Item::II_SOL:
            iter.vSolveI((*cm)[i]->cast<SolveI>());
            break;
          case Item::II_OUT:
            iter.vOutputI((*cm)[i]->cast<OutputI>());
            break;
          case Item::II_FUN:
            iter.vFunctionI((*cm)[i]->cast<FunctionI>());
            break;      
          }
        }
      }
    }
  };
  
  /// Run iterator \a i over all items of model \a m
  template<class I>
  void iterItems(I& i, Model* m) {
    ItemIter<I>(i).run(m);
  }
  
}

#endif
