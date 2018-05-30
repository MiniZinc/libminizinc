#include <minizinc/solvers/mzn_solverfactory.hh>
#include <minizinc/solvers/mzn_solverinstance.hh>

namespace MiniZinc {
  namespace {
    void getWrapper() {
      static MZN_SolverFactory _mzn_solverfactory;
      return;
    }
  }
  MZN_SolverFactoryInitialiser::MZN_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
