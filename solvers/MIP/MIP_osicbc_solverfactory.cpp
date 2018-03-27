#include <minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_osicbc_wrap.hh>

namespace MiniZinc {
  namespace {
     void getWrapper() {
       static MIP_SolverFactory<MIP_osicbc_wrapper> _osicbc_solver_factory;
       return;
    }
  }
  OSICBC_SolverFactoryInitialiser::OSICBC_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
