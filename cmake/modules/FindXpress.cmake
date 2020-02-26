# - Try to find FICO Xpress
# Once done this will define
#  XPRESS_FOUND        - System has FICO Xpress
#  XPRESS_INCLUDE_DIRS - The FICO Xpress include directories
# User can set Xpress_ROOT to the preferred installation prefix

#TODO: Check default installation locations
find_path(XPRESS_INCLUDE xprs.h
          PATHS $ENV{XPRESSDIR} $ENV{XPRESS} $ENV{XPRESS_DIR}
          HINTS /opt/xpressmp C:/xpressmp
          PATH_SUFFIXES include)

# handle the QUIETLY and REQUIRED arguments and set XPRESS_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Xpress
  FOUND_VAR XPRESS_FOUND
  REQUIRED_VARS XPRESS_INCLUDE
  FAIL_MESSAGE "Could NOT find Xpress, use Xpress_ROOT to hint its location"
)

mark_as_advanced(XPRESS_INCLUDE)

set(XPRESS_INCLUDE_DIRS ${XPRESS_INCLUDE})
