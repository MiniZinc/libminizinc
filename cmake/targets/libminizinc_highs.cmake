### MiniZinc HiGHS Solver Target

if(HIGHS_FOUND)

  ### Compile target for the OsiCBC interface
  add_library(minizinc_highs OBJECT

    solvers/MIP/MIP_highs_solverfactory.cpp
    solvers/MIP/MIP_highs_wrap.cpp

    include/minizinc/solvers/MIP/MIP_highs_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_highs_wrap.hh
  )
  target_include_directories(minizinc_highs PRIVATE ${HIGHS_INCLUDE_DIRS})
  add_dependencies(minizinc_highs minizinc_mip)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_HIGHS)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_highs>)
  target_link_libraries(mzn ${HIGHS_LIBRARIES})

endif()
