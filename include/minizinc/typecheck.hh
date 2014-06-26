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
#include <minizinc/exception.hh>

namespace MiniZinc {

  /// Topological sorting of items
  class TopoSorter {
  public:
    typedef std::vector<VarDecl*> Decls;
    typedef ASTStringMap<Decls>::t DeclMap;
    typedef UNORDERED_NAMESPACE::unordered_map<VarDecl*,int> PosMap;
    
    /// List of all declarations
    Decls decls;
    /// Map from identifiers to declarations
    DeclMap env;
    /// Map from declarations to positions
    PosMap pos;
    
    /// Add a variable declaration
    void add(VarDecl* vd, bool unique);
    /// Remove a variable declaration
    void remove(VarDecl* vd);
    
    VarDecl* checkId(const ASTString& id, const Location& loc);
    /// Run the topological sorting for expression \a e
    void run(Expression* e);
  };
  
  /// Type check the model \a m
  void typecheck(Model* m, std::vector<TypeError>& typeErrors);

  /// Type check new assign item \a ai in model \a m
  void typecheck(Model* m, AssignI* ai);

  /// Typecheck FlatZinc variable declarations
  void typecheck_fzn(Model* m);
  
}

#endif
