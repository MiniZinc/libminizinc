# - Try to find SCIP
# Once done this will define
#  SCIP_FOUND        - System has SCIP
#  SCIP_INCLUDE_DIRS - The SCIP include directories
# User can set SCIP_ROOT to the preferred installation prefix

# We only need headers, since we always compile SCIP as a plugin
find_path(SCIP_INCLUDE scip/scip.h
          PATH_SUFFIXES include)

# handle the QUIETLY and REQUIRED arguments and set SCIP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(SCIP
  FOUND_VAR SCIP_FOUND
  REQUIRED_VARS SCIP_INCLUDE
  FAIL_MESSAGE "Could NOT find SCIP, use SCIP_ROOT to hint its location"
)

mark_as_advanced(GUROBI_INCLUDE)

set(SCIP_INCLUDE_DIRS ${SCIP_INCLUDE})
