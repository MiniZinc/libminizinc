### MiniZinc Solver Target
#   Combined definition of all solvers compiled

add_library(minizinc_solver
  lib/flattener.cpp
  lib/passes/compile_pass.cpp
  lib/process.cpp
  lib/solns2out.cpp
  lib/solver.cpp
  lib/solver_config.cpp
  include/minizinc/flattener.hh
  include/minizinc/passes/compile_pass.hh
  include/minizinc/process.hh
  include/minizinc/solns2out.hh
  include/minizinc/solver.hh
  include/minizinc/solver_config.hh
)
target_link_libraries(minizinc_solver minizinc_compiler)

if(TARGET minizinc_cplex)
  set_target_properties(minizinc_solver PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
  target_link_libraries(minizinc_solver minizinc_cplex ${CMAKE_THREAD_LIBS_INIT} ${CPLEX_LIBRARIES})
  target_compile_definitions(minizinc_solver PRIVATE HAS_CPLEX)
endif()
if(TARGET minizinc_geas)
  target_link_libraries(minizinc_solver minizinc_geas)
  target_compile_definitions(minizinc_solver PRIVATE HAS_GEAS)
endif()
if(TARGET minizinc_gecode)
  target_include_directories(minizinc_solver PRIVATE ${GECODE_INCLUDE_DIRS})
  target_link_libraries(minizinc_solver minizinc_gecode)
  target_compile_definitions(minizinc_solver PRIVATE HAS_GECODE)
endif()
if(TARGET minizinc_gurobi)
  target_link_libraries(minizinc_solver minizinc_gurobi)
  target_compile_definitions(minizinc_solver PRIVATE HAS_GUROBI)
endif()
if(TARGET minizinc_osicbc)
  target_link_libraries(minizinc_solver minizinc_osicbc ${OSICBC_TARGETS} ${CMAKE_THREAD_LIBS_INIT})
  target_compile_definitions(minizinc_solver PRIVATE HAS_OSICBC)
endif()
if(TARGET minizinc_scip)
  target_link_libraries(minizinc_solver minizinc_scip)
  target_compile_definitions(minizinc_solver PRIVATE HAS_SCIP)
endif()
if(TARGET minizinc_xpress)
  target_link_libraries(minizinc_solver minizinc_xpress)
  target_compile_definitions(minizinc_solver PRIVATE HAS_XPRESS)
endif()
target_link_libraries(minizinc_solver minizinc_fzn minizinc_nl)

install(
  TARGETS minizinc_solver
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
