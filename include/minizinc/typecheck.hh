/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <minizinc/astexception.hh>
#include <minizinc/hash.hh>
#include <minizinc/model.hh>

namespace MiniZinc {

/// Scoped variable declarations
class Scopes {
protected:
  typedef IdMap<VarDecl*> DeclMap;
  enum ScopeType { ST_TOPLEVEL, ST_FUN, ST_INNER };
  struct Scope {
    /// Map from identifiers to declarations
    DeclMap m;
    /// Type of this scope
    ScopeType st;
    /// Constructor
    Scope(ScopeType st0) : st(st0) {}
    /// Whether this scope is toplevel
    bool toplevel() const { return st == ST_TOPLEVEL; }
  };
  /// Stack of scopes
  std::vector<Scope> _s;

public:
  /// Constructor
  Scopes();

  /// Add a variable declaration
  void add(EnvI& env, VarDecl* vd);

  /// Push a new toplevel scope
  void pushToplevel();
  /// Push a new function scope
  void pushFun();
  /// Push a new scope
  void push();
  /// Pop topmost scope
  void pop();

  /// Return declaration for \a ident, or NULL if not found
  VarDecl* find(Id* ident);

  /// Find declarations with identifiers similar to \a ident
  VarDecl* findSimilar(Id* ident);
};

/// Topological sorting of items
class TopoSorter {
public:
  typedef std::vector<VarDecl*> Decls;
  typedef std::unordered_map<VarDecl*, int> PosMap;

  /// List of all declarations
  Decls decls;
  /// Scoped declarations
  Scopes scopes;
  /// Map from declarations to positions
  PosMap pos;
  /// The model
  Model* model;
  /// A set of identifiers that require a toString function (for enums)
  IdMap<bool>& needToString;
  /// A list of enum constructors that require type checking
  std::vector<Call*>& enumConstructorSetTypes;

  TopoSorter(Model* model0, IdMap<bool>& needToString0,
             std::vector<Call*>& enumConstructorSetTypes0)
      : model(model0),
        needToString(needToString0),
        enumConstructorSetTypes(enumConstructorSetTypes0) {}

  /// Add a variable declaration item
  void add(EnvI& env, VarDeclI* vd, bool handleEnums, Model* enumItems);
  /// Get variable declaration from identifier \a id
  VarDecl* get(EnvI& env, const ASTString& id, const Location& loc);

  VarDecl* checkId(EnvI& env, const ASTString& id_v, const Location& loc);
  VarDecl* checkId(EnvI& env, Id* ident, const Location& loc);
  /// Run the topological sorting for expression \a e
  void run(EnvI& env, Expression* e);
};

class TyperFn {
public:
  virtual void operator()(EnvI& env, FunctionI* fi) = 0;
};

/// Type check the model \a m
void typecheck(Env& env, Model* origModel, std::vector<TypeError>& typeErrors,
               bool ignoreUndefinedParameters, bool allowMultiAssignment, bool isFlatZinc = false);

/// Type check new assign item \a ai in model \a m
void typecheck(Env& env, Model* m, AssignI* ai);

/// Create monomorphised versions of polymorphic functions
void type_specialise(Env& env, Model* origModel, TyperFn& typer);
/// Turn monomorphised calls to standard library functions back into calls to generic functions
void type_demonomorphise_library(Env& e, Model* model);
/// Return de-monomorphised version of call identifier \a ident
std::string demonomorphise_identifier(const ASTString& ident);

/// Output description of parameters and output variables to \a os
void output_model_interface(Env& env, Model* m, std::ostream& os,
                            const std::vector<std::string>& skipDirs);

/// Output information about variable types (enum types) to \a os
void output_model_variable_types(Env& env, Model* m, std::ostream& os,
                                 const std::vector<std::string>& skipDirs);

std::string create_enum_to_string_name(Id* ident, const std::string& prefix);
}  // namespace MiniZinc
