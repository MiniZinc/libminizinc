#include <minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_gurobi_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPGurobiWrapper> _gurobi_solver_factory; }
}  // namespace
GurobiSolverFactoryInitialiser::GurobiSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
