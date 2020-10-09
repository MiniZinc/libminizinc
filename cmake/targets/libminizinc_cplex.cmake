### MiniZinc CPLEX Solver Target

if(CPLEX_FOUND)

  ### Compile target for the CPlex interface
  add_library(minizinc_cplex OBJECT
    solvers/MIP/MIP_cplex_solverfactory.cpp
    solvers/MIP/MIP_cplex_wrap.cpp

    include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_cplex_wrap.hh
  )
  set_target_properties(minizinc_cplex PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_include_directories(minizinc_cplex PRIVATE ${CPLEX_INCLUDE_DIRS})
  add_dependencies(minizinc_cplex minizinc_mip)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_CPLEX)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_cplex>)
  set_target_properties(mzn PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_link_libraries(mzn ${CPLEX_LIBRARIES})
endif()
