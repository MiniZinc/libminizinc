/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_HTMLPRINTER_HH
#define __MINIZINC_HTMLPRINTER_HH

#include <iostream>
#include <vector>

namespace MiniZinc {

  class Model;
  
  class HtmlDocument {
  protected:
    std::string _filename;
    std::string _doc;
  public:
    HtmlDocument(std::string filename, std::string document)
    : _filename(filename), _doc(document) {}
    std::string filename(void) const { return _filename; }
    std::string document(void) const { return _doc; }
  };
  

  class HtmlPrinter {
  public:
    static std::vector<HtmlDocument> printHtml(Model* m);
    static HtmlDocument printHtmlSinglePage(Model* m);
  };
  
}

#endif
