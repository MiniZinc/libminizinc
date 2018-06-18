#include <minizinc/solvers/MIP/MIP_xpress_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_xpress_wrap.hh>

namespace MiniZinc {
  namespace {
     void getWrapper() {
       static MIP_SolverFactory<MIP_xpress_wrapper> _xpress_solver_factory;
       return;
    }
  }
  Xpress_SolverFactoryInitialiser::Xpress_SolverFactoryInitialiser(void) {
    getWrapper();
  }
}
