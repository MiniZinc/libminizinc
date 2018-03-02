/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#ifndef __MINIZINC_GECODE_PASS_HH__
#define __MINIZINC_GECODE_PASS_HH__

#include <minizinc/flatten.hh>
#include <minizinc/options.hh>

namespace MiniZinc {

  class GecodePass : public Pass {
    Options gopts;

    public:
    GecodePass(Options& gopts);

    Env* run(Env* e, std::ostream& log);
  };

}

#endif
