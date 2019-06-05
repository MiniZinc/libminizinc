### MiniZinc Gurobi Solver Target

if(GUROBI_FOUND)

  ### Compile target for the Gurobi interface
  add_library(minizinc_gurobi OBJECT
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_gurobi_wrap.cpp solvers/MIP/MIP_gurobi_solverfactory.cpp
    include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp
    lib/algorithms/min_cut.cpp lib/utils_savestream.cpp
  )
  target_include_directories(minizinc_gurobi PRIVATE ${GUROBI_INCLUDE_DIRS})

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(minizinc PRIVATE HAS_GUROBI)
  target_sources(minizinc PRIVATE $<TARGET_OBJECTS:minizinc_gurobi>)
  target_link_libraries(minizinc ${GUROBI_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

endif()
