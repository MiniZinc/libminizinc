### MiniZinc Geas Solver Target

if(GEAS_FOUND AND USE_GEAS)

  ### Compile target for the Geas interface
  add_library(minizinc_geas OBJECT
    solvers/geas/geas_constraints.cpp
    solvers/geas/geas_solverfactory.cpp
    solvers/geas/geas_solverinstance.cpp

    include/minizinc/solvers/geas/geas_constraints.hh
    include/minizinc/solvers/geas_solverfactory.hh
    include/minizinc/solvers/geas_solverinstance.hh
  )
  target_include_directories(minizinc_geas PRIVATE "${GEAS_INCLUDE_DIRS}")
  add_dependencies(minizinc_geas minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_GEAS)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_geas>)
  target_link_libraries(mzn Geas)

endif()
