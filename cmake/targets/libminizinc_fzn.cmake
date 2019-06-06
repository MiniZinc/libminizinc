### MiniZinc FlatZinc Executable Solver Target

add_library(minizinc_fzn OBJECT
  solvers/fzn/fzn_solverfactory.cpp
  solvers/fzn/fzn_solverinstance.cpp
  solvers/mzn/mzn_solverfactory.cpp
  solvers/mzn/mzn_solverinstance.cpp

  include/minizinc/solvers/fzn_solverfactory.hh
  include/minizinc/solvers/fzn_solverinstance.hh
  include/minizinc/solvers/mzn_solverfactory.hh
  include/minizinc/solvers/mzn_solverinstance.hh
)
add_dependencies(minizinc_fzn minizinc_parser)
