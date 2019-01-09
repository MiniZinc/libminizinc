### MiniZinc NonLinear Executable Solver Target

add_library(minizinc_nl
  solvers/nl/nl_file.cpp
  solvers/nl/nl_solverinstance.cpp
  solvers/nl/nl_solverfactory.cpp
  include/minizinc/solvers/nl/nl_file.hh
  include/minizinc/solvers/nl/nl_solverfactory.hh
  include/minizinc/solvers/nl/nl_solverinstance.hh
  solvers/mzn/mzn_solverinstance.cpp
  solvers/mzn/mzn_solverfactory.cpp
  include/minizinc/solvers/mzn_solverfactory.hh
  include/minizinc/solvers/mzn_solverinstance.hh
)

target_link_libraries(minizinc_nl minizinc_compiler)

install(
    TARGETS minizinc_nl
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
