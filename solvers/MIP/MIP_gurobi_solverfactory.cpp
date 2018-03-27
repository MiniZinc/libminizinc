#include <minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_gurobi_wrap.hh>

namespace MiniZinc {
  namespace {
     void getWrapper() {
       static MIP_SolverFactory<MIP_gurobi_wrapper> _gurobi_solver_factory;
       return;
    }
  }
  Gurobi_SolverFactoryInitialiser::Gurobi_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
