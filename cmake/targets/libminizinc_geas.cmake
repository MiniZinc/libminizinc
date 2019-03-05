### MiniZinc Geas Solver Target

if(GEAS_FOUND AND USE_GEAS)
  add_library(minizinc_geas
    solvers/geas/geas_constraints.cpp
    solvers/geas/geas_solverfactory.cpp
    solvers/geas/geas_solverinstance.cpp
    include/minizinc/solvers/geas_solverfactory.hh
    include/minizinc/solvers/geas_solverinstance.hh
    include/minizinc/solvers/geas/geas_constraints.hh
  )
  target_link_libraries(minizinc_geas minizinc_compiler Geas)

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_geas)
  install(
    TARGETS minizinc_geas
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()