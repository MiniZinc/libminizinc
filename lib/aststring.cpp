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
#include <vector>

#ifndef HAS_MEMCPY_S
namespace {
void memcpy_s(char* dest, size_t /*size*/, const char* src, size_t count) {
  memcpy(dest, src, count);
}
}  // namespace
#endif

namespace MiniZinc {

int ASTString::levenshteinDistance(const ASTString& other) const {
  size_t m = size();
  size_t n = other.size();
  const char* s = c_str();
  const char* t = other.c_str();
  assert(m > 0);
  assert(n > 0);

  // dynamic programming matrix
  std::vector<int> dp0(n + 1);
  std::vector<int> dp1(n + 1, 0);
  // initialise matrix
  for (int i = 0; i <= n; i++) {
    dp0[i] = i;
  }

  for (int i = 1; i <= m; i++) {
    dp1[0] = i;
    for (int j = 1; j <= n; j++) {
      int del = dp0[j] + 1;
      int ins = dp1[j - 1] + 1;
      int sub = dp0[j - 1] + static_cast<int>(s[i - 1] != t[j - 1]);
      dp1[j] = std::min(del, std::min(ins, sub));
    }
    std::swap(dp0, dp1);
  }
  return dp0[n];
}

ASTStringData::Interner& ASTStringData::interner() {
  static Interner _interner;
  return _interner;
}

ASTStringData::ASTStringData(const std::string& s)
    : ASTChunk(s.size() + sizeof(size_t) + 1, ASTNode::NID_STR) {

  // Get a pointer to the start of the data area
  char* dataPtr = _data; // Access the _data member inherited from ASTChunk

  // Compute the hash
  CStringHash hasher;
  size_t hashValue = hasher(std::make_pair(s.c_str(), s.size()));

  // Store the hash
  *reinterpret_cast<size_t*>(dataPtr) = hashValue;

  // Calculate pointer after size_t
  char* stringPtr = dataPtr + sizeof(size_t);

  // Copy the string into it.
  std::memcpy(stringPtr, s.c_str(), s.size());
  stringPtr[s.size()] = 0; // Null terminate
}

ASTStringData* ASTStringData::a(const std::string& s) {
  if (s.empty()) {
    return nullptr;
  }
  auto it = interner().find({s.c_str(), s.size()});
  if (it != interner().end()) {
    return it->second;
  }
  auto* as = static_cast<ASTStringData*>(alloc(1 + sizeof(size_t) + s.size()));
  new (as) ASTStringData(s);
  interner().emplace(std::make_pair(as->c_str(), as->size()), as);
  return as;
}
}  // namespace MiniZinc
