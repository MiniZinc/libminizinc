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

  ASTStringO::ASTStringO(const std::string& s)
    : ASTChunk(s.size()+sizeof(size_t)+1) {
    memcpy_s(_data+sizeof(size_t),s.size()+1,s.c_str(),s.size());
    *(_data+sizeof(size_t)+s.size())=0;
    std::hash<std::string> h;
    reinterpret_cast<size_t*>(_data)[0] = h(s);
  }

  ASTStringO*
  ASTStringO::a(const std::string& s) {
    ASTStringO* as =
      static_cast<ASTStringO*>(alloc(1+sizeof(size_t)+s.size()));
    new (as) ASTStringO(s);
    return as;
  }
  
}
