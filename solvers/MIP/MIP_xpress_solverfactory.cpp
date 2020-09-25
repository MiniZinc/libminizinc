#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_xpress_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_xpress_wrap.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIP_SolverFactory<MIP_xpress_wrapper> _xpress_solver_factory; }
}  // namespace
Xpress_SolverFactoryInitialiser::Xpress_SolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
