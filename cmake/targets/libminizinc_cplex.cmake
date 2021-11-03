### MiniZinc CPLEX Solver Target

### Compile target for the CPlex interface
add_library(minizinc_cplex OBJECT
	solvers/MIP/MIP_cplex_solverfactory.cpp
	solvers/MIP/MIP_cplex_wrap.cpp

	include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh
	include/minizinc/solvers/MIP/MIP_cplex_wrap.hh
)

if(NOT CPLEX_PLUGIN)
  target_include_directories(minizinc_cplex PRIVATE ${CPLEX_INCLUDE_DIRS})
	target_link_libraries(mzn ${CPLEX_LIBRARIES})
	set_target_properties(minizinc_cplex PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
	set_target_properties(mzn PROPERTIES COMPILE_FLAGS ${CPLEX_COMPILE_FLAGS})
endif()

### Setup correct compilation into the MiniZinc library
add_dependencies(minizinc_cplex minizinc_mip)
target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_cplex>)
