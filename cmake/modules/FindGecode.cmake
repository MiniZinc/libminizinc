# - Try to find Gecode
# Once done this will define
#  GECODE_FOUND - System has Gecode
#  GECODE_INCLUDES - The Gecode include directories
#  GECODE_LIBRARIES - The libraries needed to use Gecode

find_path(GECODE_INCLUDE gecode/kernel.hh
          HINTS ${GECODE_HOME} ${GECODE_HOME}/include)

set(GECODE_REQ_LIBS gecodedriver gecodefloat gecodeint gecodekernel gecodeminimodel gecodesearch gecodeset gecodesupport)

foreach(GECODE_LIB ${GECODE_REQ_LIBS})
  set(GECODE_LIB_LOC "GECODE_LIB_LOC-NOTFOUND")
  find_library(GECODE_LIB_LOC NAMES ${GECODE_LIB} lib${GECODE_LIB}
               HINTS ${GECODE_HOME} ${GECODE_HOME}/lib)
  if ("${GECODE_LIB_LOC}" STREQUAL "GECODE_LIB_LOC-NOTFOUND")
    message(STATUS "Gecode: Could not find library `${GECODE_LIB}`")
    set(GECODE_LIBRARY "")
    break()
  endif()
  list(APPEND GECODE_LIBRARY ${GECODE_LIB_LOC})
endforeach(GECODE_LIB)

unset(GECODE_REQ_LIBS)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set GECODE_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Gecode DEFAULT_MSG
                                  GECODE_INCLUDE GECODE_LIBRARY)

mark_as_advanced(GECODE_INCLUDE GECODE_LIBRARY)

set(GECODE_LIBRARIES ${GECODE_LIBRARY})
set(GECODE_INCLUDES ${GECODE_INCLUDE})
