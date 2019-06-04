# - Try to find Gecode
# Once done this will define
#  GECODE_FOUND          - System has Gecode
#  GECODE_INCLUDE_DIRS   - The Gecode include directories
#  GECODE_LIBRARIES      - The libraries needed to use Gecode
#  GECODE_TARGETS        - The names of imported targets created for gecode
# User can set GECODE_ROOT to the preferred installation prefix

find_path(GECODE_INCLUDE gecode/kernel.hh
          PATHS ${GECODE_ROOT} ENV GECODE_ROOT
          PATH_SUFFIXES include)

find_file(GECODE_CONFIG_LOC gecode/support/config.hpp
          PATHS ${GECODE_ROOT} ENV GECODE_ROOT
          HINTS ${GECODE_INCLUDE}
          PATH_SUFFIXES include)

if(NOT "${GECODE_CONFIG_LOC}" STREQUAL "GECODE_CONFIG_LOC-NOTFOUND")
  file(READ "${GECODE_CONFIG_LOC}" GECODE_CONFIG)
  string(REGEX MATCH "\#define GECODE_VERSION \"([0-9]+.[0-9]+.[0-9]+)\"" _ "${GECODE_CONFIG}")
  set(GECODE_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "\#define GECODE_LIBRARY_VERSION \"([0-9]+-[0-9]+-[0-9]+)\"" _ "${GECODE_CONFIG}")
  set(GECODE_LIBRARY_VERSION "${CMAKE_MATCH_1}")
  string(REGEX MATCH "\#define GECODE_STATIC_LIBS ([0-9]+)" _ "${GECODE_CONFIG}")
  set(GECODE_STATIC_LIBS "${CMAKE_MATCH_1}")
  string(REGEX MATCH "\#define GECODE_HAS_GIST" GECODE_HAS_GIST "${GECODE_CONFIG}")
  string(REGEX MATCH "\#define GECODE_HAS_MPFR" GECODE_HAS_MPFR "${GECODE_CONFIG}")
endif()

set(GECODE_COMPONENTS Driver Flatzinc Float Int Kernel Minimodel Search Set Support)
if(GECODE_HAS_GIST)
  list(APPEND GECODE_COMPONENTS Gist)
endif()

foreach(GECODE_COMP ${GECODE_COMPONENTS})
  # Try to find gecode library
  string(TOLOWER "gecode${GECODE_COMP}" GECODE_LIB)
  set(GECODE_LIB_LOC "GECODE_LIB_LOC-NOTFOUND")
  find_library(GECODE_LIB_LOC NAMES ${GECODE_LIB} ${GECODE_LIB}-${GECODE_LIBRARY_VERSION}-r-x64 ${GECODE_LIB}-${GECODE_LIBRARY_VERSION}-d-x64
               PATHS ${GECODE_ROOT} ENV GECODE_ROOT
               PATH_SUFFIXES lib)
  if(NOT "${GECODE_LIB_LOC}" STREQUAL "GECODE_LIB_LOC-NOTFOUND")
      list(APPEND GECODE_LIBRARY ${GECODE_LIB_LOC})
      add_library(Gecode::${GECODE_COMP} UNKNOWN IMPORTED)
      set_target_properties(Gecode::${GECODE_COMP} PROPERTIES
                            IMPORTED_LOCATION ${GECODE_LIB_LOC}
                            INTERFACE_INCLUDE_DIRECTORIES ${GECODE_INCLUDE})
      set(Gecode_FIND_REQUIRED_${GECODE_COMP} TRUE)
      set(Gecode_${GECODE_COMP}_FOUND TRUE)
  endif()
endforeach(GECODE_COMP)

if(WIN32 AND GECODE_HAS_GIST AND GECODE_STATIC_LIBS)
  find_package(Qt5 QUIET COMPONENTS Core Gui Widgets PrintSupport)
  set_target_properties(Gecode::Gist PROPERTIES
                        INTERFACE_LINK_LIBRARIES "Qt5::Core;Qt5::Gui;Qt5::Widgets;Qt5::PrintSupport")
endif()

unset(GECODE_REQ_LIBS)
unset(GECODE_LIB_WIN)
unset(GECODE_LIB_LOC)

if(GECODE_LIBRARY AND GECODE_HAS_MPFR)
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
  HANDLE_COMPONENTS
)

mark_as_advanced(GECODE_INCLUDE GECODE_LIBRARY)

set(GECODE_LIBRARIES ${GECODE_LIBRARY})
set(GECODE_INCLUDE_DIRS ${GECODE_INCLUDE})
