#include <minizinc/solvers/MIP/MIP_cplex_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_cplex_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void getWrapper() { static MIP_SolverFactory<MIP_cplex_wrapper> _cplex_solver_factory; }
}  // namespace
Cplex_SolverFactoryInitialiser::Cplex_SolverFactoryInitialiser() { getWrapper(); }
}  // namespace MiniZinc
