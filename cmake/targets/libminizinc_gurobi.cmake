### MiniZinc Gurobi Solver Target

### Compile target for the Gurobi interface
add_library(minizinc_gurobi OBJECT
	solvers/MIP/MIP_gurobi_solverfactory.cpp
	solvers/MIP/MIP_gurobi_wrap.cpp

	include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh
	include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh
	)
add_dependencies(minizinc_gurobi minizinc_mip)

### Link headers and library if not a plugin
if(NOT GUROBI_PLUGIN)
  target_include_directories(minizinc_gurobi PRIVATE ${GUROBI_INCLUDE_DIRS})
  target_link_libraries(mzn ${GUROBI_LIBRARIES})
endif()

### Setup correct compilation into the MiniZinc library
target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_gurobi>)

