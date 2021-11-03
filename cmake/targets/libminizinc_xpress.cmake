### MiniZinc FICO Xpress Solver Target

### Compile target for the Xpress interface
add_library(minizinc_xpress OBJECT
	solvers/MIP/MIP_xpress_solverfactory.cpp
	solvers/MIP/MIP_xpress_wrap.cpp

	include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh
	include/minizinc/solvers/MIP/MIP_xpress_wrap.hh
)
add_dependencies(minizinc_xpress minizinc_mip)

### Setup correct compilation into the MiniZinc library
target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_xpress>)
