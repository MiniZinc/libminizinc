### MiniZinc Gurobi Solver Target

if(GUROBI_FOUND)

  ### Compile target for the Gurobi interface
  add_library(minizinc_gurobi OBJECT
    solvers/MIP/MIP_gurobi_solverfactory.cpp
    solvers/MIP/MIP_gurobi_wrap.cpp

    include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh
  )
  target_include_directories(minizinc_gurobi PRIVATE ${GUROBI_INCLUDE_DIRS})
  add_dependencies(minizinc_gurobi minizinc_mip)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_GUROBI)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_gurobi>)
  target_link_libraries(mzn ${GUROBI_LIBRARIES})

endif()
