/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#pragma once

#include <iostream>
#include <vector>

namespace MiniZinc {

  class Model;
  
  class HtmlDocument {
  protected:
    std::string _filename;
    std::string _title;
    std::string _doc;
  public:
    HtmlDocument(const std::string& filename, const std::string& title, const std::string& document)
    : _filename(filename), _title(title), _doc(document) {}
    std::string filename(void) const { return _filename; }
    std::string title(void) const { return _title; }
    std::string document(void) const { return _doc; }
  };
  

  class HtmlPrinter {
  public:
    static std::vector<HtmlDocument> printHtml(EnvI& env, Model* m, const std::string& basename,
                                               int splitLevel, bool includeStdLib, bool generateIndex);
  };
  
  class RSTPrinter {
  public:
    static std::vector<HtmlDocument> printRST(EnvI& env, Model* m, const std::string& basename,
                                              int splitLevel, bool includeStdLib, bool generateIndex);
  };

}
