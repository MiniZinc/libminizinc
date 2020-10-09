#include <minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh>
#include <minizinc/solvers/MIP/MIP_osicbc_wrap.hh>
#include <minizinc/solvers/MIP/MIP_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MIPSolverFactory<MIPosicbcWrapper> _osicbc_solver_factory; }
}  // namespace
OSICBCSolverFactoryInitialiser::OSICBCSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
