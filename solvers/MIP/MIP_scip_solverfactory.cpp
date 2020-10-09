#include <minizinc/solvers/MIP/MIP_scip_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_scip_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPScipWrapper> _scip_solver_factory; }
}  // namespace
SCIPSolverFactoryInitialiser::SCIPSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
