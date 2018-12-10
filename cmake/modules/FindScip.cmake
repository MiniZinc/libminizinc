# - Try to find SCIP
# Once done this will define
#  SCIP_FOUND        - System has Gecode
#  SCIP_INCLUDE_DIRS - The Gecode include directories
#  SCIP_LIBRARIES    - The libraries needed to use Gecode
# User can set SCIP_ROOT, SOPLEX_ROOT, ZIMPL_ROOT to the preferred installation prefixes

# TODO: Add default installation locations

find_path(SCIP_INCLUDE scip/scip.h
          HINTS ${SCIP_ROOT} $ENV{SCIP_ROOT}
          PATH_SUFFIXES include)

find_library(SCIP_LIBRARY scip
          HINTS ${SCIP_ROOT} $ENV{SCIP_ROOT}
          PATH_SUFFIXES lib)

find_path(SOPLEX_INCLUDE soplex.h
          HINTS ${SOPLEX_ROOT} $ENV{SOPLEX_ROOT} ${SCIP_ROOT} $ENV{SCIP_ROOT}
          PATH_SUFFIXES include)

find_library(SOPLEX_LIBRARY soplex
             HINTS ${SOPLEX_ROOT} $ENV{SOPLEX_ROOT} ${SCIP_ROOT} $ENV{SCIP_ROOT}
             PATH_SUFFIXES lib)

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set SCIP_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(Scip DEFAULT_MSG
                                  SCIP_INCLUDE SCIP_LIBRARY SOPLEX_INCLUDE SOPLEX_LIBRARY)

mark_as_advanced(SCIP_INCLUDE SCIP_LIBRARY SOPLEX_INCLUDE SOPLEX_LIBRARY)

set(SCIP_LIBRARIES ${SCIP_LIBRARY} ${SOPLEX_LIBRARY})
set(SCIP_INCLUDE_DIRS ${SCIP_INCLUDE} ${SOPLEX_INCLUDE})
list(REMOVE_DUPLICATES SCIP_INCLUDE_DIRS)