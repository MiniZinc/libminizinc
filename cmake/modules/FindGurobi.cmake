# - Try to find Gurobi
# Once done this will define
#  GUROBI_FOUND        - System has GUROBI
#  GUROBI_INCLUDE_DIRS - The GUROBI include directories
#  GUROBI_LIBRARIES    - The libraries needed to use GUROBI
#  GUROBI_COMPILE_FLAGS  - The definitions required to compile with GUROBI
# User can set Gurobi_ROOT to the preferred installation prefix

option(GUROBI_PLUGIN "Build Gurobi binding as a plugin" ON)

set(GUROBI_COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG")

set(GUROBI_VERSIONS 811 810 801 752 702)

foreach(VERSION ${GUROBI_VERSIONS})
  list(APPEND GUROBI_DEFAULT_LOC "/opt/gurobi${VERSION}/linux64")
  list(APPEND GUROBI_DEFAULT_LOC "/opt/gurobi${VERSION}/linux64")
  list(APPEND GUROBI_DEFAULT_LOC "C:\\gurobi${VERSION}\\win64")
  list(APPEND GUROBI_DEFAULT_LOC "C:\\gurobi${VERSION}\\win32")
  list(APPEND GUROBI_DEFAULT_LOC "/Library/gurobi${VERSION}/mac64")

  string(SUBSTRING ${VERSION} 0 2 VERSION)
  list(APPEND GUROBI_LIB_NAMES gurobi${VERSION})
endforeach(VERSION)

find_path(GUROBI_INCLUDE gurobi_c.h
          PATHS $ENV{GUROBI_HOME}
          HINTS ${GUROBI_DEFAULT_LOC}
          PATH_SUFFIXES include)

if(GUROBI_PLUGIN)
  include(CheckIncludeFiles)
  # TODO: Cleanup this mess
  check_include_files(dlfcn.h HAS_DLFCN_H)
  check_include_files(Windows.h HAS_WINDOWS_H)
  if(HAS_DLFCN_H)
    find_library(GUROBI_LIBRARY dl)
  elseif(HAS_WINDOWS_H)
    set(GUROBI_LIBRARY ${GUROBI_INCLUDE})
  endif()
else()
  foreach(GUROBI_LIB ${GUROBI_LIB_NAMES})
    find_library(GUROBI_LIBRARY NAMES ${GUROBI_LIB}
                 HINTS $ENV{GUROBI_HOME}
                 PATHS ${GUROBI_DEFAULT_LOC}
                 PATH_SUFFIXES lib)
    if(NOT "${GUROBI_LIBRARY}" STREQUAL "GUROBI_LIBRARY-NOTFOUND")
      break()
    endif()
  endforeach(GUROBI_LIB)
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CBC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Gurobi DEFAULT_MSG
                                  GUROBI_INCLUDE GUROBI_LIBRARY)

if(GUROBI_PLUGIN AND HAS_WINDOWS_H AND NOT HAS_DLFCN_H)
  unset(GUROBI_LIBRARY)
endif()

mark_as_advanced(GUROBI_INCLUDE GUROBI_LIBRARY)

set(GUROBI_LIBRARIES ${GUROBI_LIBRARY})
set(GUROBI_INCLUDE_DIRS ${GUROBI_INCLUDE})
