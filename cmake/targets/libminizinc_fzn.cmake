### MiniZinc FlatZinc Executable Solver Target

add_library(minizinc_fzn
  solvers/fzn/fzn_solverinstance.cpp
  solvers/fzn/fzn_solverfactory.cpp
  include/minizinc/solvers/fzn_solverfactory.hh
  include/minizinc/solvers/fzn_solverinstance.hh
  solvers/mzn/mzn_solverinstance.cpp
  solvers/mzn/mzn_solverfactory.cpp
  include/minizinc/solvers/mzn_solverfactory.hh
  include/minizinc/solvers/mzn_solverinstance.hh
)

target_link_libraries(minizinc_fzn minizinc_compiler)

install(
    TARGETS minizinc_fzn
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
)
