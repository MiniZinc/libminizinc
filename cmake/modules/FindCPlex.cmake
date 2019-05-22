# - Try to find CPLEX
# Once done this will define
#  CPLEX_FOUND        - System has CPLEX
#  CPLEX_INCLUDE_DIRS - The CPLEX include directories
#  CPLEX_LIBRARIES    - The libraries needed to use CPLEX
#  CPLEX_COMPILE_FLAGS  - The definitions required to compile with CPLEX
# User can set CPLEX_ROOT to the preferred installation prefix

set(CPLEX_COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG")

set(CPLEX_VERSIONS 129 128 1271 127 1263 1262 1261 126)

foreach(VERSION ${CPLEX_VERSIONS})
  list(APPEND CPLEX_DEFAULT_LOC "/opt/ibm/ILOG/CPLEX_Studio${VERSION}")
  list(APPEND CPLEX_DEFAULT_LOC "/opt/IBM/ILOG/CPLEX_Studio${VERSION}")
  list(APPEND CPLEX_DEFAULT_LOC "C:\\Program Files\\IBM\\ILOG\\CPLEX_Studio${VERSION}")
  list(APPEND CPLEX_DEFAULT_LOC "C:\\Program Files (x86)\\IBM\\ILOG\\CPLEX_Studio${VERSION}")
  list(APPEND CPLEX_DEFAULT_LOC "$ENV{HOME}/Applications/IBM/ILOG/CPLEX_Studio${VERSION}")
  list(APPEND CPLEX_DEFAULT_LOC "/Applications/IBM/ILOG/CPLEX_Studio${VERSION}")

  list(APPEND CPLEX_LIB_NAMES cplex${VERSION})
endforeach(VERSION)

find_path(CPLEX_INCLUDE ilcplex/cplex.h
          HINTS ${CPLEX_ROOT} ENV CPLEX_ROOT
          PATHS ${CPLEX_DEFAULT_LOC}
          PATH_SUFFIXES include cplex/include)

if(CPLEX_PLUGIN)
  include(CheckIncludeFiles)
  # TODO: Cleanup this mess
  check_include_files(dlfcn.h HAS_DLFCN_H)
  check_include_files(Windows.h HAS_WINDOWS_H)
  if(HAS_DLFCN_H)
    find_library(CPLEX_LIBRARY dl)
  elseif(HAS_WINDOWS_H)
    set(CPLEX_LIBRARY ${CPLEX_INCLUDE})
  endif()
else()
  foreach(CPLEX_LIB ${CPLEX_LIB_NAMES})
    find_library(CPLEX_LIBRARY NAMES cplex ${CPLEX_LIB}
                 HINTS ${CPLEX_ROOT} ENV CPLEX_ROOT
                 PATHS ${CPLEX_DEFAULT_LOC}
                 PATH_SUFFIXES lib/x86-64_linux/static_pic lib/x86-64_osx/static_pic lib/x64_windows_vs2013/stat_mda cplex/lib/x86-64_linux/static_pic cplex/lib/x86-64_osx/static_pic cplex/lib/x64_windows_vs2013/stat_mda)
    if(NOT "${CPLEX_LIBRARY}" STREQUAL "CPLEX_LIBRARY-NOTFOUND")
      break()
    endif()
  endforeach(CPLEX_LIB)
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CBC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(CPlex DEFAULT_MSG
                                  CPLEX_INCLUDE CPLEX_LIBRARY)

if(CPLEX_PLUGIN AND HAS_WINDOWS_H AND NOT HAS_DLFCN_H)
  unset(CPLEX_LIBRARY)
endif()

mark_as_advanced(CPLEX_INCLUDE CPLEX_LIBRARY)

set(CPLEX_LIBRARIES ${CPLEX_LIBRARY})
set(CPLEX_INCLUDE_DIRS ${CPLEX_INCLUDE})
