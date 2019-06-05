### MiniZinc OsiCBC Solver Target

if(OSICBC_FOUND AND USE_OSICBC)

  ### Compile target for the OsiCBC interface
  add_library(minizinc_osicbc OBJECT
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_osicbc_wrap.cpp lib/algorithms/min_cut.cpp
    solvers/MIP/MIP_osicbc_solverfactory.cpp include/minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_osicbc_wrap.hh
  include/minizinc/solvers/MIP/MIP_solverinstance.hpp)
  target_include_directories(minizinc_osicbc PRIVATE ${OSICBC_INCLUDE_DIRS})

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(minizinc PRIVATE HAS_OSICBC)
  target_sources(minizinc PRIVATE $<TARGET_OBJECTS:minizinc_osicbc>)
  target_link_libraries(minizinc ${OSICBC_TARGETS})
endif()
