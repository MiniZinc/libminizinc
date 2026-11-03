### MiniZinc Gecode Solver Target

if(Gecode_FOUND)

  ### Compile target for the Gecode interface
  add_library(minizinc_gecode OBJECT
    lib/passes/gecode_pass.cpp

    solvers/gecode/aux_brancher.hh
    solvers/gecode/fzn_space.cpp
    solvers/gecode/gecode_constraints.cpp
    solvers/gecode/gecode_solverinstance.cpp

    include/minizinc/passes/gecode_pass.hh
    include/minizinc/solvers/gecode/fzn_space.hh
    include/minizinc/solvers/gecode/gecode_constraints.hh
    include/minizinc/solvers/gecode_solverinstance.hh
  )
  target_include_directories(minizinc_gecode PRIVATE "${Gecode_INCLUDE_DIRS}")
  add_dependencies(minizinc_gecode minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_GECODE)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_gecode>)

  # Gecode's own targets pull in the libraries they depend on
  target_link_libraries(mzn Gecode::gecodedriver Gecode::gecodefloat Gecode::gecodeset)

endif()
