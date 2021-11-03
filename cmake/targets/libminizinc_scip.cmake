### MiniZinc SCIP Solver Target

### Compile target for the SCIP interface
add_library(minizinc_scip OBJECT
	solvers/MIP/MIP_scip_solverfactory.cpp
	solvers/MIP/MIP_scip_wrap.cpp

	include/minizinc/solvers/MIP/MIP_scip_solverfactory.hh
	include/minizinc/solvers/MIP/MIP_scip_wrap.hh
)
add_dependencies(minizinc_scip minizinc_mip)

### Setup correct compilation into the MiniZinc library
target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_scip>)
