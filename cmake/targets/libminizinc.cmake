### MiniZinc Library Target
#   Combined definition of the MiniZinc core and all solvers compiled

add_library(minizinc
  lib/flattener.cpp
  lib/passes/compile_pass.cpp
  lib/process.cpp
  lib/solns2out.cpp
  lib/solver.cpp
  lib/solver_config.cpp
  include/minizinc/flattener.hh
  include/minizinc/passes/compile_pass.hh
  include/minizinc/process.hh
  include/minizinc/solns2out.hh
  include/minizinc/solver.hh
  include/minizinc/solver_config.hh

  $<TARGET_OBJECTS:minizinc_core>
  $<TARGET_OBJECTS:minizinc_fzn>
  $<TARGET_OBJECTS:minizinc_nl>
)

if(GECODE_FOUND AND USE_GECODE)
  target_link_libraries(minizinc Gecode::Minimodel Gecode::Support)
endif()

include(cmake/targets/libminizinc_cplex.cmake)
include(cmake/targets/libminizinc_geas.cmake)
include(cmake/targets/libminizinc_gecode.cmake)
include(cmake/targets/libminizinc_gurobi.cmake)
include(cmake/targets/libminizinc_osicbc.cmake)
include(cmake/targets/libminizinc_scip.cmake)
include(cmake/targets/libminizinc_xpress.cmake)

install(
  TARGETS minizinc
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)
