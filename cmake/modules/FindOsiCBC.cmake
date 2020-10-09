# - Try to find CBC
# Once done this will define
#  OSICBC_FOUND           - System has CBC
#  OSICBC_INCLUDE_DIRS    - The CBC include directories
#  OSICBC_LIBRARIES       - The libraries needed to use CBC
#  GOSICBC_TARGETS        - The names of imported targets created for CBC
# User can set OsiCBC_ROOT to the preferred installation prefix

set(OSICBC_FIND_FILES coin/CbcSolver.hpp coin/CglPreProcess.hpp coin/ClpConfig.h coin/CoinSignal.hpp coin/OsiClpSolverInterface.hpp coin/OsiSolverInterface.hpp)

foreach(OSICBC_FILE ${OSICBC_FIND_FILES})
  set(OSICBC_FILE_LOC "OSICBC_LIB_LOC-NOTFOUND")
  find_path(OSICBC_FILE_LOC ${OSICBC_FILE}
            PATH_SUFFIXES cbc cgl clp coinutils osi include)
  if("${OSICBC_FILE_LOC}" STREQUAL "OSICBC_FILE_LOC-NOTFOUND")
#    message(STATUS "OsiCBC: Could not find file `${OSICBC_FILE}`")
    set(OSICBC_INCLUDE "")
    break()
  endif()
  list(APPEND OSICBC_INCLUDE ${OSICBC_FILE_LOC})
  # Add "/coin" for CBC internal dependencies
  list(APPEND OSICBC_INCLUDE ${OSICBC_FILE_LOC}/coin)
endforeach(OSICBC_FILE)

list(REMOVE_DUPLICATES OSICBC_INCLUDE)
unset(OSICBC_FIND_FILES)
unset(OSICBC_FILE_LOC)

find_file(CBC_CONFIG_LOC NAMES coin/config_cbc_default.h coin/CbcConfig.h
  HINTS ${OSICBC_INCLUDE}
  PATH_SUFFIXES cbc)

if(NOT "${CBC_CONFIG_LOC}" STREQUAL "CBC_CONFIG_LOC-NOTFOUND")
  file(READ "${CBC_CONFIG_LOC}" CBC_CONFIG)
  string(REGEX MATCH "\#define +CBC_VERSION +\"([0-9]+.[0-9]+)\"" _ "${CBC_CONFIG}")
  set(OSICBC_VERSION "${CMAKE_MATCH_1}")
  unset(CBC_CONFIG)
endif()
unset(CBC_CONFIG_LOC)

if(WIN32 AND NOT UNIX)
  set(OSICBC_REQ_LIBS Osi OsiClp OsiCbc Clp Cgl Cbc CbcSolver CoinUtils)
else()
  set(OSICBC_REQ_LIBS CbcSolver Cbc Cgl OsiClp Clp Osi CoinUtils)
endif()

foreach(OSICBC_LIB ${OSICBC_REQ_LIBS})
  set(OSICBC_LIB_LOC "OSICBC_LIB_LOC-NOTFOUND")
  find_library(OSICBC_LIB_LOC NAMES ${OSICBC_LIB} lib${OSICBC_LIB}
               PATH_SUFFIXES lib)
  if("${OSICBC_LIB_LOC}" STREQUAL "OSICBC_LIB_LOC-NOTFOUND")
#    message(STATUS "OsiCBC: Could not find library `${OSICBC_LIB}`")
    set(OSICBC_LIBRARY "")
    break()
  endif()
  list(APPEND OSICBC_LIBRARY ${OSICBC_LIB_LOC})
  add_library(${OSICBC_LIB} UNKNOWN IMPORTED)
  set_target_properties(${OSICBC_LIB} PROPERTIES
                        IMPORTED_LOCATION ${OSICBC_LIB_LOC}
                        INTERFACE_INCLUDE_DIRECTORIES "${OSICBC_INCLUDE}")
  list(APPEND OSICBC_TARGETS ${OSICBC_LIB})
endforeach(OSICBC_LIB)

unset(OSICBC_REQ_LIBS)
unset(OSICBC_LIB_LOC)

if(UNIX AND NOT WIN32 AND NOT DEFINED EMSCRIPTEN)
  find_package(ZLIB)
  if(NOT ZLIB_FOUND)
    message(STATUS "OsiCBC: Missing dependency `Zlib`")
    set(OSICBC_LIBRARY "")
  else()
    list(APPEND OSICBC_LIBRARY ${ZLIB_LIBRARIES})
    list(APPEND OSICBC_TARGETS ${ZLIB_LIBRARIES})
  endif()
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CBC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OsiCBC
  FOUND_VAR OSICBC_FOUND
  REQUIRED_VARS OSICBC_INCLUDE OSICBC_LIBRARY
  VERSION_VAR OSICBC_VERSION
  FAIL_MESSAGE "Could NOT find OsiCBC, use OsiCBC_ROOT to hint its location"
)

mark_as_advanced(OSICBC_INCLUDE OSICBC_LIBRARY)

set(OSICBC_LIBRARIES ${OSICBC_LIBRARY})
set(OSICBC_INCLUDE_DIRS ${OSICBC_INCLUDE})
