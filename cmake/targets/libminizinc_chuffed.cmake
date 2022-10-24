### MiniZinc Chuffed Solver Target

if(CHUFFED_FOUND)

  ### Compile target for the Chuffed interface
  add_library(minizinc_chuffed OBJECT
    solvers/chuffed/chuffed_solverfactory.cpp
    solvers/chuffed/chuffed_solverinstance.cpp

    include/minizinc/solvers/chuffed_solverfactory.hh
    include/minizinc/solvers/chuffed_solverinstance.hh
  )
  target_include_directories(minizinc_chuffed PRIVATE "${CHUFFED_INCLUDE_DIRS}")
  target_compile_definitions(minizinc_chuffed PRIVATE CHUFFED_VERSION="${chuffed_VERSION}")
  add_dependencies(minizinc_chuffed minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_CHUFFED)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_chuffed>)

  target_link_libraries(mzn ${CHUFFED_LIBRARIES})

  ### Copy minizinc library from Chuffed (so we don't have to keep a copy in this repo)
  file(COPY "${CHUFFED_SHARE_DIR}/minizinc/chuffed" DESTINATION "${CMAKE_BINARY_DIR}/share/minizinc")
  install(
    DIRECTORY "${CHUFFED_SHARE_DIR}/minizinc/chuffed"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/minizinc"
  )

  ### Add Chuffed's msc file but remove the executable field so it loads the builtin solver
  file(READ "${CHUFFED_SHARE_DIR}/minizinc/solvers/chuffed.msc" chuffed_solver_config)
  string(REGEX REPLACE "\"org.chuffed.chuffed\"" "\"org.minizinc.chuffed\"" chuffed_solver_config ${chuffed_solver_config})
  string(REGEX REPLACE "\"executable\":" "\"_executable\":" chuffed_solver_config ${chuffed_solver_config})
  file(WRITE "${CMAKE_BINARY_DIR}/share/minizinc/solvers/chuffed.msc" ${chuffed_solver_config})
  install(
    FILES "${CMAKE_BINARY_DIR}/share/minizinc/solvers/chuffed.msc"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/minizinc/solvers"
  )

else()

  ### Remove Chuffed's library if present, so that it doesn't get accidentally picked up
  file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/share/minizinc/chuffed")
  file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/share/minizinc/chuffed")
  file(REMOVE "${CMAKE_BINARY_DIR}/share/minizinc/solvers/chuffed.msc")

endif()
