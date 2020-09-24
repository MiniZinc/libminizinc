#include <minizinc/solvers/nl/nl_solverfactory.hh>
#include <minizinc/solvers/nl/nl_solverinstance.hh>

namespace MiniZinc {
namespace {
void getWrapper() { static NL_SolverFactory _nl_solverfactory; }
}  // namespace

NL_SolverFactoryInitialiser::NL_SolverFactoryInitialiser() { getWrapper(); }
}  // namespace MiniZinc
