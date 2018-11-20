# - Try to find CBC
# Once done this will define
#  OSICBC_FOUND - System has CBC
#  OSICBC_INCLUDES - The CBC include directories
#  OSICBC_LIBRARIES - The libraries needed to use CBC

find_path(OSICBC_INCLUDE coin/CbcSolver.hpp
          HINTS ${OSICBC_HOME} ${OSICBC_HOME}/include ${OSICBC_INCLUDEDIR})

if(WIN32 AND NOT UNIX)
  set(OSICBC_REQ_LIBS libOsi libOsiClp libOsiCbc libClp libCgl libCbc libCbcSolver libCoinUtils)
else()
  set(OSICBC_REQ_LIBS CbcSolver Cbc Cgl OsiClp Clp Osi CoinUtils) #TODO: Handle ZLib
endif()

foreach(OSICBC_LIB ${OSICBC_REQ_LIBS})
  set(OSICBC_LIB_LOC "OSICBC_LIB_LOC-NOTFOUND")
  find_library(OSICBC_LIB_LOC NAMES ${OSICBC_LIB} lib${OSICBC_LIB}
               HINTS ${OSICBC_HOME} ${OSICBC_HOME}/lib)
  if("${OSICBC_LIB_LOC}" STREQUAL "OSICBC_LIB_LOC-NOTFOUND")
    message(STATUS "OsiCBC: Could not find library `${OSICBC_LIB}`")
    set(OSICBC_LIBRARY "")
    break()
  endif()
  list(APPEND OSICBC_LIBRARY ${OSICBC_LIB_LOC})
endforeach(OSICBC_LIB)

unset(OSICBC_REQ_LIBS)

if(UNIX AND NOT WIN32)
  find_package(ZLIB)
  if(NOT ZLIB_FOUND)
    message(STATUS "OsiCBC: Missing dependency `Zlib`")
    set(OSICBC_LIBRARY "")
  else()
    list(APPEND OSICBC_LIBRARY ${ZLIB_LIBRARIES})
  endif()
endif()

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set CBC_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(OSICBC DEFAULT_MSG
                                  OSICBC_INCLUDE OSICBC_LIBRARY)

mark_as_advanced(OSICBC_INCLUDE OSICBC_LIBRARY)

set(OSICBC_LIBRARIES ${OSICBC_LIBRARY})
set(OSICBC_INCLUDES ${OSICBC_INCLUDE})
