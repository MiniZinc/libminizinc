
#include "gecode_pass.hh"
#include "gecode_solverinstance.hh"

namespace MiniZinc {

  GecodePropagationPass::GecodePropagationPass(FlatteningOptions& opts, std::string library, bool sac, bool shave, bool bounds, unsigned int npass)
    : Pass(opts), _sac(sac), _shave(shave), _bounds(bounds), _npass(npass) {

    }

  std::string GecodePropagationPass::getLibrary() { return library; }

  void GecodePropagationPass::run(Env& e) {
    // Build options object
    // options.sac = _sac
    // options.shave = _shave
    // options.bounds = _bounds
    // options.npass = _npass
    Options opt;
    GecodeSolverInstance gsi(e, opt);
    //gsi.presolve();
  }
}
