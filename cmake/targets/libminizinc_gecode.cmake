### MiniZinc Gecode Solver Target

if(GECODE_FOUND AND USE_GECODE)
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
  target_include_directories(minizinc_gecode PRIVATE ${GECODE_INCLUDE_DIRS})
  target_link_libraries(minizinc_gecode Gecode::Driver Gecode::Float Gecode::Int Gecode::Kernel Gecode::Search Gecode::Set)

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_gecode)
  install(
    TARGETS minizinc_gecode
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )

  install(
    DIRECTORY solvers/gecode
    DESTINATION include/minizinc/solvers
  )

endif()
