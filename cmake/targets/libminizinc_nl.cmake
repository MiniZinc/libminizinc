### MiniZinc NonLinear Executable Solver Target

add_library(minizinc_nl OBJECT
  solvers/nl/nl_components.cpp
  solvers/nl/nl_file.cpp
  solvers/nl/nl_solreader.cpp
  solvers/nl/nl_solverfactory.cpp
  solvers/nl/nl_solverinstance.cpp

  include/minizinc/solvers/nl/nl_components.hh
  include/minizinc/solvers/nl/nl_file.hh
  include/minizinc/solvers/nl/nl_solreader.hh
  include/minizinc/solvers/nl/nl_solverfactory.hh
  include/minizinc/solvers/nl/nl_solverinstance.hh
)
add_dependencies(minizinc_nl minizinc_parser)
