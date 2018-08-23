#include <minizinc/solvers/fzn_solverfactory.hh>
#include <minizinc/solvers/fzn_solverinstance.hh>

namespace MiniZinc {
  namespace {
    void getWrapper() {
      static FZN_SolverFactory _fzn_solverfactory;
      return;
    }
  }
  FZN_SolverFactoryInitialiser::FZN_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}

