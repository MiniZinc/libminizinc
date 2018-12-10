### MiniZinc SCIP Solver Target

if(SCIP_FOUND AND USE_SCIP)
  add_library(minizinc_scip
              solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_scip_wrap.cpp
              solvers/MIP/MIP_scip_solverfactory.cpp lib/algorithms/min_cut.cpp)
  target_include_directories(minizinc_scip PRIVATE ${SCIP_INCLUDE_DIRS})
  target_link_libraries(minizinc_scip minizinc ${SCIP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_scip)
  install(
    TARGETS minizinc_scip
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()