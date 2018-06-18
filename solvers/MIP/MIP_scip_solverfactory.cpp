#include <minizinc/solvers/MIP/MIP_scip_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_scip_wrap.hh>

namespace MiniZinc {
  namespace {
     void getWrapper() {
       static MIP_SolverFactory<MIP_scip_wrapper> _scip_solver_factory;
       return;
    }
  }
  SCIP_SolverFactoryInitialiser::SCIP_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
