#include <minizinc/solvers/MIP/MIP_cplex_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_cplex_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPCplexWrapper> _cplex_solver_factory; }
}  // namespace
CplexSolverFactoryInitialiser::CplexSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
