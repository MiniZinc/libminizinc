### MiniZinc OsiCBC Solver Target

if(OSICBC_FOUND)

  ### Compile target for the OsiCBC interface
  add_library(minizinc_osicbc OBJECT

    solvers/MIP/MIP_osicbc_solverfactory.cpp
    solvers/MIP/MIP_osicbc_wrap.cpp

    include/minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_osicbc_wrap.hh
  )
  target_include_directories(minizinc_osicbc PRIVATE ${OSICBC_INCLUDE_DIRS})
  add_dependencies(minizinc_osicbc minizinc_mip)
  if (UNIX AND NOT WIN32)
    target_compile_definitions(minizinc_osicbc PRIVATE HAVE_CONFIG_H)
  endif()

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_OSICBC)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_osicbc>)
  target_link_libraries(mzn ${OSICBC_TARGETS})

endif()
