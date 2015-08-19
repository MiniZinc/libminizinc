
#include <minizinc/solvers/gecode/gecode_pass.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

  GecodePass::GecodePass(Options& g_opts) : gopts(g_opts) {}

  bool GecodePass::pre(Env* env) {
    return env->flat()->size() > 0;
  }

  Env* GecodePass::run(Env* env) {
    GecodeSolverInstance gecode(*env,gopts);
    gecode.processFlatZinc();
    gecode.presolve(env->flat());
    return env;
  }
}

