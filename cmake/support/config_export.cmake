### Export of a CMake configuration for MiniZinc
## This allows for "find_package(minizinc)"

if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH "Installation directory for CMake files")

if(NOT IS_ABSOLUTE "${INSTALL_CMAKE_DIR}")
  set(INSTALL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}/${INSTALL_CMAKE_DIR}")
endif()

file(RELATIVE_PATH REL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}"
     "${INSTALL_CMAKE_DIR}")
file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
     "${CMAKE_INSTALL_PREFIX}/include")

# Add external (static) dependencies
if(TARGET minizinc_geas)
  install(
    FILES cmake/modules/FindGeas.cmake
    DESTINATION ${REL_CMAKE_DIR}
    COMPONENT dev
  )
  set(CONF_DEPENDENCIES "${CONF_DEPENDENCIES}find_dependency(Geas)\n")
endif()
if(TARGET minizinc_gecode)
  install(
    FILES cmake/modules/FindGecode.cmake cmake/modules/FindMPFR.cmake
    DESTINATION ${REL_CMAKE_DIR}
    COMPONENT dev
  )
  if(GECODE_HAS_GIST)
    set(_CONF_GIST " Gist")
  endif()
  set(CONF_DEPENDENCIES "${CONF_DEPENDENCIES}find_dependency(Gecode 6.0 COMPONENTS Driver Float Int Kernel Minimodel Search Set Support${_CONF_GIST})\n")
endif()
if(TARGET minizinc_osicbc)
  install(
    FILES cmake/modules/FindOsiCBC.cmake
    DESTINATION ${REL_CMAKE_DIR}
    COMPONENT dev
  )
  set(CONF_DEPENDENCIES "${CONF_DEPENDENCIES}find_dependency(OsiCBC)\n")
endif()

# Add all targets to the build-tree export set
export(TARGETS mzn
       FILE "${PROJECT_BINARY_DIR}/libminizincTargets.cmake")

# Export the package for use from the build-tree
# (this registers the build-tree with a global CMake-registry)
export(PACKAGE libminizinc)

# Create the libminizincConfig.cmake and libminizincConfigVersion files
# ... for the build tree
set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
configure_file(
  cmake/templates/libminizincConfig.cmake.in
  "${PROJECT_BINARY_DIR}/libminizincConfig.cmake"
  @ONLY
)
# ... for the install tree
set(CONF_INCLUDE_DIRS "\${libminizinc_CMAKE_DIR}/${REL_INCLUDE_DIR}")
configure_file(
  cmake/templates/libminizincConfig.cmake.in
  "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libminizincConfig.cmake"
  @ONLY
)
# ... for both
configure_file(
  cmake/templates/libminizincConfigVersion.cmake.in
  "${PROJECT_BINARY_DIR}/libminizincConfigVersion.cmake"
  @ONLY
)

# Install the libminizincConfig.cmake and libminizincConfigVersion.cmake
install(
  FILES "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libminizincConfig.cmake" "${PROJECT_BINARY_DIR}/libminizincConfigVersion.cmake"
  DESTINATION ${REL_CMAKE_DIR}
  COMPONENT dev
)

# Install the export set for use with the install-tree
install(
  EXPORT libminizincTargets
  DESTINATION ${REL_CMAKE_DIR}
  COMPONENT dev
)
