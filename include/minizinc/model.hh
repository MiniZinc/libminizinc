/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/ast.hh>
#include <minizinc/astmap.hh>
#include <minizinc/gc.hh>

#include <iterator>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace MiniZinc {

class VarDeclIterator;
class ConstraintIterator;
class FunctionIterator;

class CopyMap;
class EnvI;

class Model;

class VarDeclIteratorContainer {
private:
  Model* _m;

public:
  VarDeclIteratorContainer(Model* m) : _m(m) {}
  VarDeclIterator begin();
  VarDeclIterator end();
};

class ConstraintIteratorContainer {
private:
  Model* _m;

public:
  ConstraintIteratorContainer(Model* m) : _m(m) {}
  ConstraintIterator begin();
  ConstraintIterator end();
};

class FunctionIteratorContainer {
private:
  Model* _m;

public:
  FunctionIteratorContainer(Model* m) : _m(m) {}
  FunctionIterator begin();
  FunctionIterator end();
};

/// A MiniZinc model
class Model : public GCMarker {
  friend Model* copy(EnvI& env, CopyMap& cm, Model* m, bool isFlatModel);

public:
  struct FnEntry {
    std::vector<Type> t;
    FunctionI* fi;
    bool isPolymorphic;
    FnEntry(FunctionI* fi0);
    bool operator<(const FnEntry& f) const;
    static bool compare(const FnEntry& e1, const FnEntry& e2);
  };

protected:
  /// Add all instances of polymorphic entry \a fe to \a entries
  static void addPolymorphicInstances(Model::FnEntry& fe, std::vector<FnEntry>& entries);

  void mark(MINIZINC_GC_STAT_ARGS) override {
    _filepath.mark();
    _filename.mark();
    for (auto& _item : _items) {
#if defined(MINIZINC_GC_STATS)
      Item::mark(_items[j], gc_stats);
#else
      Item::mark(_item);
#endif
    }
  };

  /// Type of map from identifiers to function declarations
  using FnMap = ASTStringMap<std::vector<FnEntry>>;
  /// Map from identifiers to function declarations
  FnMap _fnmap;

  /// Type of map from Type (represented as int) to reverse mapper functions
  using RevMapperMap = std::unordered_map<int, FunctionI*>;
  /// Map from Type (represented as int) to reverse mapper functions
  RevMapperMap _revmapmap;

  /// Filename of the model
  ASTString _filename;
  /// Path of the model
  ASTString _filepath;
  /// Parent model if model was included
  Model* _parent;
  /// Items in the model
  std::vector<Item*> _items;
  /// Pointer to the solve item
  SolveI* _solveItem;
  /// Pointer to the output item
  OutputI* _outputItem;
  /// File-level documentation comment
  std::string _docComment;

  /// Store some declarations
  struct FnDecls {
    using TCheckedDecl = std::pair<bool, FunctionI*>;  // bool means that it was checked
    TCheckedDecl boundsDisj = {false, nullptr};        // SCIP's bound disjunction
  } _fnDecls;

public:
  /// Construct empty model
  Model();
  /// Destructor
  ~Model() override;

  /// Add \a i to the model
  void addItem(Item* i);

  /// Get parent model
  Model* parent() const { return _parent; }
  /// Set parent model to \a p
  void setParent(Model* p) {
    assert(_parent == nullptr);
    _parent = p;
  }

  /// Get file name
  ASTString filename() const { return _filename; }
  /// Get file path
  ASTString filepath() const { return _filepath; }
  /// Set file name
  void setFilename(const std::string& f) {
    assert(_filename.size() == 0);
    _filename = ASTString(f);
  }
  /// Set file name
  void setFilename(const ASTString& f) { _filename = f; }
  /// Set file path
  void setFilepath(const std::string& f) {
    assert(_filepath.size() == 0);
    _filepath = ASTString(f);
  }
  void setFilepath(const ASTString& f) {
    assert(_filepath.size() == 0);
    _filepath = f;
  }

  /// Register a builtin function item
  bool registerFn(EnvI& env, FunctionI* fi, bool keepSorted = false, bool throwIfDuplicate = true);
  /// Sort functions by type
  void sortFn();
  /// Check that registered functions do not clash wrt overloading
  void checkFnOverloading(EnvI& env);
  /// Fix function table after type checking
  void fixFnMap();
  /// Return function declaration for \a id matching \a args
  FunctionI* matchFn(EnvI& env, const ASTString& id, const std::vector<Expression*>& args,
                     bool strictEnums) const;
  /// Return function declaration for \a id matching types \a t
  FunctionI* matchFn(EnvI& env, const ASTString& id, const std::vector<Type>& t, bool strictEnums);
  /// Return function declaration matching call \a c
  FunctionI* matchFn(EnvI& env, Call* c, bool strictEnums, bool throwIfNotFound = false) const;
  /// Return function declaration for reverse mapper for type \a t
  FunctionI* matchRevMap(EnvI& env, const Type& t) const;
  /// Check whether functions \a f and \a g have the same overloaded variants
  bool sameOverloading(EnvI& env, const std::vector<Expression*>& args, FunctionI* f,
                       FunctionI* g) const;
  /// Merge all builtin functions into \a m
  void mergeStdLib(EnvI& env, Model* m) const;

  /// Return item \a i
  Item*& operator[](unsigned int i);
  /// Return item \a i
  const Item* operator[](unsigned int i) const;
  /// Return number of items
  unsigned int size() const;

  typedef std::vector<Item*>::iterator iterator;
  typedef std::vector<Item*>::const_iterator const_iterator;

  /// Iterator for beginning of items
  iterator begin();
  /// Iterator for beginning of items
  const_iterator begin() const;
  /// Iterator for end of items
  iterator end();
  /// Iterator for end of items
  const_iterator end() const;

  ConstraintIteratorContainer constraints();
  VarDeclIteratorContainer vardecls();
  FunctionIteratorContainer functions();

  SolveI* solveItem();

  OutputI* outputItem();
  void setOutputItem(OutputI* oi);

  /// Add a file-level documentation comment
  void addDocComment(const std::string& s) { _docComment += s; }

  /// Return the file-level documentation comment
  const std::string& docComment() const { return _docComment; }

  /// Remove all items marked as removed
  void compact();

  /// Get the stored function declarations
  FnDecls& getFnDecls() { return _fnDecls; }
};

class VarDeclIterator {
  Model* _model;
  Model::iterator _it;

public:
  typedef Model::iterator::difference_type difference_type;
  typedef Model::iterator::value_type value_type;
  typedef VarDeclI& reference;
  typedef VarDeclI* pointer;
  typedef std::forward_iterator_tag iterator_category;

  VarDeclIterator() {}
  VarDeclIterator(const VarDeclIterator& vi) : _it(vi._it) {}
  VarDeclIterator(Model* model, const Model::iterator& it) : _model(model), _it(it) {
    while (_it != _model->end() && (!(*_it)->isa<VarDeclI>() || (*_it)->removed())) {
      ++_it;
    }
  }
  ~VarDeclIterator() {}

  VarDeclIterator& operator=(const VarDeclIterator& vi) {
    if (this != &vi) {
      _it = vi._it;
    }
    return *this;
  }
  bool operator==(const VarDeclIterator& vi) const { return _it == vi._it; }
  bool operator!=(const VarDeclIterator& vi) const { return _it != vi._it; }
  VarDeclIterator& operator++() {
    do {
      ++_it;
    } while (_it != _model->end() && (!(*_it)->isa<VarDeclI>() || (*_it)->removed()));
    return *this;
  }

  reference operator*() const { return *(*_it)->cast<VarDeclI>(); }
  pointer operator->() const { return (*_it)->cast<VarDeclI>(); }
};

class ConstraintIterator {
  Model* _model;
  Model::iterator _it;

public:
  typedef Model::iterator::difference_type difference_type;
  typedef Model::iterator::value_type value_type;
  typedef ConstraintI& reference;
  typedef ConstraintI* pointer;
  typedef std::forward_iterator_tag iterator_category;

  ConstraintIterator() {}
  ConstraintIterator(const ConstraintIterator& vi) : _it(vi._it) {}
  ConstraintIterator(Model* model, const Model::iterator& it) : _model(model), _it(it) {
    while (_it != _model->end() && (!(*_it)->isa<ConstraintI>() || (*_it)->removed())) {
      ++_it;
    }
  }
  ~ConstraintIterator() {}

  ConstraintIterator& operator=(const ConstraintIterator& vi) {
    if (this != &vi) {
      _it = vi._it;
    }
    return *this;
  }
  bool operator==(const ConstraintIterator& vi) const { return _it == vi._it; }
  bool operator!=(const ConstraintIterator& vi) const { return _it != vi._it; }
  ConstraintIterator& operator++() {
    do {
      ++_it;
    } while (_it != _model->end() && (!(*_it)->isa<ConstraintI>() || (*_it)->removed()));
    return *this;
  }

  reference operator*() const { return *(*_it)->cast<ConstraintI>(); }
  pointer operator->() const { return (*_it)->cast<ConstraintI>(); }
};

class FunctionIterator {
  Model* _model;
  Model::iterator _it;

public:
  typedef Model::iterator::difference_type difference_type;
  typedef Model::iterator::value_type value_type;
  typedef FunctionI& reference;
  typedef FunctionI* pointer;
  typedef std::forward_iterator_tag iterator_category;

  FunctionIterator() {}
  FunctionIterator(const FunctionIterator& vi) : _it(vi._it) {}
  FunctionIterator(Model* model, const Model::iterator& it) : _model(model), _it(it) {
    while (_it != _model->end() && (!(*_it)->isa<FunctionI>() || (*_it)->removed())) {
      ++_it;
    }
  }
  ~FunctionIterator() {}

  FunctionIterator& operator=(const FunctionIterator& vi) {
    if (this != &vi) {
      _it = vi._it;
    }
    return *this;
  }
  bool operator==(const FunctionIterator& vi) const { return _it == vi._it; }
  bool operator!=(const FunctionIterator& vi) const { return _it != vi._it; }
  FunctionIterator& operator++() {
    do {
      ++_it;
    } while (_it != _model->end() && (!(*_it)->isa<FunctionI>() || (*_it)->removed()));
    return *this;
  }

  reference operator*() const { return *(*_it)->cast<FunctionI>(); }
  pointer operator->() const { return (*_it)->cast<FunctionI>(); }
};

class EnvI;

/// Environment
class Env {
private:
  EnvI* _e;

public:
  Env(Model* m = nullptr, std::ostream& outstream = std::cout, std::ostream& errstream = std::cerr);
  ~Env();

  Model* model();
  void model(Model* m);
  Model* flat();
  void swap();
  Model* output();
  EnvI& envi();
  const EnvI& envi() const;
  std::ostream& dumpErrorStack(std::ostream& os);
  const std::vector<std::string>& warnings();
  void clearWarnings();
  unsigned int maxCallStack() const;
  std::ostream& evalOutput(std::ostream& os);
};

class CallStackItem {
public:
  EnvI& env;
  CallStackItem(EnvI& env0, Expression* e);
  CallStackItem(EnvI& env0, Id* ident, IntVal i);
  ~CallStackItem();
};

/// Visitor for model items
class ItemVisitor {
public:
  /// Enter model
  static bool enterModel(Model* /*m*/) { return true; }
  /// Enter item
  static bool enter(Item* /*m*/) { return true; }
  /// Visit include item
  void vIncludeI(IncludeI* /*ii*/) {}
  /// Visit variable declaration
  void vVarDeclI(VarDeclI* /*vdi*/) {}
  /// Visit assign item
  void vAssignI(AssignI* /*ai*/) {}
  /// Visit constraint item
  void vConstraintI(ConstraintI* /*ci*/) {}
  /// Visit solve item
  void vSolveI(SolveI* /*si*/) {}
  /// Visit output item
  void vOutputI(OutputI* /*oi*/) {}
  /// Visit function item
  void vFunctionI(FunctionI* /*fi*/) {}
};

/// Iterator over items in a model and all its included models
template <class I>
class ItemIter {
protected:
  I& _iter;

public:
  ItemIter(I& iter) : _iter(iter) {}
  void run(Model* m) {
    std::unordered_set<Model*> seen;
    std::vector<Model*> models;
    models.push_back(m);
    seen.insert(m);
    while (!models.empty()) {
      Model* cm = models.back();
      models.pop_back();
      if (!_iter.enterModel(cm)) {
        continue;
      }
      std::vector<Model*> includedModels;
      for (auto& i : *cm) {
        if (i->removed()) {
          continue;
        }
        if (!_iter.enter(i)) {
          continue;
        }
        switch (i->iid()) {
          case Item::II_INC:
            if (seen.find(i->cast<IncludeI>()->m()) == seen.end()) {
              includedModels.push_back(i->cast<IncludeI>()->m());
              seen.insert(i->cast<IncludeI>()->m());
            }
            _iter.vIncludeI(i->cast<IncludeI>());
            break;
          case Item::II_VD:
            _iter.vVarDeclI(i->cast<VarDeclI>());
            break;
          case Item::II_ASN:
            _iter.vAssignI(i->cast<AssignI>());
            break;
          case Item::II_CON:
            _iter.vConstraintI(i->cast<ConstraintI>());
            break;
          case Item::II_SOL:
            _iter.vSolveI(i->cast<SolveI>());
            break;
          case Item::II_OUT:
            _iter.vOutputI(i->cast<OutputI>());
            break;
          case Item::II_FUN:
            _iter.vFunctionI(i->cast<FunctionI>());
            break;
        }
      }
      for (auto i = static_cast<unsigned int>(includedModels.size()); (i--) != 0U;) {
        models.push_back(includedModels[i]);
      }
    }
  }
};

/// Run iterator \a i over all items of model \a m
template <class I>
void iter_items(I& i, Model* m) {
  ItemIter<I>(i).run(m);
}

}  // namespace MiniZinc
