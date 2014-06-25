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
    UNORDERED_NAMESPACE::unordered_map<Expression*,int> _emap;
    
    void print(ASTString& s);
    int print(Expression* e);
    int print(Item* i);
    int print(Annotation& ann);
  public:
    CodePrinter(std::ostream& os);
    void print(Model* m, const std::string& functionName);
  };
  
}

#endif
