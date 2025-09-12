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
#include <minizinc/warning.hh>

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
class TypeError;

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
    bool isPolymorphicVariant;
    FnEntry(EnvI& env, FunctionI* fi0);
    static bool checkPoly(const EnvI& env, const Type& t);
    static bool compare(const EnvI& env, const FnEntry& e1, const FnEntry& e2);
  };

protected:
  /// Add all instances of polymorphic entry \a fe to \a entries
  static void addPolymorphicInstances(EnvI& env, Model::FnEntry& fe, std::vector<FnEntry>& entries);

  void mark() override {
    _filepath.mark();
    _filename.mark();
    for (auto& _item : _items) {
      Item::mark(_item);
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
    assert(_filename.empty());
    _filename = ASTString(f);
  }
  /// Set file name
  void setFilename(const ASTString& f) { _filename = f; }
  /// Set file path
  void setFilepath(const std::string& f) {
    assert(_filepath.empty());
    _filepath = ASTString(f);
  }
  void setFilepath(const ASTString& f) {
    assert(_filepath.empty());
    _filepath = f;
  }

  /// Register a builtin function item
  bool registerFn(EnvI& env, FunctionI* fi, bool keepSorted = false, bool throwIfDuplicate = true);
  /// Sort functions by type
  void sortFn(const EnvI& env);
  /// Sort functions with same name as \a fi by type
  void sortFn(const EnvI& env, FunctionI* fi);
  /// Check that registered functions do not clash wrt overloading
  void checkFnOverloading(EnvI& env);
  /// Fix function table for fi during type checking
  void fixFnMap(FunctionI* fi);
  /// Check whether all functions in function map can be flattened or evaluated
  void checkFnValid(EnvI& env, std::vector<TypeError>& errors);
  /// Return the function declaration for the reficiation for a function with identifier \a id that
  /// will take arguments \a args.
  ///
  /// WARNING: \a args is expected to include the reification argument.
  FunctionI* matchReification(EnvI& env, const ASTString& id, const std::vector<Expression*>& args,
                              bool canHalfReify, bool strictEnums) const;
  /// Return the function declaration for the reficiation for a function with identifier \a id that
  /// will take the argument types \a t.
  ///
  /// WARNING: \a t is expected to include the type of the reification variable.
  FunctionI* matchReification(EnvI& env, const ASTString& id, const std::vector<Type>& t,
                              bool canHalfReify, bool strictEnums) const;
  /// Return function declaration for \a id matching \a args
  FunctionI* matchFn(EnvI& env, const ASTString& id, const std::vector<Expression*>& args,
                     bool strictEnums) const;
  /// Return function declaration for \a id matching types \a t
  FunctionI* matchFn(EnvI& env, const ASTString& id, const std::vector<Type>& t,
                     bool strictEnums) const;
  /// Return function declaration matching call \a c
  FunctionI* matchFn(EnvI& env, Call* c, bool strictEnums, bool throwIfNotFound = false) const;
  /// Return function declarations that are potential overloads for call \a c (same identifier and
  /// same number of arguments)
  std::vector<FunctionI*> potentialOverloads(EnvI& env, Call* c) const;
  /// Return function declaration for reverse mapper for type \a t
  FunctionI* matchRevMap(EnvI& env, const Type& t) const;
  /// Check if function with this name exists
  bool fnExists(EnvI& env, const ASTString& id) const;
  /// Return all functions that could match \a c, if some of c's arguments had stronger insts (e.g.
  /// par vs var)
  std::vector<FunctionI*> possibleMatches(EnvI& env, const ASTString& ident,
                                          const std::vector<Type>& ta) const;
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
  /// Return whether model is empty
  bool empty() const;

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
  const std::vector<std::unique_ptr<Warning>>& warnings();
  std::ostream& dumpWarnings(std::ostream& os, bool werror, bool json, int exceptWarning = -1);
  void clearWarnings();
  unsigned int maxCallStack() const;
  std::ostream& evalOutput(std::ostream& os, std::ostream& log);
};

/// Boolean evaluation context
enum BCtx { C_ROOT, C_POS, C_NEG, C_MIX };

/// Evaluation context
struct Ctx {
  /// Boolean context
  BCtx b;
  /// Integer context
  BCtx i;
  /// Boolen negation flag
  bool neg;
  /// Default constructor (root context)
  Ctx() : b(C_ROOT), i(C_MIX), neg(false) {}
  /// Copy constructor
  Ctx(const Ctx& ctx) : b(ctx.b), i(ctx.i), neg(ctx.neg) {}
  /// Assignment operator
  Ctx& operator=(const Ctx& ctx) {
    if (this != &ctx) {
      b = ctx.b;
      i = ctx.i;
      neg = ctx.neg;
    }
    return *this;
  }
  /// Return true variable if in root context, nullptr otherwise
  VarDecl* partialityVar(EnvI& env) const;
};

/// Turn \a c into positive context
BCtx operator+(const BCtx& c);
/// Negate context \a c
BCtx operator-(const BCtx& c);

class CallStackItem {
private:
  EnvI& _env;
  enum CSIType { CSI_NONE, CSI_VD, CSI_REDUNDANT, CSI_SYMMETRY } _csiType;
  bool _maybePartial;

public:
  CallStackItem(EnvI& env0, Expression* e, const Ctx& ctx = Ctx());
  CallStackItem(EnvI& env0, Id* ident, IntVal i);
  void replace();
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
