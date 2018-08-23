/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Pierre Wilke <wilke.pierre@gmail.com>
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_PRETTYPRINTER_HH__
#define __MINIZINC_PRETTYPRINTER_HH__

#include <iostream>

#include <minizinc/ast.hh>

namespace MiniZinc {

  class Document;
  class ItemDocumentMapper;
  class PrettyPrinter;

  class Printer {
  private:
    EnvI* env;
    ItemDocumentMapper* ism;
    PrettyPrinter* printer;
    std::ostream& _os;
    int _width;
    bool _flatZinc;
    
    void init(void);
    void p(Document* d);
    void p(const Item* i);
  public:
    Printer(std::ostream& os, int width=80, bool flatZinc=true, EnvI* env=NULL);
    ~Printer(void);
    
    void print(const Expression* e);
    void print(const Item* i);
    void print(const Model* m);
    
    static std::string escapeStringLit(const ASTString& s);

  };

  /// Output operator for expressions
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Expression& e) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    Printer p(s,0);
    p.print(&e);
    return os << s.str();
  }

  /// Output operator for items
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Item& i) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    Printer p(s);
    p.print(&i);
    return os << s.str();
  }

  void ppFloatVal(std::ostream& os, const FloatVal& fv, bool hexFloat=false);
  
}

void debugprint(MiniZinc::Expression* e);
void debugprint(MiniZinc::Item* i);
void debugprint(MiniZinc::Model* m);
void debugprint(const MiniZinc::Location& l);

#endif
