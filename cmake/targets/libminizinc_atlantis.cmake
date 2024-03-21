### MiniZinc Atlantis Solver Target

if(atlantis_FOUND)

  ### Compile target for the Atlantis interface
  add_library(minizinc_atlantis OBJECT
    solvers/atlantis/atlantis_solverfactory.cpp
    solvers/atlantis/atlantis_solverinstance.cpp

    include/minizinc/solvers/atlantis_solverfactory.hh
    include/minizinc/solvers/atlantis_solverinstance.hh
  )
  target_link_libraries(minizinc_atlantis PRIVATE atlantis::atlantis)
  target_compile_definitions(minizinc_atlantis PRIVATE ATLANTIS_VERSION="${atlantis_FOUND}")
  add_dependencies(minizinc_atlantis minizinc_parser)

  ### Setup correct compilation into the MiniZinc library
  target_compile_definitions(mzn PRIVATE HAS_ATLANTIS)
  target_sources(mzn PRIVATE $<TARGET_OBJECTS:minizinc_atlantis>)

  target_link_libraries(mzn atlantis::atlantis)

  ### Copy minizinc library from Atlantis (so we don't have to keep a copy in this repo)
  get_property(ATLANTIS_LIBRARY TARGET atlantis::atlantis PROPERTY LOCATION)
  get_filename_component(ATLANTIS_LIB_DIR ${ATLANTIS_LIBRARY} DIRECTORY)
  set(ATLANTIS_SHARE_DIR ${ATLANTIS_LIB_DIR}/../../${CMAKE_INSTALL_DATAROOTDIR})
  file(COPY "${ATLANTIS_SHARE_DIR}/minizinc/atlantis/" DESTINATION "${CMAKE_BINARY_DIR}/share/minizinc/atlantis_internal")
  install(
    DIRECTORY "${ATLANTIS_SHARE_DIR}/minizinc/atlantis/"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/minizinc/atlantis_internal"
  )

  ### Add Atlantis' msc file but remove the executable field so it loads the builtin solver
  file(READ "${ATLANTIS_SHARE_DIR}/minizinc/solvers/atlantis.msc" atlantis_solver_config)
  string(REGEX REPLACE "\"se.uu.it.atlantis\"" "\"org.minizinc.atlantis\"" atlantis_solver_config ${atlantis_solver_config})
  string(REGEX REPLACE "\"executable\":" "\"_executable\":" atlantis_solver_config ${atlantis_solver_config})
  string(REGEX REPLACE "\"\.\./atlantis\"" "\"../atlantis_internal\"" atlantis_solver_config ${atlantis_solver_config})
  file(WRITE "${CMAKE_BINARY_DIR}/share/minizinc/solvers/atlantis_internal.msc" ${atlantis_solver_config})
  install(
    FILES "${CMAKE_BINARY_DIR}/share/minizinc/solvers/atlantis_internal.msc"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/minizinc/solvers"
  )

else()

  ### Remove Atlantis' library if present, so that it doesn't get accidentally picked up
  file(REMOVE_RECURSE "${CMAKE_BINARY_DIR}/share/minizinc/atlantis_internal")
  file(REMOVE "${CMAKE_BINARY_DIR}/share/minizinc/solvers/atlantis_internal.msc")

endif()
