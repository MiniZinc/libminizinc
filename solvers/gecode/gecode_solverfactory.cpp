#include <minizinc/solvers/gecode_solverfactory.hh>
#include <minizinc/solvers/gecode_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static GecodeSolverFactory _gecode_solverfactory; }
}  // namespace
GecodeSolverFactoryInitialiser::GecodeSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
