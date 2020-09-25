### Compile target for the base MIP interface

add_library(minizinc_mip OBJECT
  lib/algorithms/min_cut.cpp
  lib/utils_savestream.cpp

  solvers/MIP/MIP_solverinstance.cpp

  include/minizinc/plugin.hh
  include/minizinc/solvers/MIP/MIP_wrap.hh
  include/minizinc/solvers/MIP/MIP_solverinstance.hh
  include/minizinc/solvers/MIP/MIP_solverinstance.hpp
)
add_dependencies(minizinc_mip minizinc_parser)
