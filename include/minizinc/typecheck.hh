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

namespace MiniZinc {

  class TopoSorter {
  public:
    typedef std::vector<VarDecl*> Decls;
    typedef ASTStringMap<Decls>::t DeclMap;
    typedef std::unordered_map<VarDecl*,int> PosMap;
    
    Decls decls;
    DeclMap env;
    PosMap pos;
    
    void add(VarDecl* vd, bool unique);
    void remove(VarDecl* vd);
    
    VarDecl* checkId(const ASTString& id, const Location& loc);
    
    void run(Expression* e);
  };
  
  void typecheck(Model* m);
  
}

#endif
