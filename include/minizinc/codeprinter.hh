/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_CODEPRINTER_HH__
#define __MINIZINC_CODEPRINTER_HH__

#include <iostream>

#include <minizinc/ast.hh>
#include <minizinc/stl_map_set.hh>

namespace MiniZinc {
  
  class CodePrinter {
  private:
    std::ostream& _os;
    int _icount;
    int _ecount;
    int _acount;
    int _mcount;
    typedef UNORDERED_NAMESPACE::unordered_map<Expression*,int> EMap;
    EMap _emap;
    typedef UNORDERED_NAMESPACE::unordered_map<Model*,int> MMap;
    MMap _mmap;
    ASTStringMap<int>::t _smap;
    
    void print(ASTString& s);
    int print(Expression* e);
    int print(Item* i);
    int print(Annotation& ann);
    int print(Model* m);
  public:
    CodePrinter(std::ostream& os);
    void print(Model* m, const std::string& functionName);
  };
  
}

#endif
