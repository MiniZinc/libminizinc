# - Try to find Gecode
# Once done this will define
#  GECODE_FOUND        - System has Gecode
#  GECODE_INCLUDE_DIRS - The Gecode include directories
#  GECODE_LIBRARIES    - The libraries needed to use Gecode
# User can set GECODE_ROOT to the preferred installation prefix

find_path(GECODE_INCLUDE gecode/kernel.hh
          HINTS ${GECODE_ROOT} $ENV{GECODE_ROOT}
          PATH_SUFFIXES include)

set(GECODE_REQ_LIBS gecodedriver gecodefloat gecodeint gecodekernel gecodeminimodel gecodesearch gecodeset gecodesupport)

foreach(GECODE_LIB ${GECODE_REQ_LIBS})
  # Determine windows library name
  file(GLOB_RECURSE GECODE_LIB_WIN RELATIVE "${GECODE_INCLUDE}/../lib" "${GECODE_LIB}*.lib")
  if(GECODE_LIB_WIN)
    get_filename_component(GECODE_LIB_WIN ${GECODE_LIB_WIN} NAME_WE)
  endif()

  # Try to find gecode library
  set(GECODE_LIB_LOC "GECODE_LIB_LOC-NOTFOUND")
  find_library(GECODE_LIB_LOC NAMES ${GECODE_LIB} lib${GECODE_LIB} ${GECODE_LIB_WIN}
               HINTS ${GECODE_ROOT} $ENV{GECODE_ROOT}
               PATH_SUFFIXES lib)
  if("${GECODE_LIB_LOC}" STREQUAL "GECODE_LIB_LOC-NOTFOUND")
    message(STATUS "Gecode: Could not find library `${GECODE_LIB}`")
    set(GECODE_LIBRARY "")
    break()
  endif()
  list(APPEND GECODE_LIBRARY ${GECODE_LIB_LOC})
endforeach(GECODE_LIB)

unset(GECODE_REQ_LIBS)
unset(GECODE_LIB_LOC)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GECODE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Gecode DEFAULT_MSG
                                  GECODE_INCLUDE GECODE_LIBRARY)

mark_as_advanced(GECODE_INCLUDE GECODE_LIBRARY)

set(GECODE_LIBRARIES ${GECODE_LIBRARY})
set(GECODE_INCLUDE_DIRS ${GECODE_INCLUDE})
