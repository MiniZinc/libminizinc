#include <minizinc/solvers/MIP/MIP_cplex_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_cplex_wrap.hh>

namespace MiniZinc {
  namespace {
     void getWrapper() {
       static MIP_SolverFactory<MIP_cplex_wrapper> _cplex_solver_factory;
       return;
    }
  }
  Cplex_SolverFactoryInitialiser::Cplex_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
