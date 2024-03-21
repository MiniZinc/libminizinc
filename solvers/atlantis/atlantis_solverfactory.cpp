#include <minizinc/solvers/atlantis_solverfactory.hh>
#include <minizinc/solvers/atlantis_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static AtlantisSolverFactory _atlantis_solverfactory; }
}  // namespace
AtlantisSolverFactoryInitialiser::AtlantisSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
