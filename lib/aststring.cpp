/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/aststring.hh>
#include <iostream>

#ifndef HAS_MEMCPY_S
namespace {
  void memcpy_s(char* dest, size_t, const char* src, size_t count) {
    memcpy(dest,src,count);
  }
}
#endif

namespace MiniZinc {

  ASTStringData::Interner ASTStringData::interner = Interner(1024);

  ASTStringData::ASTStringData(const std::string& s)
    : ASTChunk(s.size()+sizeof(size_t)+1, ASTNode::NID_STR) {
    memcpy_s(_data+sizeof(size_t),s.size()+1,s.c_str(),s.size());
    *(_data+sizeof(size_t)+s.size())=0;
    std::hash<std::string> h;
    reinterpret_cast<size_t*>(_data)[0] = h(s);
  }

  ASTStringData*
  ASTStringData::a(const std::string& s) {
    if (s.empty()) {
      return nullptr;
    }
    auto it = interner.find({s.c_str(), s.size()});
    if (it != interner.end()) {
      return it->second;
    }
    auto as = static_cast<ASTStringData*>(alloc(1+sizeof(size_t)+s.size()));
    new (as) ASTStringData(s);
    interner.emplace(std::make_pair(as->c_str(), as->size()), as);
    return as;
  }
  
}
