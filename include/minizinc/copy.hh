/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_COPY_HH__
#define __MINIZINC_COPY_HH__

#include <minizinc/model.hh>

namespace MiniZinc {
  
  Expression* copy(ASTContext& ctx, Expression* e);
  Item* copy(ASTContext& ctx, Item* e);
  Model* copy(ASTContext& ctx, Model* e);
  
}

#endif
