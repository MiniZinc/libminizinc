### MiniZinc OsiCBC Solver Target

if(OSICBC_FOUND AND USE_OSICBC)
  find_package(Threads REQUIRED)

  add_library(minizinc_osicbc
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_osicbc_wrap.cpp lib/algorithms/min_cut.cpp
    solvers/MIP/MIP_osicbc_solverfactory.cpp include/minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_osicbc_wrap.hh
  include/minizinc/solvers/MIP/MIP_solverinstance.hpp)

  target_include_directories(minizinc_osicbc PRIVATE ${OSICBC_INCLUDE_DIRS})
  target_link_libraries(minizinc_osicbc minizinc ${OSICBC_TARGETS})

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_osicbc)
  install(
    TARGETS minizinc_osicbc
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()
