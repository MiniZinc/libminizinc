# - Try to find Gecode
# Once done this will define
#  GECODE_FOUND          - System has Gecode
#  GECODE_INCLUDE_DIRS   - The Gecode include directories
#  GECODE_LIBRARIES      - The libraries needed to use Gecode
#  GECODE_TARGETS        - The names of imported targets created for gecode
# User can set GECODE_ROOT to the preferred installation prefix

find_path(GECODE_INCLUDE gecode/kernel.hh
          HINTS ${GECODE_ROOT} ENV GECODE_ROOT
          PATH_SUFFIXES include)

if(NOT "${GECODE_INCLUDE}" STREQUAL "GECODE_INCLUDE-NOTFOUND")
  file(READ ${GECODE_INCLUDE}/gecode/support/config.hpp GECODE_CONFIG)
  string(REGEX MATCH "\#define GECODE_VERSION \"([0-9]+.[0-9]+.[0-9]+)\"" _ ${GECODE_CONFIG})
  set(GECODE_VERSION ${CMAKE_MATCH_1})
  string(REGEX MATCH "\#define GECODE_LIBRARY_VERSION \"([0-9]+-[0-9]+-[0-9]+)\"" _ ${GECODE_CONFIG})
  set(GECODE_LIBRARY_VERSION ${CMAKE_MATCH_1})
  string(REGEX MATCH "\#define GECODE_STATIC_LIBS ([0-9]+)" _ ${GECODE_CONFIG})
  set(GECODE_STATIC_LIBS ${CMAKE_MATCH_1})
  string(REGEX MATCH "\#define GECODE_HAS_GIST" GECODE_HAS_GIST ${GECODE_CONFIG})
  string(REGEX MATCH "\#define GECODE_HAS_MPFR" GECODE_HAS_MPFR ${GECODE_CONFIG})
endif()

set(GECODE_REQ_LIBS gecodedriver gecodefloat gecodeint gecodekernel gecodeminimodel gecodesearch gecodeset gecodesupport)
if(GECODE_HAS_GIST)
  list(APPEND GECODE_REQ_LIBS gecodegist)
endif()

foreach(GECODE_LIB ${GECODE_REQ_LIBS})
  # Try to find gecode library
  set(GECODE_LIB_LOC "GECODE_LIB_LOC-NOTFOUND")
  find_library(GECODE_LIB_LOC NAMES ${GECODE_LIB} ${GECODE_LIB}-${GECODE_LIBRARY_VERSION}-r-x64 ${GECODE_LIB}-${GECODE_LIBRARY_VERSION}-d-x64
               HINTS ${GECODE_ROOT} ENV GECODE_ROOT
               PATH_SUFFIXES lib)
  if("${GECODE_LIB_LOC}" STREQUAL "GECODE_LIB_LOC-NOTFOUND")
#    message(STATUS "Gecode: Could not find library `${GECODE_LIB}`")
    set(GECODE_LIBRARY "")
    break()
  endif()
  list(APPEND GECODE_LIBRARY ${GECODE_LIB_LOC})
  if(GECODE_STATIC_LIBS OR NOT WIN32)
    add_library(${GECODE_LIB} UNKNOWN IMPORTED)
    set_target_properties(${GECODE_LIB} PROPERTIES
                          IMPORTED_LOCATION ${GECODE_LIB_LOC}
                          INTERFACE_INCLUDE_DIRECTORIES ${GECODE_INCLUDE})
    list(APPEND GECODE_TARGETS ${GECODE_LIB})
  endif()
endforeach(GECODE_LIB)

if(WIN32 AND NOT GECODE_STATIC_LIBS)
  get_filename_component(GECODE_LIB_WIN ${GECODE_LIB_LOC} DIRECTORY)
  link_directories(${GECODE_LIB_WIN})
endif()

unset(GECODE_REQ_LIBS)
unset(GECODE_LIB_WIN)
unset(GECODE_LIB_LOC)

if(GECODE_HAS_MPFR)
  find_package(MPFR)
  list(APPEND GECODE_LIBRARY ${MPFR_LIBRARIES})
  list(APPEND GECODE_TARGETS ${MPFR_LIBRARIES})
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GECODE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(
  Gecode
  REQUIRED_VARS GECODE_INCLUDE GECODE_LIBRARY
  VERSION_VAR GECODE_VERSION
)

mark_as_advanced(GECODE_INCLUDE GECODE_LIBRARY)

set(GECODE_LIBRARIES ${GECODE_LIBRARY})
set(GECODE_INCLUDE_DIRS ${GECODE_INCLUDE})
