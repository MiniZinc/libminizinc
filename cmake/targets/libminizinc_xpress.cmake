### MiniZinc FICO Xpress Solver Target

if(XPRESS_FOUND AND USE_XPRESS)

  ### Compile target for the Xpress interface
  add_library(minizinc_xpress OBJECT
    lib/algorithms/min_cut.cpp

    solvers/MIP/MIP_solverinstance.cpp
    solvers/MIP/MIP_xpress_solverfactory.cpp
    solvers/MIP/MIP_xpress_wrap.cpp

    include/minizinc/plugin.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hpp
    include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_xpress_wrap.hh
  )

  target_include_directories(minizinc_xpress PRIVATE ${XPRESS_INCLUDE_DIRS})
  add_dependencies(minizinc_xpress minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_XPRESS)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_xpress>)

endif()
