#include <minizinc/solvers/MIP/MIP_solverinstance.hh>
#include <minizinc/solvers/MIP/MIP_xpress_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_xpress_wrap.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPxpressWrapper> _xpress_solver_factory; }
}  // namespace
XpressSolverFactoryInitialiser::XpressSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
