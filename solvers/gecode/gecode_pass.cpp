
#include "gecode_pass.hh"
#include "gecode_solverinstance.hh"

namespace MiniZinc {

  GecodePass::GecodePass(
      FlatteningOptions& opts,
      Options& g_opts,
      std::string library = "gecode",
      bool mod = false,
      bool sac = false,
      bool shave = false,
      bool bounds = true,
      unsigned int npass = 1)
    : Pass(opts), _sac(sac), _shave(shave), _bounds(bounds), _npass(npass), gopts(gopts), presolve_model(mod) {

    }

  std::string GecodePass::getLibrary() { return library; }

  void GecodePass::run(Env& env) {
    GecodeSolverInstance gecode(env,gopts);
    gecode.processFlatZinc();
    Model* m = presolve_model ? env.model() : env.flat();
    gecode.presolve(m);
  }
}

