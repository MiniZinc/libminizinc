/* -*- mode: C++; c-basic-offset: 2; indent-tabs-mode: nil -*- */

/*
 *  Main authors:
 *     Kevin Leo <kevin.leo@monash.edu>
 */

/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include <minizinc/passes/gecode_pass.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

  GecodePass::GecodePass(GecodeOptions* g_opts) : gopts(g_opts) {}

  Env* GecodePass::run(Env* env, std::ostream& log) {
    try {
      GecodeSolverInstance gecode(*env,log,new GecodeOptions(*gopts));
      gecode.processFlatZinc();
      gecode.presolve(env->flat());
    } catch(InternalError e) {
      std::cerr << "Warning during presolve: " << e.msg() << std::endl;

    }
    return env;
  }
}

