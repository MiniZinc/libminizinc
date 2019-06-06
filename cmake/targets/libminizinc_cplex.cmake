### MiniZinc CPLEX Solver Target

if(CPLEX_FOUND AND USE_CPLEX)

  ### Compile target for the CPlex interface
  add_library(minizinc_cplex OBJECT
    lib/algorithms/min_cut.cpp

    solvers/MIP/MIP_cplex_solverfactory.cpp
    solvers/MIP/MIP_cplex_wrap.cpp
    solvers/MIP/MIP_solverinstance.cpp

    include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_cplex_wrap.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh
  )
  set_target_properties(minizinc_cplex PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_include_directories(minizinc_cplex PRIVATE ${CPLEX_INCLUDE_DIRS})
  add_dependencies(minizinc_cplex minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(minizinc PRIVATE HAS_CPLEX)
  target_sources(minizinc PRIVATE $<TARGET_OBJECTS:minizinc_cplex>)
  set_target_properties(minizinc PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_link_libraries(minizinc ${CMAKE_THREAD_LIBS_INIT} ${CPLEX_LIBRARIES})
endif()
