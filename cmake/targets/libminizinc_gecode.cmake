### MiniZinc Gecode Solver Target

find_package(Gecode)

if(GECODE_FOUND)
  find_package(Threads REQUIRED)

  add_library(minizinc_gecode
    solvers/gecode/fzn_space.cpp
    solvers/gecode/gecode_solverinstance.cpp
    solvers/gecode/gecode_solverfactory.cpp
    solvers/gecode/gecode_constraints.cpp
    lib/passes/gecode_pass.cpp
    solvers/gecode/aux_brancher.hh
    include/minizinc/passes/gecode_pass.hh
    include/minizinc/solvers/gecode/fzn_space.hh
    include/minizinc/solvers/gecode_solverinstance.hh
    include/minizinc/solvers/gecode_solverfactory.hh
    include/minizinc/solvers/gecode/gecode_constraints.hh
  )
  target_link_libraries(minizinc_gecode minizinc ${CMAKE_THREAD_LIBS_INIT})

  target_include_directories(minizinc PRIVATE ${GECODE_INCLUDE_DIRS})
  target_include_directories(minizinc_gecode PRIVATE ${GECODE_INCLUDE_DIRS})
  target_link_libraries(minizinc ${GECODE_LIBRARIES})
  target_link_libraries(minizinc_gecode ${GECODE_LIBRARIES})
  target_compile_definitions(minizinc PRIVATE HAS_GECODE)

  find_package(MPFR)
  if(MPFR_FOUND AND NOT DEFINED GECODE_NO_MPFR)
    target_include_directories(minizinc_gecode PRIVATE ${MPFR_INCLUDES})
    target_link_libraries(minizinc_gecode ${MPFR_LIBRARIES})
  endif()

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_gecode)
  INSTALL(
    TARGETS minizinc_gecode
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

  INSTALL(
    DIRECTORY solvers/gecode
    DESTINATION include/minizinc/solvers
  )

endif()
