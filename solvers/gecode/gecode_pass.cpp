
#include <minizinc/solvers/gecode/gecode_pass.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

  GecodePass::GecodePass(FlatteningOptions& opts, Options& g_opts, std::string lib) : Pass(opts), gopts(g_opts), library(lib) {}

  std::string GecodePass::getLibrary() { return library; }

  void GecodePass::run(Env& env) {
    GecodeSolverInstance gecode(env,gopts);
    gecode.processFlatZinc();
    gecode.presolve(env.flat());
  }
}

