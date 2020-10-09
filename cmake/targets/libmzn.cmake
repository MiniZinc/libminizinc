### MiniZinc Library Target
#   Combined definition of the MiniZinc core and all solvers compiled

include(cmake/targets/libminizinc_parser.cmake)
include(cmake/targets/libminizinc_fzn.cmake)
include(cmake/targets/libminizinc_nl.cmake)
include(cmake/targets/libminizinc_mip.cmake)

add_library(mzn
  lib/MIPdomains.cpp
  lib/ast.cpp
  lib/astexception.cpp
  lib/astmap.cpp
  lib/aststring.cpp
  lib/astvec.cpp
  lib/builtins.cpp
  lib/cdecode.c
  lib/cencode.c
  lib/chain_compressor.cpp
  lib/copy.cpp
  lib/eval_par.cpp
  lib/file_utils.cpp
  lib/flatten.cpp
  lib/flatten/flat_exp.cpp
  lib/flatten/flatten_anon.cpp
  lib/flatten/flatten_arrayaccess.cpp
  lib/flatten/flatten_arraylit.cpp
  lib/flatten/flatten_binop.cpp
  lib/flatten/flatten_call.cpp
  lib/flatten/flatten_comp.cpp
  lib/flatten/flatten_id.cpp
  lib/flatten/flatten_ite.cpp
  lib/flatten/flatten_let.cpp
  lib/flatten/flatten_par.cpp
  lib/flatten/flatten_setlit.cpp
  lib/flatten/flatten_unop.cpp
  lib/flatten/flatten_vardecl.cpp
  lib/flattener.cpp
  lib/gc.cpp
  lib/htmlprinter.cpp
  lib/json_parser.cpp
  lib/lexer.lxx
  lib/thirdparty/miniz.c
  lib/model.cpp
  lib/optimize.cpp
  lib/optimize_constraints.cpp
  lib/output.cpp
  lib/param_config.cpp
  lib/parser.cpp
  lib/parser.yxx
  lib/passes/compile_pass.cpp
  lib/pathfileprinter.cpp
  lib/prettyprinter.cpp
  lib/solns2out.cpp
  lib/solver.cpp
  lib/solver_config.cpp
  lib/solver_instance_base.cpp
  lib/statistics.cpp
  lib/type.cpp
  lib/typecheck.cpp
  lib/values.cpp
  lib/support/regex/parser.yxx
  lib/support/regex/lexer.lxx

  include/minizinc/ast.hh
  include/minizinc/ast.hpp
  include/minizinc/astexception.hh
  include/minizinc/astiterator.hh
  include/minizinc/astmap.hh
  include/minizinc/aststring.hh
  include/minizinc/astvec.hh
  include/minizinc/builtins.hh
  include/minizinc/chain_compressor.hh
  include/minizinc/config.hh.in
  include/minizinc/copy.hh
  include/minizinc/eval_par.hh
  include/minizinc/exception.hh
  include/minizinc/file_utils.hh
  include/minizinc/flat_exp.hh
  include/minizinc/flatten.hh
  include/minizinc/flatten_internal.hh
  include/minizinc/flattener.hh
  include/minizinc/gc.hh
  include/minizinc/hash.hh
  include/minizinc/htmlprinter.hh
  include/minizinc/interrupt.hh
  include/minizinc/iter.hh
  include/minizinc/json_parser.hh
  include/minizinc/model.hh
  include/minizinc/optimize.hh
  include/minizinc/optimize_constraints.hh
  include/minizinc/output.hh
  include/minizinc/param_config.hh
  include/minizinc/parser.hh
  include/minizinc/passes/compile_pass.hh
  include/minizinc/pathfileprinter.hh
  include/minizinc/prettyprinter.hh
  include/minizinc/process.hh
  include/minizinc/solns2out.hh
  include/minizinc/solver.hh
  include/minizinc/solver_config.hh
  include/minizinc/solver_instance.hh
  include/minizinc/solver_instance_base.hh
  include/minizinc/statistics.hh
  include/minizinc/support/regex.hh
  include/minizinc/_thirdparty/b64/cdecode.h
  include/minizinc/_thirdparty/b64/cencode.h
  include/minizinc/_thirdparty/b64/decode.h
  include/minizinc/_thirdparty/b64/encode.h
  include/minizinc/_thirdparty/miniz.h
  include/minizinc/timer.hh
  include/minizinc/type.hh
  include/minizinc/typecheck.hh
  include/minizinc/utils.hh
  include/minizinc/values.hh

  $<TARGET_OBJECTS:minizinc_parser>
  $<TARGET_OBJECTS:minizinc_fzn>
  $<TARGET_OBJECTS:minizinc_mip>
  $<TARGET_OBJECTS:minizinc_nl>
)
target_link_libraries(mzn ${CMAKE_THREAD_LIBS_INIT})

### Add Solver Interfaces to the MiniZinc library when available
include(cmake/targets/libminizinc_cplex.cmake)
include(cmake/targets/libminizinc_geas.cmake)
include(cmake/targets/libminizinc_gecode.cmake)
include(cmake/targets/libminizinc_gurobi.cmake)
include(cmake/targets/libminizinc_osicbc.cmake)
include(cmake/targets/libminizinc_scip.cmake)
include(cmake/targets/libminizinc_xpress.cmake)

if(GECODE_FOUND)
  target_link_libraries(mzn Gecode::Minimodel Gecode::Support)
endif()


### Add all necessary files to the install target
install(
  TARGETS mzn
  EXPORT libminizincTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)
install(
  DIRECTORY share/minizinc
  DESTINATION ${CMAKE_INSTALL_DATAROOTDIR}
)
install(
  DIRECTORY include/minizinc
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
  PATTERN config.hh.in EXCLUDE
)
install(
  DIRECTORY lib/cached/minizinc
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
