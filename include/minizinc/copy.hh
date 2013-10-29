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
  
  /// Create a deep copy of expression \a e
  Expression* copy(Expression* e);
  /// Create a deep copy of item \a i
  Item* copy(Item* i);
  /// Create a deep copy of model \a m
  Model* copy(Model* m);
  
}

#endif
