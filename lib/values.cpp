/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Guido Tack <guido.tack@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/values.hh>

#include <climits>

namespace MiniZinc {

IntVal IntVal::minint() { return IntVal(std::numeric_limits<long long int>::min()); }
IntVal IntVal::maxint() { return IntVal(std::numeric_limits<long long int>::max()); }
IntVal IntVal::infinity() { return IntVal(1, true); }

IntSetVal::IntSetVal(IntVal m, IntVal n) : ASTChunk(sizeof(Range)) {
  get(0).min = m;
  get(0).max = n;
}

FloatSetVal::FloatSetVal(FloatVal m, FloatVal n) : ASTChunk(sizeof(Range)) {
  get(0).min = m;
  get(0).max = n;
}

FloatVal FloatVal::infinity() { return FloatVal(1.0, true); }

}  // namespace MiniZinc
