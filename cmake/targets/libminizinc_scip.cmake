### MiniZinc SCIP Solver Target

if(SCIP_FOUND AND USE_SCIP)

  ### Compile target for the SCIP interface
  add_library(minizinc_scip OBJECT
              solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_scip_wrap.cpp
              solvers/MIP/MIP_scip_solverfactory.cpp lib/algorithms/min_cut.cpp)
  target_include_directories(minizinc_scip PRIVATE ${SCIP_INCLUDE_DIRS})

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(minizinc PRIVATE HAS_SCIP)
  target_sources(minizinc PRIVATE $<TARGET_OBJECTS:minizinc_scip>)
  target_link_libraries(minizinc ${SCIP_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT})

endif()
