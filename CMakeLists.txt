cmake_minimum_required (VERSION 2.6)
project (libminizinc CXX)

# The version number.
set (libminizinc_VERSION_MAJOR 0)
set (libminizinc_VERSION_MINOR 8)

include(CheckCXXCompilerFlag)

SET(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
check_cxx_compiler_flag(-Werror HAS_WERROR)

if (HAS_WERROR)
  SET(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
endif()

set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LANGUAGE_STANDARD "c++0x")
set(CMAKE_XCODE_ATTRIBUTE_CLANG_CXX_LIBRARY "libc++")

check_cxx_compiler_flag(-std=c++11 HAS_STDCPP11)
if (HAS_STDCPP11)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
else()
  check_cxx_compiler_flag(-std=c++0x HAS_STDCPP0X)
  if (HAS_STDCPP0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
  endif()
endif()

check_cxx_compiler_flag(-stdlib=libc++ HAS_STDLIBLIBCPP)

if (HAS_STDLIBLIBCPP)
  set(SAFE_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  check_cxx_source_compiles("int main(void) {return 0;}" LINKS_STDLIBLIBCPP)
  if (NOT LINKS_STDLIBLIBCPP)
    set(CMAKE_CXX_FLAGS "${SAFE_CMAKE_CXX_FLAGS}")
  endif()
endif()
   
CHECK_CXX_SOURCE_COMPILES("int main(void) { static __thread int x; (void)x; return 0;}" HAS_ATTR_THREAD)

if (NOT HAS_ATTR_THREAD)
  CHECK_CXX_SOURCE_COMPILES("int main(void) { __declspec(thread) static int x; (void)x; return 0;}" HAS_DECLSPEC_THREAD)
endif()


SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in"
  "${PROJECT_BINARY_DIR}/config.hh"
  )

include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_custom_target(MZNParser echo "Creating parser")

add_custom_command(
  SOURCE "${PROJECT_SOURCE_DIR}/lib/lexer.lxx"
  COMMAND "flex"
  ARGS -o"${PROJECT_BINARY_DIR}/lexer.yy.cpp"
       "${PROJECT_SOURCE_DIR}/lib/lexer.lxx"
  TARGET MZNParser
  OUTPUTS "${PROJECT_BINARY_DIR}/lexer.yy.cpp"
)

file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/minizinc)

add_custom_command(
  SOURCE "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
  COMMAND "bison"
  ARGS -o "${PROJECT_BINARY_DIR}/parser.tab.cpp"
       --defines="${PROJECT_BINARY_DIR}/minizinc/parser.tab.hh"
       "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
  TARGET MZNParser
  DEPENDS "${PROJECT_BINARY_DIR}/lexer.yy.cpp"
  OUTPUTS "${PROJECT_BINARY_DIR}/parser.tab.cpp" 
          "${PROJECT_BINARY_DIR}/minizinc/parser.tab.hh"
)

set_source_files_properties("${PROJECT_BINARY_DIR}/lexer.yy.cpp" GENERATED)
set_source_files_properties("${PROJECT_BINARY_DIR}/parser.tab.cpp" GENERATED)

add_library(minizinc
lib/ast.cpp
lib/aststring.cpp
lib/astvec.cpp
lib/builtins.cpp
lib/copy.cpp
lib/eval_par.cpp
lib/gc.cpp
"${PROJECT_BINARY_DIR}/lexer.yy.cpp"
lib/model.cpp
"${PROJECT_BINARY_DIR}/parser.tab.cpp"
lib/prettyprinter.cpp
lib/typecheck.cpp
lib/flatten.cpp
lib/optimize.cpp
lib/parser.yxx
lib/lexer.lxx
include/minizinc/ast.hh
include/minizinc/ast.hpp
include/minizinc/astiterator.hh
include/minizinc/aststring.hh
include/minizinc/astvec.hh
include/minizinc/builtins.hh
include/minizinc/config.hh.in
include/minizinc/copy.hh
include/minizinc/eval_par.hh
include/minizinc/exception.hh
include/minizinc/flatten.hh
include/minizinc/gc.hh
include/minizinc/hash.hh
include/minizinc/iter.hh
include/minizinc/model.hh
include/minizinc/optimize.hh
include/minizinc/parser.hh
include/minizinc/prettyprinter.hh
include/minizinc/type.hh
include/minizinc/typecheck.hh
include/minizinc/values.hh
)

# add the executable
add_executable(mzn2fzn mzn2fzn.cpp)
target_link_libraries(mzn2fzn minizinc)

add_executable(solns2out solns2out.cpp)
target_link_libraries(solns2out minizinc)
