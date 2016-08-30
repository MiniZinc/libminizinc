/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_TYPECHECK_HH__
#define __MINIZINC_TYPECHECK_HH__

#include <minizinc/model.hh>
#include <minizinc/hash.hh>
#include <minizinc/astexception.hh>

namespace MiniZinc {

  /// Topological sorting of items
  class TopoSorter {
  public:
    typedef std::vector<VarDecl*> Decls;
    typedef IdMap<Decls> DeclMap;
    typedef UNORDERED_NAMESPACE::unordered_map<VarDecl*,int> PosMap;
    
    /// List of all declarations
    Decls decls;
    /// Map from identifiers to declarations
    DeclMap idmap;
    /// Map from declarations to positions
    PosMap pos;
    /// The model
    Model* model;
    
    TopoSorter(Model* model0) : model(model0) {}
    
    /// Add a variable declaration
    void add(EnvI& env, VarDecl* vd, bool unique);
    /// Add a variable declaration item
    void add(EnvI& env, VarDeclI* vd, bool unique, bool handleEnums, std::vector<Item*>& enumItems);
    /// Remove a variable declaration
    void remove(EnvI& env, VarDecl* vd);
    /// Get variable declaration from identifier \a id
    VarDecl* get(EnvI& env, const ASTString& id, const Location& loc);
    
    VarDecl* checkId(EnvI& env, const ASTString& id, const Location& loc);
    VarDecl* checkId(EnvI& env, Id* id, const Location& loc);
    /// Run the topological sorting for expression \a e
    void run(EnvI& env, Expression* e);
  };
  
  /// Type check the model \a m
  void typecheck(Env& env, Model* m, std::vector<TypeError>& typeErrors,
                 bool ignoreUndefinedParameters = false);

  /// Type check new assign item \a ai in model \a m
  void typecheck(Env& env, Model* m, AssignI* ai);

  /// Typecheck FlatZinc variable declarations
  void typecheck_fzn(Env& env, Model* m);
  
}

#endif
