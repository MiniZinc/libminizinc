#include <minizinc/solvers/mzn_solverfactory.hh>
#include <minizinc/solvers/mzn_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static MZNSolverFactory _mzn_solverfactory; }
}  // namespace
MZNSolverFactoryInitialiser::MZNSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
