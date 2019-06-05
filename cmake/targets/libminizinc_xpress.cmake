### MiniZinc FICO Xpress Solver Target

if(XPRESS_FOUND AND USE_XPRESS)

  ### Compile target for the Xpress interface
  add_library(minizinc_xpress OBJECT
              solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_xpress_wrap.cpp solvers/MIP/MIP_xpress_solverfactory.cpp
              include/minizinc/solvers/MIP/MIP_xpress_wrap.hh include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh
              include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp
              lib/algorithms/min_cut.cpp
            )

  target_include_directories(minizinc_xpress PRIVATE ${XPRESS_INCLUDE_DIRS})
  target_link_libraries(minizinc_xpress minizinc_core xprb xprs ${CMAKE_THREAD_LIBS_INIT})

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(minizinc PRIVATE HAS_XPRESS)
  target_sources(minizinc PRIVATE $<TARGET_OBJECTS:minizinc_xpress>)
  target_link_libraries(minizinc xprb xprs ${CMAKE_THREAD_LIBS_INIT})

endif()
