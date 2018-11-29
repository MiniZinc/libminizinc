cmake_minimum_required (VERSION 3.4.0)

project (libminizinc CXX C)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug"
    "Release"
    "MinSizeRel"
    "RelWithDebInfo"
  )
endif()

# -------------------------------------------------------------------------------------------------------------------
# -- The below checks if certain solver paths are set in the config call for the corresp. interface to be compiled --
# -- For example, to include gecode in the minizinc binary run                                                     --
# --     "cd build; cmake -D GECODE_HOME=$GECODE_HOME ..; cmake --build ."                                         --
# -------------------------------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------------------------------
# The version number.
set (libminizinc_VERSION_MAJOR 2)
set (libminizinc_VERSION_MINOR 2)
set (libminizinc_VERSION_PATCH 3)
set (libminizinc_VERSION
  ${libminizinc_VERSION_MAJOR}.${libminizinc_VERSION_MINOR}.${libminizinc_VERSION_PATCH})

if (ADDITIONAL_DATE_STRING)
  set (libminizinc_VERSION_PATCH "${libminizinc_VERSION_PATCH}.${ADDITIONAL_DATE_STRING}")
endif()

if (NOT BUILD_REF)
  set(BUILD_REF "")
endif ()

# Offer the user the choice of overriding the installation directories
set(INSTALL_LIB_DIR lib CACHE PATH "Installation directory for libraries")
set(INSTALL_BIN_DIR bin CACHE PATH "Installation directory for executables")
set(INSTALL_INCLUDE_DIR include CACHE PATH
  "Installation directory for header files")
if(WIN32 AND NOT CYGWIN)
  set(DEF_INSTALL_CMAKE_DIR CMake)
else()
  set(DEF_INSTALL_CMAKE_DIR lib/cmake)
endif()
set(INSTALL_CMAKE_DIR ${DEF_INSTALL_CMAKE_DIR} CACHE PATH
  "Installation directory for CMake files")

# Make relative paths absolute (needed later on)
foreach(p LIB BIN INCLUDE CMAKE)
  set(var INSTALL_${p}_DIR)
  if(NOT IS_ABSOLUTE "${${var}}")
    set(${var} "${CMAKE_INSTALL_PREFIX}/${${var}}")
  endif()
endforeach()

# -------------------------------------------------------------------------------------------------------------------
option (USE_ADDRESS_SANITIZER "Use GCC Address Sanitizer" OFF)
if (USE_ADDRESS_SANITIZER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

# -------------------------------------------------------------------------------------------------------------------
option (BUILD_STATIC_EXECUTABLE "Build statically linked executables" OFF)  ## does not work TODO
if (BUILD_STATIC_EXECUTABLE)
#  SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#  SET(BUILD_SHARED_LIBRARIES OFF)
  SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -static")
endif()

# -------------------------------------------------------------------------------------------------------------------

if(APPLE)
  execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE OSX_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CMAKE_OSX_SYSROOT ${OSX_SYSROOT})
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif(APPLE)

include(CheckCXXCompilerFlag)

SET(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
check_cxx_compiler_flag(-Werror HAS_WERROR)

if (HAS_WERROR)
  SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
endif()

set (CMAKE_CXX_STANDARD 11)

# -------------------------------------------------------------------------------------------------------------------
CHECK_CXX_SOURCE_COMPILES("int main(void) { static __thread int x; (void)x; return 0;}" HAS_ATTR_THREAD)

if (NOT HAS_ATTR_THREAD)
  CHECK_CXX_SOURCE_COMPILES("int main(void) { __declspec(thread) static int x; (void)x; return 0;}" HAS_DECLSPEC_THREAD)
endif()

CHECK_CXX_SOURCE_COMPILES("#include <cstdlib>
int main(void) { long long int x = atoll(\"123\"); (void)x; }" HAS_ATOLL)
CHECK_CXX_SOURCE_COMPILES("
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libproc.h>
#include <unistd.h>

int main (int argc, char* argv[])
{
    pid_t pid = getpid();
    char path[PROC_PIDPATHINFO_MAXSIZE];
    (void) proc_pidpath (pid, path, sizeof(path));
    return 0;
}
" HAS_PIDPATH)

CHECK_CXX_SOURCE_COMPILES("
#include <windows.h>
int main (int argc, char* argv[]) {
  char path[MAX_PATH];
  (void) GetModuleFileName(NULL, path, MAX_PATH);
  return 0;
}" HAS_GETMODULEFILENAME)

CHECK_CXX_SOURCE_COMPILES("
#include <windows.h>
int main (int argc, char* argv[]) {
  (void) GetFileAttributes(NULL);
  return 0;
}" HAS_GETFILEATTRIBUTES)

CHECK_CXX_SOURCE_COMPILES("
#include <string.h>
int main (int argc, char* argv[]) {
  (void) memcpy_s(NULL,0,NULL,0);
  return 0;
}" HAS_MEMCPY_S)

#check_cxx_source_compiles("#include <sstream>
##include <iomanip>
#int main(void) { std::ostringstream oss; std::hexfloat(oss); return 0; }" HAS_HEXFLOAT)

# -------------------------------------------------------------------------------------------------------------------
SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/minizinc)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc/html)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc/pdf)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  ${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in
  ${PROJECT_BINARY_DIR}/include/minizinc/config.hh
  )

configure_file (
  ${PROJECT_SOURCE_DIR}/doc/spec/version.tex.in
  ${PROJECT_BINARY_DIR}/doc/pdf/version.tex
  )

include_directories(${PROJECT_BINARY_DIR}/include)

include_directories(${PROJECT_SOURCE_DIR}/include)

# -------------------------------------------------------------------------------------------------------------------
include(cmake/targets/libminizinc.cmake)
# -------------------------------------------------------------------------------------------------------------------
find_package(Threads REQUIRED)

# -------------------------------------------------------------------------------------------------------------------

include(cmake/targets/libminizinc_cplex.cmake)
include(cmake/targets/libminizinc_gecode.cmake)
include(cmake/targets/libminizinc_gurobi.cmake)
include(cmake/targets/libminizinc_osicbc.cmake)
include(cmake/targets/libminizinc_xpress.cmake)

include(cmake/targets/libminizinc_fzn.cmake)

include(cmake/targets/libminizinc_solver.cmake)

include(cmake/targets/minizinc.cmake)
include(cmake/targets/mzn2doc.cmake)

INSTALL(
  TARGETS minizinc minizinc_solver minizinc_fzn minizinc-bin mzn2fzn solns2out mzn2doc
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# -------------------------------------------------------------------------------------------------------------------

option (BUILD_HTML_DOCUMENTATION "Build HTML documentation for the MiniZinc library" OFF)

if (BUILD_HTML_DOCUMENTATION)
  add_custom_target(doc ALL
    DEPENDS "mzn2doc"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND mzn2doc --toplevel-groups 2
    --include-stdlib
    --html-header ${PROJECT_SOURCE_DIR}/doc/html/header.html
    --html-footer ${PROJECT_SOURCE_DIR}/doc/html/footer.html
      --output-base ${PROJECT_BINARY_DIR}/doc/html/doc share/minizinc/std/globals.mzn
  )
  FILE(COPY
       ${PROJECT_SOURCE_DIR}/doc/html/style.css
       ${PROJECT_SOURCE_DIR}/doc/html/header.html
       ${PROJECT_SOURCE_DIR}/doc/html/footer.html
       ${PROJECT_SOURCE_DIR}/doc/html/MiniZn_logo.jpg
       DESTINATION ${PROJECT_BINARY_DIR}/doc/html/)
  INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/doc/html DESTINATION doc)
endif()

option (BUILD_HTML_DOCUMENTATION_JEKYLL "Build HTML documentation for the MiniZinc library" OFF)

if (BUILD_HTML_DOCUMENTATION_JEKYLL)
  add_custom_target(doc ALL
    DEPENDS "mzn2doc"
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
    COMMAND mzn2doc --toplevel-groups 2
    --include-stdlib
    --html-header ${PROJECT_SOURCE_DIR}/doc/html/header_jekyll.html
    --html-footer ${PROJECT_SOURCE_DIR}/doc/html/footer_jekyll.html
      --output-base ${PROJECT_BINARY_DIR}/doc/html/doc share/minizinc/std/globals.mzn
  )
  INSTALL(DIRECTORY ${PROJECT_BINARY_DIR}/doc/html DESTINATION doc)
endif()

option (BUILD_PDF_DOCUMENTATION "Build PDF documentation for the MiniZinc language" OFF)

if (BUILD_PDF_DOCUMENTATION)
  add_custom_target(doc_spec ALL
    DEPENDS doc/spec/common-spec.tex doc/spec/minizinc-spec.tex
    WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/doc/spec
    COMMAND latexmk -pdf -output-directory=${PROJECT_BINARY_DIR}/doc/pdf minizinc-spec
  )
endif()

INSTALL(DIRECTORY share/minizinc DESTINATION share)
INSTALL(DIRECTORY include/minizinc
        DESTINATION include
        PATTERN config.hh.in EXCLUDE
)

# -------------------------------------------------------------------------------------------------------------------
INSTALL(FILES ${PROJECT_BINARY_DIR}/include/minizinc/config.hh DESTINATION include/minizinc)
INSTALL(FILES README.txt INSTALL.txt LICENSE.txt DESTINATION doc/..)
INSTALL(DIRECTORY doc/pdf DESTINATION doc)
INSTALL(DIRECTORY share/examples DESTINATION share)
INSTALL(DIRECTORY lib/cached/minizinc DESTINATION include)

  # Add all targets to the build-tree export set
  export(TARGETS minizinc ${EXTRA_TARGETS}
    FILE "${PROJECT_BINARY_DIR}/libminizincTargets.cmake")

  # Export the package for use from the build-tree
  # (this registers the build-tree with a global CMake-registry)
  export(PACKAGE libminizinc)

  # Create the libminizincConfig.cmake and libminizincConfigVersion files¬

  file(RELATIVE_PATH REL_CMAKE_DIR "${CMAKE_INSTALL_PREFIX}"
     "${INSTALL_CMAKE_DIR}")
  file(RELATIVE_PATH REL_INCLUDE_DIR "${INSTALL_CMAKE_DIR}"
     "${INSTALL_INCLUDE_DIR}")
  # ... for the build tree
  set(CONF_INCLUDE_DIRS "${PROJECT_SOURCE_DIR}" "${PROJECT_BINARY_DIR}")
  configure_file(libminizincConfig.cmake.in
    "${PROJECT_BINARY_DIR}/libminizincConfig.cmake" @ONLY)
  # ... for the install tree
  set(CONF_INCLUDE_DIRS "\${libminizinc_CMAKE_DIR}/${REL_INCLUDE_DIR}")
  configure_file(libminizincConfig.cmake.in
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libminizincConfig.cmake" @ONLY)
  # ... for both
  configure_file(libminizincConfigVersion.cmake.in
    "${PROJECT_BINARY_DIR}/libminizincConfigVersion.cmake" @ONLY)

  # Install the libminizincConfig.cmake and libminizincConfigVersion.cmake
  INSTALL(FILES
    "${PROJECT_BINARY_DIR}${CMAKE_FILES_DIRECTORY}/libminizincConfig.cmake"
    "${PROJECT_BINARY_DIR}/libminizincConfigVersion.cmake"
    DESTINATION ${REL_CMAKE_DIR} COMPONENT dev)

  # Install the export set for use with the install-tree

  INSTALL(EXPORT libminizincTargets DESTINATION
    ${REL_CMAKE_DIR} COMPONENT dev)
# -------------------------------------------------------------------------------------------------------------------
SET(CPACK_PACKAGE_VERSION_MAJOR ${libminizinc_VERSION_MAJOR})
SET(CPACK_PACKAGE_VERSION_MINOR ${libminizinc_VERSION_MINOR})
SET(CPACK_PACKAGE_VERSION_PATCH ${libminizinc_VERSION_PATCH})
SET(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
IF(WIN32 AND NOT UNIX)
  SET(CPACK_NSIS_DISPLAY_NAME "${CPACK_PACKAGE_INSTALL_DIRECTORY} My Famous Project")
  SET(CPACK_NSIS_URL_INFO_ABOUT "http:\\\\\\\\www.minizinc.org")
  SET(CPACK_NSIS_CONTACT "info@minizinc.org")
  SET(CPACK_NSIS_MODIFY_PATH ON)
  SET(CPACK_GENERATOR "ZIP")
ELSE(WIN32 AND NOT UNIX)
  SET(CPACK_STRIP_FILES "bin/MyExecutable")
  SET(CPACK_SOURCE_STRIP_FILES "")
  SET(CPACK_GENERATOR "TGZ")
ENDIF(WIN32 AND NOT UNIX)

set(CPACK_SOURCE_GENERATOR "TGZ;ZIP")
set(CPACK_SOURCE_PACKAGE_FILE_NAME
  "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}")
set(CPACK_SOURCE_IGNORE_FILES
  "${CMAKE_CURRENT_BINARY_DIR}/;/tests/;/.git/;/.gitignore;~$;${CPACK_SOURCE_IGNORE_FILES}")

include(CPack)
