### MiniZinc NonLinear Executable Solver Target

add_library(minizinc_nl
  solvers/nl/nl_expressions.cpp
  solvers/nl/nl_file.cpp
  solvers/nl/nl_header.cpp
  solvers/nl/nl_solverfactory.cpp
  solvers/nl/nl_solverinstance.cpp
  solvers/nl/segments/bseg.cpp
  solvers/nl/segments/rseg.cpp
  solvers/nl/segments/cseg.cpp
  solvers/nl/segments/jseg.cpp
  solvers/nl/segments/oseg.cpp

  include/minizinc/solvers/nl/nl_expressions.hh
  include/minizinc/solvers/nl/nl_file.hh
  include/minizinc/solvers/nl/nl_header.hh
  include/minizinc/solvers/nl/nl_printable.hh
  include/minizinc/solvers/nl/nl_segments.hh
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
