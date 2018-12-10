### MiniZinc CPLEX Solver Target

find_package(cplex)

if(CPLEX_FOUND AND USE_CPLEX)
  find_package(Threads REQUIRED)

  add_library(minizinc_cplex
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_cplex_wrap.cpp lib/algorithms/min_cut.cpp
    solvers/MIP/MIP_cplex_solverfactory.cpp include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_cplex_wrap.hh
  )

  set_target_properties(minizinc_cplex PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_include_directories(minizinc_cplex PRIVATE ${CPLEX_INCLUDE_DIRS})
  target_link_libraries(minizinc_cplex minizinc ${CMAKE_THREAD_LIBS_INIT} ${CPLEX_LIBRARIES})

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_cplex)

  install(
    TARGETS minizinc_cplex
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()
