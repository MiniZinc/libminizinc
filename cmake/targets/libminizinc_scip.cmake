### MiniZinc SCIP Solver Target

if(SCIP_FOUND)

  ### Compile target for the SCIP interface
  add_library(minizinc_scip OBJECT
    solvers/MIP/MIP_scip_solverfactory.cpp
    solvers/MIP/MIP_scip_wrap.cpp

    include/minizinc/solvers/MIP/MIP_scip_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_scip_wrap.hh
  )
  target_include_directories(minizinc_scip PRIVATE ${SCIP_INCLUDE_DIRS})
  add_dependencies(minizinc_scip minizinc_mip)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_SCIP)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_scip>)

endif()
