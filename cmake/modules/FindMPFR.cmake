### Try to find MPFR
# Once done this will define
#  MPFR_FOUND         - System has MPFR
#  MPFR_INCLUDE_DIRS  - The MPFR include directories
#  MPFR_LIBRARIES     - The libraries needed to use MPFR

find_path(MPFR_INCLUDE NAMES mpfr.h
          PATHS $ENV{GMPDIR} $ENV{MPFRDIR} ${INCLUDE_INSTALL_DIR})

find_library(MPFR_LIBRARY mpfr
             PATHS $ENV{GMPDIR} $ENV{MPFRDIR} ${LIB_INSTALL_DIR})

include(FindPackageHandleStandardArgs)
# handle the QUIETLY and REQUIRED arguments and set MPFR_FOUND to TRUE
# if all listed variables are TRUE
find_package_handle_standard_args(MPFR
  FOUND_VAR MPFR_FOUND
  REQUIRED_VARS MPFR_LIBRARY MPFR_INCLUDE
  FAIL_MESSAGE "Could NOT find MPFR, use MPFR_ROOT to hint its location"
)

mark_as_advanced(MPFR_INCLUDE MPFR_LIBRARY)

set(MPFR_INCLUDES ${MPFR_INCLUDE})
set(MPFR_LIBRARIES ${MPFR_LIBRARY})
