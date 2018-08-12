cmake_minimum_required (VERSION 3.0.0)

project (libminizinc CXX C)
set(CMAKE_MODULE_PATH
  ${CMAKE_MODULE_PATH}
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
)

# -------------------------------------------------------------------------------------------------------------------
# -- The below checks if certain solver paths are set in the config call for the corresp. interface to be compiled --
# -- For example, to include gecode in the minizinc binary run                                                     --
# --     "cd build; cmake -D GECODE_HOME=$GECODE_HOME ..; cmake --build ."                                         --
# -------------------------------------------------------------------------------------------------------------------
if (DEFINED GECODE_HOME AND NOT "${GECODE_HOME} " STREQUAL " ")
  set(HAS_GECODE TRUE)
endif()
if (DEFINED CPLEX_STUDIO_DIR AND NOT "${CPLEX_STUDIO_DIR} " STREQUAL " ")
  set(HAS_CPLEX TRUE)
endif()
if(DEFINED GUROBI_HOME AND NOT "${GUROBI_HOME} " STREQUAL " ")
  set(HAS_GUROBI TRUE)
endif()
if(DEFINED XPRESS_HOME AND NOT "${XPRESS_HOME} " STREQUAL " ")
  set(HAS_XPRESS TRUE)
endif()
if (DEFINED SCIP_DIR AND DEFINED SOPLEX_DIR AND DEFINED ZIMPL_DIR AND NOT "${SCIP_DIR} " STREQUAL " ")
  set(HAS_SCIP TRUE)                     # Can use SCIP w/o ZIMPL too
endif()
if (DEFINED OSICBC_HOME AND NOT "${OSICBC_HOME} " STREQUAL " ")
  set(HAS_OSICBC TRUE)
endif()

# -------------------------------------------------------------------------------------------------------------------
# The version number.
set (libminizinc_VERSION_MAJOR 2)
set (libminizinc_VERSION_MINOR 2)
set (libminizinc_VERSION_PATCH 0)
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

include (CheckIncludeFiles)

# -------------------------------------------------------------------------------------------------------------------
option (BUILD_GUROBI_PLUGIN "Build Gurobi binding as a plugin" OFF)
option (BUILD_CPLEX_PLUGIN "Build CPLEX binding as a plugin" OFF)
if (BUILD_GUROBI_PLUGIN OR BUILD_CPLEX_PLUGIN OR HAS_CPLEX)
  if(HAS_GUROBI OR HAS_CPLEX)
    CHECK_INCLUDE_FILES (dlfcn.h HAS_DLFCN_H)
    if(HAS_DLFCN_H)
      find_library (NEEDS_LIBDL NAMES dl)
    endif()
    CHECK_INCLUDE_FILES (Windows.h HAS_WINDOWS_H)
    if(HAS_DLFCN_H OR HAS_WINDOWS_H)
      if (BUILD_GUROBI_PLUGIN)
        set(HAS_GUROBI_PLUGIN TRUE)
      endif()
      if (BUILD_CPLEX_PLUGIN)
        set(HAS_CPLEX_PLUGIN TRUE)
      endif()
    endif()
  endif()
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

if ( (CMAKE_VERSION VERSION_GREATER 3.1.0) OR (CMAKE_VERSION VERSION_EQUAL 3.1.0) )
  set (CMAKE_CXX_STANDARD 11)
endif()
check_cxx_compiler_flag(-std=c++11 HAS_STDCPP11)
if (HAS_STDCPP11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  check_cxx_compiler_flag(-std=c++0x HAS_STDCPP0X)
  if (HAS_STDCPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  else()
    message("Warning: Initial check for C++11 unsuccessful. This is fine if HAS_CPP11 test below succeeds.")
  endif()
endif()

# -------------------------------------------------------------------------------------------------------------------
if (USE_STDLIBCPP)
  set(MZN_LIBCPP libstdc++)
  set(SAFE_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=${MZN_LIBCPP}")
  check_cxx_source_compiles("int main(void) {return 0;}" LINKS_STDLIBLIBCPP)
  if (NOT LINKS_STDLIBLIBCPP)
    set(CMAKE_CXX_FLAGS "${SAFE_CMAKE_CXX_FLAGS}")
  endif()
endif()

check_cxx_source_compiles("#include <unordered_set>
int main(void) { std::unordered_set<int> x; return 0; }" HAS_CPP11)

if (NOT HAS_CPP11)
  message(FATAL_ERROR "A c++11 compatible C++ standard library is required to build libminizinc.")
endif()

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

add_custom_target(MZNParser echo "Creating parser")

# When updating the cached files, update MD5 sums defined in this file
include(${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake)

macro(MD5 filename md5sum)
  file(READ "${filename}" RAW_MD5_FILE)
  string(REGEX REPLACE "\r" "" STRIPPED_MD5_FILE "${RAW_MD5_FILE}")
  string(MD5 ${md5sum} "${STRIPPED_MD5_FILE}")
endmacro(MD5)

find_package(BISON)
if(BISON_FOUND)
  BISON_TARGET(MZNParser
    ${PROJECT_SOURCE_DIR}/lib/parser.yxx
    ${PROJECT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${PROJECT_BINARY_DIR}/include/minizinc/parser.tab.hh
    COMPILE_FLAGS "-p mzn_yy -l"
  )

  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/minizinc/support/regex/)
  BISON_TARGET(RegExParser
    ${PROJECT_SOURCE_DIR}/lib/support/regex/parser.yxx
    ${PROJECT_BINARY_DIR}/regex_parser.tab.cpp
    DEFINES_FILE ${PROJECT_BINARY_DIR}/include/minizinc/support/regex/parser.tab.hh
    COMPILE_FLAGS "-l"
  )
else()
  MD5(${PROJECT_SOURCE_DIR}/lib/parser.yxx parser_yxx_md5)
  if(NOT "${parser_yxx_md5}" STREQUAL "${parser_yxx_md5_cached}")
    message(FATAL_ERROR "The file parser.yxx has been modified but bison cannot be run."
	  "\nIf you are sure parser.tab.cpp and minizinc/parser.tab.hh in ${PROJECT_SOURCE_DIR}/lib/cached/ are correct then"
	  "copy parser.yxx's md5 ${parser_yxx_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake")
  endif()
  include_directories(${PROJECT_SOURCE_DIR}/lib/cached)
  set(BISON_MZNParser_OUTPUTS
    ${PROJECT_SOURCE_DIR}/lib/cached/parser.tab.cpp
    ${PROJECT_SOURCE_DIR}/lib/cached/minizinc/parser.tab.hh
  )

  # TODO: Remove support or add RegEx
endif()

find_package(FLEX)
if(FLEX_FOUND)
  FLEX_TARGET(MZNLexer
    ${PROJECT_SOURCE_DIR}/lib/lexer.lxx
    ${PROJECT_BINARY_DIR}/lexer.yy.cpp
    COMPILE_FLAGS "-P mzn_yy -L"
  )
  ADD_FLEX_BISON_DEPENDENCY(MZNLexer MZNParser)

  FLEX_TARGET(RegExLexer
    ${PROJECT_SOURCE_DIR}/lib/support/regex/lexer.lxx
    ${PROJECT_BINARY_DIR}/regex_lexer.yy.cpp
    COMPILE_FLAGS "-L"
  )
  ADD_FLEX_BISON_DEPENDENCY(RegExLexer RegExParser)
else()
  MD5(${PROJECT_SOURCE_DIR}/lib/lexer.lxx lexer_lxx_md5)
  if(NOT "${lexer_lxx_md5}" STREQUAL "${lexer_lxx_md5_cached}")
    message(FATAL_ERROR "The file lexer.lxx has been modified but flex cannot be run."
	  "\nIf you are sure ${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp is correct then"
	  "copy lexer.lxx's md5 md5 ${lexer_lxx_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake")
  endif()
  set(FLEX_MZNLexer_OUTPUTS ${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp)

  # TODO: Remove support or add RegEx
endif()

# -------------------------------------------------------------------------------------------------------------------
add_library(minizinc
lib/ast.cpp
lib/astexception.cpp
lib/aststring.cpp
lib/astvec.cpp
lib/builtins.cpp
lib/copy.cpp
lib/eval_par.cpp
lib/file_utils.cpp
lib/gc.cpp
lib/htmlprinter.cpp
lib/json_parser.cpp
lib/miniz.c
lib/cdecode.c
lib/cencode.c
lib/model.cpp
lib/prettyprinter.cpp
lib/pathfileprinter.cpp
lib/type.cpp
lib/typecheck.cpp
lib/flatten.cpp
lib/MIPdomains.cpp
lib/optimize.cpp
lib/options.cpp
lib/optimize_constraints.cpp
lib/output.cpp
lib/parser.yxx
lib/solver_instance.cpp
lib/solver_instance_base.cpp
lib/statistics.cpp
lib/lexer.lxx
lib/values.cpp
include/minizinc/ast.hh
include/minizinc/ast.hpp
include/minizinc/astexception.hh
include/minizinc/astiterator.hh
include/minizinc/aststring.hh
include/minizinc/astvec.hh
include/minizinc/builtins.hh
include/minizinc/config.hh.in
include/minizinc/copy.hh
include/minizinc/eval_par.hh
include/minizinc/exception.hh
include/minizinc/file_utils.hh
include/minizinc/flatten.hh
include/minizinc/flatten_internal.hh
include/minizinc/gc.hh
include/minizinc/hash.hh
include/minizinc/htmlprinter.hh
include/minizinc/iter.hh
include/minizinc/json_parser.hh
include/minizinc/model.hh
include/minizinc/optimize.hh
include/minizinc/optimize_constraints.hh
include/minizinc/options.hh
include/minizinc/output.hh
include/minizinc/parser.hh
include/minizinc/prettyprinter.hh
include/minizinc/pathfileprinter.hh
include/minizinc/statistics.hh
include/minizinc/timer.hh
include/minizinc/type.hh
include/minizinc/typecheck.hh
include/minizinc/utils.hh
include/minizinc/values.hh
include/minizinc/solver_instance.hh
include/minizinc/solver_instance_base.hh
include/minizinc/thirdparty/miniz.h
include/minizinc/thirdparty/b64/cdecode.h
include/minizinc/thirdparty/b64/cencode.h
include/minizinc/thirdparty/b64/decode.h
include/minizinc/thirdparty/b64/encode.h
${BISON_MZNParser_OUTPUTS}
${FLEX_MZNLexer_OUTPUTS}
${BISON_RegExParser_OUTPUTS}
${FLEX_RegExLexer_OUTPUTS}
)

add_library(minizinc_solver
lib/flattener.cpp
lib/passes/compile_pass.cpp
lib/process.cpp
lib/solns2out.cpp
lib/solver.cpp
lib/solver_config.cpp
include/minizinc/flattener.hh
include/minizinc/passes/compile_pass.hh
include/minizinc/process.hh
include/minizinc/solns2out.hh
include/minizinc/solver.hh
include/minizinc/solver_config.hh
)

target_link_libraries(minizinc_solver minizinc)

find_package ( Threads REQUIRED )

# -------------------------------------------------------------------------------------------------------------------

# -------------------------------------------------------------------------------------------------------------------
if(HAS_GUROBI)  # Version 7.5

	add_library(minizinc_gurobi
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_gurobi_wrap.cpp solvers/MIP/MIP_gurobi_solverfactory.cpp
    include/minizinc/solvers/MIP/MIP_gurobi_wrap.hh include/minizinc/solvers/MIP/MIP_gurobi_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp
	)
  target_include_directories(minizinc_gurobi PRIVATE "${GUROBI_HOME}/include")
  if(HAS_GUROBI_PLUGIN)
    target_link_libraries(minizinc_gurobi minizinc ${NEEDS_LIBDL} ${CMAKE_THREAD_LIBS_INIT})
  else()
    find_library(GUROBI_LIB2 "${GUROBI_HOME}/lib/gurobi75")
    target_link_libraries(minizinc_gurobi minizinc ${GUROBI_LIB2} ${CMAKE_THREAD_LIBS_INIT})
  endif()

  target_link_libraries(minizinc_solver minizinc_gurobi)
  target_compile_definitions( minizinc_solver PRIVATE HAS_GUROBI )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_gurobi)

  INSTALL(TARGETS minizinc_gurobi
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# -------------------------------------------------------------------------------------------------------------------
# -------------------------------------------------------------------------------------------------------------------
if(HAS_XPRESS)

	add_library(minizinc_xpress
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_xpress_wrap.cpp solvers/MIP/MIP_xpress_solverfactory.cpp
    include/minizinc/solvers/MIP/MIP_xpress_wrap.hh include/minizinc/solvers/MIP/MIP_xpress_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_solverinstance.hpp
	)

  target_include_directories(minizinc_xpress PRIVATE "${XPRESS_HOME}/include")
  link_directories("${XPRESS_HOME}/lib")
  target_link_libraries(minizinc_xpress minizinc xprb xprs ${CMAKE_THREAD_LIBS_INIT})

  target_link_libraries(minizinc_solver minizinc_xpress)
  target_compile_definitions( minizinc_solver PRIVATE HAS_XPRESS )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_xpress)

  INSTALL(TARGETS minizinc_xpress
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

endif()

# -------------------------------------------------------------------------------------------------------------------
if(HAS_CPLEX)  # Windows: version 12.6.2 with MSVC 2013, 64-bit (run cmake .. -G"Visual Studio 12 Win64")
  if(UNIX)
    set(CPLEX_ARCH "x86-64_linux/static_pic")
	set(CPLEX_LIB "cplex")
  endif(UNIX)

  if(APPLE)
     set(CPLEX_ARCH "x86-64_osx/static_pic")
	set(CPLEX_LIB "cplex")
  endif(APPLE)

  if(WIN32)
     set(CPLEX_ARCH "x64_windows_vs2013/stat_mda")
	set(CPLEX_LIB "cplex1263.lib")          # Here 12.6.3
  endif(WIN32)

  find_library(CPLEX_LIB2 NAMES ${CPLEX_LIB} PATHS "${CPLEX_STUDIO_DIR}/cplex/lib/${CPLEX_ARCH}")

  # add_definitions(-DIL_STD)

  # link_directories("${CPLEX_STUDIO_DIR}/cplex/lib/${CPLEX_ARCH}")
  # link_directories("${CPLEX_STUDIO_DIR}/concert/lib/x86-64_${CPLEX_ARCH}/static_pic")

  add_library(minizinc_cplex
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_cplex_wrap.cpp
    solvers/MIP/MIP_cplex_solverfactory.cpp include/minizinc/solvers/MIP/MIP_cplex_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_cplex_wrap.hh
  )
  SET_TARGET_PROPERTIES(minizinc_cplex
                               PROPERTIES COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG"
                               )
  include_directories("${CPLEX_STUDIO_DIR}/cplex/include")
  # target_include_directories(minizinc_cplex PRIVATE "${CPLEX_STUDIO_DIR}/concert/include")

  if(HAS_CPLEX_PLUGIN)
    target_link_libraries(minizinc_cplex minizinc ${CMAKE_THREAD_LIBS_INIT} ${NEEDS_LIBDL})
  else()
    target_link_libraries(minizinc_cplex minizinc ${CPLEX_LIB2} ${CMAKE_THREAD_LIBS_INIT} ${NEEDS_LIBDL})
  endif()

  SET_TARGET_PROPERTIES(minizinc_solver
    PROPERTIES COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD -Wall"
  )
  target_link_libraries(minizinc_solver minizinc_cplex ${CMAKE_THREAD_LIBS_INIT} ${NEEDS_LIBDL})
  target_compile_definitions( minizinc_solver PRIVATE HAS_CPLEX )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_cplex)

  INSTALL(TARGETS minizinc_cplex
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# ----------------------------------------------------------------------------------------------------------
if(HAS_SCIP)   # SCIP version tested: 3.2.1
  #link_directories("${SCIP_DIR}/../lib"  )  # For the single library, use 'make scipoptlib' when building SCIP

  if(UNIX)
#    set(SCIP_OS "linux")
#    set(SCIP_ARCH "x86_64")
   set(SCIP_LIBS "scipopt-3.2.1.linux.x86_64.gnu.opt")
#    set(SCIP_LIB_EXTRAS readline z zimpl.${SCIP_OS}.${SCIP_ARCH}.gnu.opt gmp)  # if SCIP configured so
   set(SCIP_LIB_EXTRAS readline z gmp)  # if SCIP configured so
  endif(UNIX)
  if(APPLE)
#   set(SCIP_OS "osx")
#    set(SCIP_ARCH "ppc")
    set(SCIP_LIBS "scipopt-3.2.1.osx.ppc.gnu.opt")
   set(SCIP_LIB_EXTRAS readline z gmp)  # if SCIP configured so
  endif(APPLE)
  if (WIN32 AND NOT UNIX)
    set(SCIP_LIBS "scip-3.2.1.win.x86_64.msvc.opt.spx.ld")
  endif()

  find_library(SCIP_LIBS2 NAMES ${SCIP_LIBS} PATHS ${SCIP_DIR}/../lib)
  message(SCIPLIBS2 : ${SCIP_LIBS2})

  add_library(minizinc_scip
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_scip_wrap.cpp
    solvers/MIP/MIP_scip_solverfactory.cpp
    )
  target_include_directories(minizinc_scip PRIVATE
    "${SCIP_DIR}/src"
    "${SOPLEX_DIR}/src"
    "${ZIMPL_DIR}/src")
  target_link_libraries(minizinc_scip minizinc
    ${SCIP_LIBS2}
    ${SCIP_LIB_EXTRAS}
    ${CMAKE_THREAD_LIBS_INIT})

  target_link_libraries(minizinc_solver minizinc_scip)
  target_compile_definitions( minizinc_solver PRIVATE HAS_SCIP )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_scip)

  INSTALL(TARGETS minizinc_scip
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# ----------------------------------------------------------------------------------------------------------
if(HAS_SCIP_SEPLIBS)
  # SCIP version tested: 3.2.0

  link_directories("${SCIP_DIR}/lib"  )
  link_directories("${SOPLEX_DIR}/lib")
  link_directories("${ZIMPL_DIR}/lib" )

  if(UNIX)
    set(SCIP_OS "linux")
    set(SCIP_ARCH "x86_64")
  endif(UNIX)

  if(APPLE)
     set(SCIP_OS "osx")
     set(SCIP_ARCH "ppc")
  endif(APPLE)

  set(SCIP_LIB_EXTRAS
    readline
    z zimpl.${SCIP_OS}.${SCIP_ARCH}.gnu.opt gmp)  # if SCIP configured so

  add_library(minizinc_mip_scip
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_scip_wrap.cpp
    )
  target_include_directories(minizinc_mip_scip PRIVATE
    "${SCIP_DIR}/src"
    "${SOPLEX_DIR}/src"
    "${ZIMPL_DIR}/src")
  target_link_libraries(minizinc_mip_scip minizinc
    scip
    nlpi.cppad #objscip
    lpispx soplex
    ${SCIP_LIB_EXTRAS}
    ${CMAKE_THREAD_LIBS_INIT})

  add_executable(mzn-mip-scip minizinc.cpp)
  target_compile_definitions( mzn-mip-scip PRIVATE HAS_MIP )
  target_include_directories(mzn-mip-scip PRIVATE
    "${SCIP_DIR}/src"
    "${SOPLEX_DIR}/src"
    "${ZIMPL_DIR}/src")
  target_link_libraries(mzn-mip-scip minizinc_mip_scip
    scip
    nlpi.cppad #objscip
    lpispx soplex
    ${SCIP_LIB_EXTRAS}
    ${CMAKE_THREAD_LIBS_INIT})

  INSTALL(TARGETS minizinc_mip_scip mzn-mip-scip
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# -------------------------------------------------------------------------------------------------------------------
if(HAS_OSICBC)
  ### Tested version 2.9.9, rev 2345 on 12.10.17 from the stable snapshot https://projects.coin-or.org/svn/Cbc/stable/2.9

  if (EXISTS "${OSICBC_HOME}/include/coin/" AND EXISTS "${OSICBC_HOME}/lib")
    set(CBC_INCLUDEDIRS "${OSICBC_HOME}/include/coin/")
    set(LNDIR "${OSICBC_HOME}/lib")
  else()
    set(CBC_INCLUDEDIRS "${OSICBC_HOME}/Clp/src/" "${OSICBC_HOME}/Clp/src/OsiClp" "${OSICBC_HOME}/Cbc/src/" "${OSICBC_HOME}/Cbc/src/OsiCbc" "${OSICBC_HOME}/Cgl/src/CglZeroHalf" "${OSICBC_HOME}/Cgl/src/CglLandP" "${OSICBC_HOME}/Cgl/src/CglTwomir" "${OSICBC_HOME}/Cgl/src/CglMixedIntegerRounding" "${OSICBC_HOME}/Cgl/src/CglMixedIntegerRounding2" "${OSICBC_HOME}/Cgl/src/CglFlowCover" "${OSICBC_HOME}/Cgl/src/CglClique" "${OSICBC_HOME}/Cgl/src/CglOddHole" "${OSICBC_HOME}/Cgl/src/CglKnapsackCover" "${OSICBC_HOME}/Cgl/src/CglGomory" "${OSICBC_HOME}/Cgl/src/CglPreProcess" "${OSICBC_HOME}/Cgl/src/CglDuplicateRow" "${OSICBC_HOME}/Cgl/src/CglRedSplit" "${OSICBC_HOME}/Cgl/src/CglProbing" "${OSICBC_HOME}/Cgl/src" "${OSICBC_HOME}/Clp/src" "${OSICBC_HOME}/Clp/src/OsiClp" "${OSICBC_HOME}/Osi/src/Osi" "${OSICBC_HOME}/CoinUtils/src" "${OSICBC_HOME}/BuildTools/headers" "${OSICBC_HOME}/Cgl/src/CglResidualCapacity")
    set(LNDIR "${OSICBC_HOME}/Cbc/MSVisualStudio/v10/x64-v120-Release")
  endif()

  if (WIN32 AND NOT UNIX)  #  Windows config based on  https://projects.coin-or.org/Cbc/wiki/VSSetup
    set(OSICBC_LIBS "libOsi" "libOsiClp" "libOsiCbc" "libClp" "libCgl" "libCbc" "libCbcSolver" "libCoinUtils")
  else ()
    # The order below is important for static versions of cbc libs to link. "z" needed for static
    set(OSICBC_LIBS "CbcSolver" "Cbc" "Cgl" "OsiClp"  "Clp" "Osi"  "CoinUtils" "z") # "ClpSolver" after OsiClp;  "OsiCbc"
#    set(OSICBC_LIBS "`PKG_CONFIG_PATH=/home/bg/Documents/prj/coin-Cbc-2.9/lib64/pkgconfig:/home/bg/Documents/prj/coin-Cbc-2.9/lib/pkgconfig:/home/bg/Documents/prj/coin-Cbc-2.9/share/pkgconfig: pkg-config --libs cbc`")
  endif()
  link_directories(${LNDIR})

  add_library(minizinc_osicbc
    solvers/MIP/MIP_solverinstance.cpp solvers/MIP/MIP_osicbc_wrap.cpp
    solvers/MIP/MIP_osicbc_solverfactory.cpp include/minizinc/solvers/MIP/MIP_osicbc_solverfactory.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hh include/minizinc/solvers/MIP/MIP_osicbc_wrap.hh
    include/minizinc/solvers/MIP/MIP_solverinstance.hpp
  )

  target_include_directories(minizinc_osicbc PRIVATE ${CBC_INCLUDEDIRS})

                                       # OSICBC_LINKEXTRAS: ArchLinux needs blas, lapack, bz2
  target_link_libraries(minizinc_osicbc minizinc ${OSICBC_LIBS} ${OSICBC_LINKEXTRAS})

  target_link_libraries(minizinc_solver minizinc_osicbc ${OSICBC_LIBS} ${OSICBC_LINKEXTRAS} ${CMAKE_THREAD_LIBS_INIT})
  target_compile_definitions( minizinc_solver PRIVATE HAS_OSICBC )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_osicbc)

  INSTALL(TARGETS minizinc_osicbc
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

# -------------------------------------------------------------------------------------------------------------------
if(HAS_GECODE)

  add_definitions(-DHAS_GECODE)
  include_directories("${GECODE_HOME}/include")
  include_directories("${GECODE_HOME}")
  link_directories("${GECODE_HOME}/lib")
  link_directories("${GECODE_HOME}")

  add_library(minizinc_gecode
    solvers/gecode/fzn_space.cpp
    solvers/gecode/gecode_solverinstance.cpp
    solvers/gecode/gecode_solverfactory.cpp
    solvers/gecode/gecode_constraints.cpp
    lib/passes/gecode_pass.cpp
    solvers/gecode/aux_brancher.hh
    include/minizinc/passes/gecode_pass.hh
    include/minizinc/solvers/gecode/fzn_space.hh
    include/minizinc/solvers/gecode_solverinstance.hh
    include/minizinc/solvers/gecode_solverfactory.hh
    include/minizinc/solvers/gecode/gecode_constraints.hh
    ${parser_hh}
    )

  target_include_directories(minizinc_gecode PRIVATE "${GECODE_HOME}/include" "${GECODE_HOME}")
  target_link_libraries(minizinc_gecode
    minizinc ${CMAKE_THREAD_LIBS_INIT}
  )

  if(NOT (SHARED_GECODE AND WIN32) )
    target_link_libraries(minizinc_gecode
      gecodedriver gecodesearch gecodeminimodel
      gecodefloat gecodeint gecodeset
      gecodekernel gecodesupport
    )
  endif()

  find_package(MPFR)
  if(MPFR_FOUND AND NOT DEFINED GECODE_NO_MPFR)
    target_include_directories(minizinc_gecode PRIVATE ${MPFR_INCLUDES})
    target_link_libraries(minizinc_gecode ${MPFR_LIBRARIES})
  endif()

  target_link_libraries(minizinc_solver minizinc_gecode)
  target_compile_definitions( minizinc_solver PRIVATE HAS_GECODE )
  set(EXTRA_TARGETS ${EXTRA_TARGETS} minizinc_gecode)

  INSTALL(TARGETS minizinc_gecode
    EXPORT libminizincTargets
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

  INSTALL(DIRECTORY solvers/gecode
    DESTINATION include/minizinc/solvers
  )

endif()

# -------------------------------------------------------------------------------------------------------------------
add_library(minizinc_fzn
  solvers/fzn/fzn_solverinstance.cpp
  solvers/fzn/fzn_solverfactory.cpp
  include/minizinc/solvers/fzn_solverfactory.hh
  include/minizinc/solvers/fzn_solverinstance.hh
  solvers/mzn/mzn_solverinstance.cpp
  solvers/mzn/mzn_solverfactory.cpp
  include/minizinc/solvers/mzn_solverfactory.hh
  include/minizinc/solvers/mzn_solverinstance.hh
)
target_link_libraries(minizinc_fzn minizinc)

add_executable(mzn2fzn minizinc.cpp)
target_link_libraries(mzn2fzn minizinc_solver)
add_executable(solns2out minizinc.cpp)
target_link_libraries(solns2out minizinc_solver)

add_executable( minizinc-bin minizinc.cpp )
set_target_properties( minizinc-bin PROPERTIES OUTPUT_NAME minizinc)
target_link_libraries(minizinc_solver minizinc_fzn)
target_link_libraries(minizinc-bin minizinc_solver)

add_executable(mzn2doc mzn2doc.cpp)
target_link_libraries(mzn2doc minizinc_solver)

INSTALL(TARGETS minizinc minizinc_solver minizinc_fzn minizinc-bin mzn2fzn solns2out mzn2doc
  EXPORT libminizincTargets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib)

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
INSTALL(DIRECTORY lib/cached DESTINATION lib PATTERN md5_cached.cmake EXCLUDE)
INSTALL(FILES ${parser_hh} DESTINATION include/minizinc)


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
