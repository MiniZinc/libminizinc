cmake_minimum_required (VERSION 2.8.7)

if (USE_STDLIBCPP)
  set(MZN_LIBCPP libstdc++)
else()
  set(MZN_LIBCPP libc++)
endif()

if(APPLE)
   SET(CMAKE_OSX_DEPLOYMENT_TARGET "10.7")
   set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
   set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "${MZN_LIBCPP}")
endif(APPLE)

project (libminizinc CXX C)

# The version number.
set (libminizinc_VERSION_MAJOR 2)
set (libminizinc_VERSION_MINOR 0)
set (libminizinc_VERSION_PATCH 4)

if (ADDITIONAL_DATE_STRING)
  set (libminizinc_VERSION_PATCH "${libminizinc_VERSION_PATCH}.${ADDITIONAL_DATE_STRING}")
endif()

include(CheckCXXCompilerFlag)

SET(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
check_cxx_compiler_flag(-Werror HAS_WERROR)

if (HAS_WERROR)
  SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
endif()

check_cxx_compiler_flag(-std=c++11 HAS_STDCPP11)
if (HAS_STDCPP11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  check_cxx_compiler_flag(-std=c++0x HAS_STDCPP0X)
  if (HAS_STDCPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
endif()

check_cxx_compiler_flag(-stdlib=${MZN_LIBCPP} HAS_STDLIBLIBCPP)

if (HAS_STDLIBLIBCPP)
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
  check_cxx_source_compiles("#include <tr1/unordered_set>
                             int main(void) { std::tr1::unordered_set<int> x; return 0; }" HAS_CPP11_TR1)
  if (HAS_CPP11_TR1)
    set(MZN_NEED_TR1 1)
  else()
    message(FATAL_ERROR "A c++11 compatible C++ standard library is required to build libminizinc.")
  endif()
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

SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/minizinc)
file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/doc/html)

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in"
  "${PROJECT_BINARY_DIR}/minizinc/config.hh"
  )

include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_target(MZNParser echo "Creating parser")

# When updating the cached files, update MD5 sums defined in this file
include(${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake)

macro(MD5 filename md5sum)
  file(READ "${filename}" RAW_MD5_FILE)
  string(REGEX REPLACE "\r" "" STRIPPED_MD5_FILE "${RAW_MD5_FILE}")
  string(MD5 ${md5sum} "${STRIPPED_MD5_FILE}")
endmacro(MD5)

find_program(FLEX_EXEC flex)
if(FLEX_EXEC)
  add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/lexer.yy.cpp"
    DEPENDS "${PROJECT_SOURCE_DIR}/lib/lexer.lxx"
    COMMAND "${FLEX_EXEC}" -L -o"${PROJECT_BINARY_DIR}/lexer.yy.cpp" "${PROJECT_SOURCE_DIR}/lib/lexer.lxx"
    )
  set_source_files_properties("${PROJECT_BINARY_DIR}/lexer.yy.cpp" GENERATED)
  set(lexer_cpp "${PROJECT_BINARY_DIR}/lexer.yy.cpp")
else(FLEX_EXEC)
  MD5("${PROJECT_SOURCE_DIR}/lib/lexer.lxx" lexer_lxx_md5)
  if(NOT "${lexer_lxx_md5}" STREQUAL "${lexer_lxx_md5_cached}")
    message(FATAL_ERROR "The file lexer.lxx has been modified but flex cannot be run.")
  endif()
  set(lexer_cpp "${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp")
endif(FLEX_EXEC)

find_program(BISON_EXEC bison)
if(BISON_EXEC)
  add_custom_command(
    OUTPUT "${PROJECT_BINARY_DIR}/parser.tab.cpp"  "${PROJECT_BINARY_DIR}/minizinc/parser.tab.hh"
    DEPENDS "${lexer_cpp}" "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
    COMMAND "${BISON_EXEC}" -l -o "${PROJECT_BINARY_DIR}/parser.tab.cpp" --defines="${PROJECT_BINARY_DIR}/minizinc/parser.tab.hh" "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
    )
  set_source_files_properties("${PROJECT_BINARY_DIR}/parser.tab.cpp" GENERATED)
  set(parser_cpp "${PROJECT_BINARY_DIR}/parser.tab.cpp")
  set(parser_hh "${PROJECT_BINARY_DIR}/minizinc/parser.tab.hh")
else(BISON_EXEC)
  MD5("${PROJECT_SOURCE_DIR}/lib/parser.yxx" parser_yxx_md5)
  if(NOT "${parser_yxx_md5}" STREQUAL "${parser_yxx_md5_cached}")
    message(FATAL_ERROR "The file parser.yxx has been modified but bison cannot be run.")
  endif()
  include_directories(${PROJECT_SOURCE_DIR}/lib/cached)
  set(parser_cpp "${PROJECT_SOURCE_DIR}/lib/cached/parser.tab.cpp")
  set(parser_hh "${PROJECT_SOURCE_DIR}/lib/cached/minizinc/parser.tab.hh")
endif(BISON_EXEC)


add_library(minizinc
lib/ast.cpp
lib/astexception.cpp
lib/aststring.cpp
lib/astvec.cpp
lib/builtins.cpp
lib/cli.cpp
lib/copy.cpp
lib/eval_par.cpp
lib/file_utils.cpp
lib/gc.cpp
lib/htmlprinter.cpp
"${lexer_cpp}"
lib/model.cpp
"${parser_cpp}"
lib/prettyprinter.cpp
lib/solver.cpp
lib/solver_instance.cpp
lib/solver_instance_base.cpp
lib/typecheck.cpp
lib/flatten.cpp
lib/optimize.cpp
lib/options.cpp
lib/optimize_constraints.cpp
lib/parser.yxx
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
include/minizinc/cli.hh
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
include/minizinc/model.hh
include/minizinc/optimize.hh
include/minizinc/optimize_constraints.hh
include/minizinc/options.hh
include/minizinc/parser.hh
include/minizinc/prettyprinter.hh
include/minizinc/solver.hh
include/minizinc/solver_instance.hh
include/minizinc/solver_instance_base.hh
include/minizinc/statistics.hh
include/minizinc/stl_map_set.hh
include/minizinc/timer.hh
include/minizinc/type.hh
include/minizinc/typecheck.hh
include/minizinc/values.hh
include/minizinc/stl_map_set.hh
include/minizinc/thirdparty/SafeInt3.hpp
"${parser_hh}"
)

# add the executable
add_executable(mzn2fzn mzn2fzn.cpp)
target_link_libraries(mzn2fzn minizinc)

add_executable(mzn2fzn_test mzn2fzn_test.cpp)
target_link_libraries(mzn2fzn_test minizinc)

add_executable(solns2out solns2out.cpp)
target_link_libraries(solns2out minizinc)

find_package ( Threads REQUIRED )

set(HAS_CPLEX FALSE)
set(HAS_GUROBI TRUE)
set(HAS_GECODE TRUE)
set(HAS_FZN    TRUE)

if(HAS_GUROBI)
	
  link_directories("${GUROBI_HOME}/lib")

	add_library(minizinc_gurobi
		solvers/gurobi/gurobi_solverinstance.cpp
	)
  target_include_directories(minizinc_gurobi PRIVATE "${GUROBI_HOME}/include")
	target_link_libraries(minizinc_gurobi minizinc gurobi_c++ gurobi60)

	add_executable(mzn-gurobi
	  solvers/gurobi/mzn-gurobi.cpp
	)
  target_include_directories(mzn-gurobi PRIVATE "${GUROBI_HOME}/include")
	target_link_libraries(mzn-gurobi minizinc_gurobi minizinc gurobi_c++ gurobi60 ${CMAKE_THREAD_LIBS_INIT})

  INSTALL(TARGETS minizinc_gurobi mzn-gurobi
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

if(HAS_CPLEX)
  if(UNIX)
    set(CPLEX_ARCH "linux")
  endif(UNIX)

  if(APPLE)
     set(CPLEX_ARCH "osx")
  endif(APPLE)

#	add_definitions(-DIL_STD)
	
  link_directories("${CPLEX_STUDIO_DIR}/cplex/lib/x86-64_${CPLEX_ARCH}/static_pic")
  link_directories("${CPLEX_STUDIO_DIR}/concert/lib/x86-64_${CPLEX_ARCH}/static_pic")

	add_library(minizinc_cplex
		solvers/cplex/cplex_solverinstance.cpp
	)
  SET_TARGET_PROPERTIES(minizinc_cplex
                               PROPERTIES COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD"
                               )
  target_include_directories(minizinc_cplex PRIVATE
    "${CPLEX_STUDIO_DIR}/cplex/include"
  )
  target_include_directories(minizinc_cplex PRIVATE "${CPLEX_STUDIO_DIR}/concert/include")
  
	target_link_libraries(minizinc_cplex minizinc ilocplex cplex concert)

	add_executable(mzn-cplex
	  solvers/cplex/mzn-cplex.cpp
	)
	SET_TARGET_PROPERTIES(mzn-cplex
                               PROPERTIES COMPILE_FLAGS "-fPIC -fno-strict-aliasing -fexceptions -DNDEBUG -DIL_STD"
                               )
  target_include_directories(mzn-cplex PRIVATE "${CPLEX_STUDIO_DIR}/cplex/include")
  target_include_directories(mzn-cplex PRIVATE "${CPLEX_STUDIO_DIR}/concert/include")
  target_link_libraries(mzn-cplex minizinc_cplex minizinc ilocplex cplex concert ${CMAKE_THREAD_LIBS_INIT})

  INSTALL(TARGETS minizinc_cplex mzn-cplex
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)
endif()

if(HAS_GECODE)
  include_directories("${GECODE_HOME}/include")
  link_directories("${GECODE_HOME}/lib")

  add_library(minizinc_gecode
          solvers/gecode/fzn_space.cpp
	  solvers/gecode/gecode_solverinstance.cpp
		solvers/gecode/gecode_constraints.cpp
    solvers/gecode/aux_brancher.hh)

  target_link_libraries(minizinc_gecode
    minizinc
    gecodedriver gecodesearch gecodeminimodel
    gecodefloat gecodeint gecodeset
    gecodekernel gecodesupport)

  add_executable(mzn-gecode
    solvers/gecode/mzn-gecode.cpp
  )
  target_link_libraries(mzn-gecode minizinc_gecode minizinc 
    gecodedriver gecodesearch gecodeminimodel
    gecodefloat gecodeint gecodeset
    gecodekernel gecodesupport ${CMAKE_THREAD_LIBS_INIT})

  INSTALL(TARGETS minizinc_gecode mzn-gecode
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib)

  INSTALL(DIRECTORY solvers/gecode
          DESTINATION include/minizinc/solvers
  )

endif()
add_executable(mzn2doc mzn2doc.cpp)
target_link_libraries(mzn2doc minizinc)
if(HAS_FZN)
	add_library(minizinc_fzn
		solvers/fzn/fzn_solverinstance.cpp
	)
	target_link_libraries(minizinc_fzn minizinc)
	add_executable(mzn-fzn solvers/fzn/mzn-fzn.cpp)
	target_link_libraries(mzn-fzn minizinc_fzn minizinc)
        add_executable(mzn-fzn-test solvers/fzn/mzn-fzn-test.cpp)
	target_link_libraries(mzn-fzn-test minizinc_fzn minizinc)
	INSTALL(TARGETS minizinc_fzn mzn-fzn mzn-fzn-test
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		ARCHIVE DESTINATION lib)
endif()

INSTALL(TARGETS mzn2fzn mzn2fzn_test solns2out mzn2doc minizinc
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

option (BUILD_HTML_DOCUMENTATION "Build HTML documentation for the MiniZinc library" OFF)

if (BUILD_HTML_DOCUMENTATION)
  add_custom_target(doc ALL
    DEPENDS "mzn2doc"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
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

option (BUILD_STATIC_EXECUTABLE "Build statically linked executables" OFF)

if (BUILD_STATIC_EXECUTABLE)
  set(CMAKE_EXE_LINKER_FLAGS -static)
endif()

INSTALL(DIRECTORY share/minizinc DESTINATION share)
INSTALL(DIRECTORY include/minizinc
        DESTINATION include
        PATTERN config.hh.in EXCLUDE
)
INSTALL(FILES ${PROJECT_BINARY_DIR}/minizinc/config.hh DESTINATION include/minizinc)
INSTALL(FILES README.txt INSTALL.txt LICENSE.txt DESTINATION doc/..)
INSTALL(DIRECTORY doc/pdf DESTINATION doc)
INSTALL(DIRECTORY share/examples DESTINATION share)
INSTALL(DIRECTORY lib/cached DESTINATION lib PATTERN md5_cached.cmake EXCLUDE)
INSTALL(FILES "${parser_hh}" DESTINATION include/minizinc)

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
