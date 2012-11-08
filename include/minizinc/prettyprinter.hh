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
    void print(Document* d, std::ostream& os, int width=80);
  public:
    Printer(void);
    ~Printer(void);
    
    void print(Expression* e, std::ostream& os, int width=80);
    void print(Item* i, std::ostream& os, int width=80);
    void print(Model* m, std::ostream& os, int width=80);
  };

}

#endif
