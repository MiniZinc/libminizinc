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
    ItemDocumentMapper* ism;
    PrettyPrinter* printer;
    void init(void);
    void p(Document* d, std::ostream& os, int width=80);
    void p(const Item* i, std::ostream& os, int width=80);
  public:
    Printer(void);
    ~Printer(void);
    
    void print(const Expression* e, std::ostream& os, int width=80);
    void print(const Item* i, std::ostream& os, int width=80);
    void print(const Model* m, std::ostream& os, int width=80);
  };

  /// Output operator for expressions
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Expression& e) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    Printer p;
    p.print(&e,s);
    return os << s.str();
  }

  /// Output operator for items
  template<class Char, class Traits>
  std::basic_ostream<Char,Traits>&
  operator <<(std::basic_ostream<Char,Traits>& os, const Item& i) {
    std::basic_ostringstream<Char,Traits> s;
    s.copyfmt(os); s.width(0);
    Printer p;
    p.print(&i,s);
    return os << s.str();
  }

}

void debugprint(MiniZinc::Expression* e);
void debugprint(MiniZinc::Item* i);
void debugprint(MiniZinc::Model* m);

#endif
