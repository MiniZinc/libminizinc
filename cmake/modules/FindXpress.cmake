# - Try to find FICO Xpress
# Once done this will define
#  XPRESS_FOUND        - System has FICO Xpress
#  XPRESS_INCLUDE_DIRS - The FICO Xpress include directories
#  XPRESS_LIBRARIES    - The libraries needed to use FICO Xpress
# User can set XPRESS_ROOT to the preferred installation prefix


#TODO: Check default installation locations
find_path(XPRESS_INCLUDE xprs.h
          HINTS ${XPRESS_ROOT} ENV XPRESS_ROOT
          PATH /opt/xpressmp ENV XPRESS_DIR
          PATH_SUFFIXES include)

foreach(XPRESS_LIB xprb xprs)
  set(XPRESS_LIB_LOC "XPRESS_LIB_LOC-NOTFOUND")
  find_library(XPRESS_LIB_LOC NAMES ${XPRESS_LIB} lib${XPRESS_LIB}
               HINTS ${XPRESS_ROOT} ENV XPRESS_ROOT
               PATH /opt/xpressmp ENV XPRESS_DIR
               PATH_SUFFIXES lib)
  if("${XPRESS_LIB_LOC}" STREQUAL "XPRESS_LIB_LOC-NOTFOUND")
#    message(STATUS "FICO Xpres: Could not find library `${XPRESS_LIB}`")
    set(XPRESS_LIBRARY "")
    break()
  endif()
  list(APPEND XPRESS_LIBRARY ${XPRESS_LIB_LOC})
endforeach(XPRESS_LIB)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XPRESS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Xpress
  FOUND_VAR XPRESS_FOUND
  REQUIRED_VARS XPRESS_INCLUDE XPRESS_LIBRARY
  FAIL_MESSAGE "Could NOT find Xpress, use XPRESS_ROOT to hint its location"
)

mark_as_advanced(XPRESS_INCLUDE XPRESS_LIBRARY)

set(XPRESS_LIBRARIES ${XPRESS_LIBRARY})
set(XPRESS_INCLUDE_DIRS ${XPRESS_INCLUDE})
