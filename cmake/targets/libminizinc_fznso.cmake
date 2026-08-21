### MiniZinc FZnSO Solver Target
#
# FZnSO solvers are shared libraries loaded at run time; nothing is linked here.
# All the protocol needs is its header-only bindings, which are vendored in
# `include/minizinc/_thirdparty` so that this always builds.

add_library(minizinc_fznso OBJECT
  solvers/fznso/fznso_model.cpp
  solvers/fznso/fznso_solverinstance.cpp

  include/minizinc/solvers/fznso/fznso_model.hh
  include/minizinc/solvers/fznso_solverinstance.hh

  include/minizinc/_thirdparty/fznso.hpp
  include/minizinc/_thirdparty/fznso_types.h
)
add_dependencies(minizinc_fznso minizinc_parser)

### Setup correct compilation into the MiniZinc library
target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_fznso>)

target_link_libraries(mzn ${CMAKE_DL_LIBS})
