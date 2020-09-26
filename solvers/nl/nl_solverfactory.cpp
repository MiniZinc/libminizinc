#include <minizinc/solvers/nl/nl_solverfactory.hh>
#include <minizinc/solvers/nl/nl_solverinstance.hh>

namespace MiniZinc {
namespace {
void get_wrapper() { static NLSolverFactory _nl_solverfactory; }
}  // namespace

NLSolverFactoryInitialiser::NLSolverFactoryInitialiser() { get_wrapper(); }
}  // namespace MiniZinc
