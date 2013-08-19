cmake_minimum_required (VERSION 2.6)
project (libmzn CXX)

# The version number.
set (libmzn_VERSION_MAJOR 0)
set (libmzn_VERSION_MINOR 8)

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

check_cxx_compiler_flag(-stdlib=libc++ HAS_STDLIBLIBCPP)

if (HAS_STDLIBLIBCPP)
  set(SAFE_CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  check_cxx_source_compiles("int main(void) {return 0;}" LINKS_STDLIBLIBCPP)
  if (NOT LINKS_STDLIBLIBCPP)
    set(CMAKE_CXX_FLAGS "${SAFE_CMAKE_CXX_FLAGS}")
  endif()
endif()
   
CHECK_CXX_SOURCE_COMPILES("int main(void) { static __thread int x; return 0;}" HAS_ATTR_THREAD)

if (NOT HAS_ATTR_THREAD)
  CHECK_CXX_SOURCE_COMPILES("int main(void) { __declspec(thread) static int x; return 0;}" HAS_DECLSPEC_THREAD)
endif()


SET (CMAKE_REQUIRED_DEFINITIONS "${SAFE_CMAKE_REQUIRED_DEFINITIONS}")

# configure a header file to pass some of the CMake settings
# to the source code
configure_file (
  "${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in"
  "${PROJECT_BINARY_DIR}/config.hh"
  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
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

add_custom_command(
  SOURCE "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
  COMMAND "bison"
  ARGS -o "${PROJECT_BINARY_DIR}/parser.tab.cpp"
       --defines="${PROJECT_BINARY_DIR}/parser.tab.hh"
       "${PROJECT_SOURCE_DIR}/lib/parser.yxx"
  TARGET MZNParser
  DEPENDS "${PROJECT_BINARY_DIR}/lexer.yy.cpp"
  OUTPUTS "${PROJECT_BINARY_DIR}/parser.tab.cpp" 
          "${PROJECT_BINARY_DIR}/parser.tab.hh"
)

set_source_files_properties("${PROJECT_BINARY_DIR}/lexer.yy.cpp" GENERATED)
set_source_files_properties("${PROJECT_BINARY_DIR}/parser.tab.cpp" GENERATED)

add_library(mzn
lib/allocator.cpp
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
)

# add the executable
add_executable(mzn2fzn mzn2fzn.cpp)
target_link_libraries(mzn2fzn mzn)
