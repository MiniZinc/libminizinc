
#include <minizinc/solvers/gecode/gecode_pass.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {

  GecodePass::GecodePass(
      FlatteningOptions& opts,
      Options& g_opts,
      std::string lib,
      bool mod,
      bool sac,
      bool shave,
      bool bounds,
      unsigned int npass)
    : Pass(opts), gopts(g_opts), library(lib), presolve_model(mod), _sac(sac), _shave(shave), _bounds(bounds), _npass(npass) {

    }

  std::string GecodePass::getLibrary() { return library; }

  void GecodePass::run(Env& env) {
    GecodeSolverInstance gecode(env,gopts);
    gecode.processFlatZinc();
    Model* m = presolve_model ? env.model() : env.flat();
    gecode.presolve(m);
  }
}

