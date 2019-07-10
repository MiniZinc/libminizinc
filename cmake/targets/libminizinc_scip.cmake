### MiniZinc SCIP Solver Target

if(SCIP_FOUND AND USE_SCIP)

  ### Compile target for the SCIP interface
  add_library(minizinc_scip OBJECT
    lib/algorithms/min_cut.cpp

    solvers/MIP/MIP_scip_solverfactory.cpp
    solvers/MIP/MIP_scip_wrap.cpp
    solvers/MIP/MIP_solverinstance.cpp

    include/minizinc/solvers/MIP/MIP_scip_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_scip_wrap.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hpp
  )
  target_include_directories(minizinc_scip PRIVATE ${SCIP_INCLUDE_DIRS})
  add_dependencies(minizinc_scip minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_SCIP)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_scip>)
  target_link_libraries(mzn ${SCIP_LIBRARIES})

endif()
