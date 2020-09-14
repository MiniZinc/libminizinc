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
void memcpy_s(char* dest, size_t, const char* src, size_t count) { memcpy(dest, src, count); }
}  // namespace
#endif

namespace MiniZinc {

double ASTString::similarity(const ASTString& other) const {
  int m = size();
  int n = other.size();
  const char* s = c_str();
  const char* t = other.c_str();
  assert(m > 0);
  assert(n > 0);

  // dynamic programming matrix
  std::vector<int> dp((m + 1) * (n + 1), 0);
  // initialise matrix
  for (int i = 0; i <= m; i++) {
    dp[i * n] = i;
  }
  for (int i = 0; i <= n; i++) {
    dp[i] = i;
  }

  for (int i = 1; i <= m; i++) {
    for (int j = 1; j <= n; j++) {
      int v = std::min(dp[(i - 1) * n + j] + 1,                           // deletion
                       dp[i * n + j - 1] + 1);                            // insertion
      v = std::min(v, dp[(i - 1) * n + j - 1] + (s[i - 1] != t[j - 1]));  // substitution
      if (i > 2 && j > 2) {
        v = std::min(v, dp[(i - 2) * n + j - 2] + 1 + (s[i - 2] != t[j - 1]) +
                            (s[j - 1] != t[j - 2]));  // transposition
      }
      dp[i * n + j] = v;
    }
  }

  // Similarity is calculated as the number of non-edits required
  // as a percentage of the longer of the two identifiers

  int edits = dp[m * n + n];
  if (edits > m || edits > n) return 0.0;

  double similarity = 1.0 - static_cast<double>(edits) / static_cast<double>(std::max(n, m));

  return similarity;
}

ASTStringData::Interner& ASTStringData::interner() {
  static Interner _interner;
  return _interner;
}

ASTStringData::ASTStringData(const std::string& s)
    : ASTChunk(s.size() + sizeof(size_t) + 1, ASTNode::NID_STR) {
  memcpy_s(_data + sizeof(size_t), s.size() + 1, s.c_str(), s.size());
  *(_data + sizeof(size_t) + s.size()) = 0;
  std::hash<std::string> h;
  reinterpret_cast<size_t*>(_data)[0] = h(s);
}

ASTStringData* ASTStringData::a(const std::string& s) {
  if (s.empty()) {
    return nullptr;
  }
  auto it = interner().find({s.c_str(), s.size()});
  if (it != interner().end()) {
    return it->second;
  }
  auto as = static_cast<ASTStringData*>(alloc(1 + sizeof(size_t) + s.size()));
  new (as) ASTStringData(s);
  interner().emplace(std::make_pair(as->c_str(), as->size()), as);
  return as;
}
}  // namespace MiniZinc
