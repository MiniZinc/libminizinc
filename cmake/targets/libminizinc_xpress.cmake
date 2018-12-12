### MiniZinc FICO Xpress Solver Target

if(XPRESS_FOUND AND USE_XPRESS)

  add_library(minizinc_xpress
              solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_xpress_wrap.cpp solvers/MIP/MIP_xpress_solverfactory.cpp
              include/minizinc/solvers/MIP/MIP_xpress_wrap.hh include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh
              include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp)

  target_include_directories(minizinc_xpress PRIVATE ${XPRESS_INCLUDE_DIRS})
  target_link_libraries(minizinc_xpress minizinc xprb xprs ${CMAKE_THREAD_LIBS_INIT})

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_xpress)
  install(
    TARGETS minizinc_xpress
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()
