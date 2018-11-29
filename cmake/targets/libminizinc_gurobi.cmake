### MiniZinc Gurobi Solver Target

find_package(Gurobi)

if(GUROBI_FOUND)
  add_library(minizinc_gurobi
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_gurobi_wrap.cpp solvers/MIP/MIP_gurobi_solverfactory.cpp
    include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp
    lib/algorithms/min_cut.cpp
  )
  target_include_directories(minizinc_gurobi PRIVATE ${GUROBI_INCLUDE_DIRS})
  target_link_libraries(minizinc_gurobi minizinc ${GUROBI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_gurobi)
  INSTALL(
    TARGETS minizinc_gurobi
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
  )
endif()
