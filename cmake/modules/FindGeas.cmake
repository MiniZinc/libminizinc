# - Try to find Geas
# Once done this will define
#  GEAS_FOUND        - System has FICO Xpress
#  GEAS_INCLUDE_DIRS - The FICO Xpress include directories
#  GEAS_LIBRARIES    - The libraries needed to use FICO Xpress
# User can set GEAS_ROOT to the preferred installation prefix
# Imported target Geas will be created for linking purposes

list(INSERT CMAKE_PREFIX_PATH 0 "${GEAS_ROOT}" "$ENV{GEAS_ROOT}")

find_path(
  GEAS_INCLUDE geas/c/geas.h
  PATH_SUFFIXES include
)

find_library(
  GEAS_LIBRARY NAMES geas libgeas
  PATH_SUFFIXES lib
)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set XPRESS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Geas
  FOUND_VAR GEAS_FOUND
  REQUIRED_VARS GEAS_INCLUDE GEAS_LIBRARY
  FAIL_MESSAGE "Could NOT find Geas, use GEAS_ROOT to hint its location"
)

mark_as_advanced(GEAS_INCLUDE GEAS_LIBRARY)
list(REMOVE_AT CMAKE_PREFIX_PATH 1 0)

if(GEAS_FOUND)
  add_library(Geas UNKNOWN IMPORTED)
  set_target_properties(Geas PROPERTIES
    IMPORTED_LOCATION ${GEAS_LIBRARY}
    INTERFACE_INCLUDE_DIRECTORIES ${GEAS_INCLUDE}
  )
endif()

set(GEAS_LIBRARIES ${GEAS_LIBRARY})
set(GEAS_INCLUDE_DIRS ${GEAS_INCLUDE})
