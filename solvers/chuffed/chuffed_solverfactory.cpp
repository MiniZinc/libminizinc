#include <minizinc/solvers/chuffed_solverfactory.hh>
#include <minizinc/solvers/chuffed_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static ChuffedSolverFactory _chuffed_solverfactory; }
}  // namespace
ChuffedSolverFactoryInitialiser::ChuffedSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
