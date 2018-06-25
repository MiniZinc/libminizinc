/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/astvec.hh>

namespace MiniZinc {

  ASTIntVecO::ASTIntVecO(const std::vector<int>& v)
    : ASTChunk(sizeof(int)*v.size()) {
    for (unsigned int i=static_cast<unsigned int>(v.size()); i--;)
      (*this)[i] = v[i];
  }

  ASTIntVecO*
  ASTIntVecO::a(const std::vector<int>& v) {
    ASTIntVecO* ao = static_cast<ASTIntVecO*>(alloc(sizeof(int)*v.size()));
    new (ao) ASTIntVecO(v);
    return ao;
  }
  
}