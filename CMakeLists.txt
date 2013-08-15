cmake_minimum_required (VERSION 2.6)
project (libmzn CXX)

# The version number.
set (libmzn_VERSION_MAJOR 0)
set (libmzn_VERSION_MINOR 8)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-std=c++11 HAS_STDCPP0X)
check_cxx_compiler_flag(-stdlib=libc++ HAS_STDLIBLIBCPP)

if (HAS_STDCPP0X)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
endif()

if (HAS_STDLIBLIBCPP)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
endif()

# configure a header file to pass some of the CMake settings
# to the source code
#configure_file (
#  "${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in"
#  "${PROJECT_BINARY_DIR}/config.hh"
#  )

# add the binary tree to the search path for include files
# so that we will find TutorialConfig.h
include_directories("${PROJECT_BINARY_DIR}")

include_directories("${PROJECT_SOURCE_DIR}/include")

add_library(mzn
lib/allocator.cpp
lib/ast.cpp
lib/aststring.cpp
lib/astvec.cpp
lib/builtins.cpp
lib/copy.cpp
lib/eval_par.cpp
lib/gc.cpp
lib/lexer.yy.cpp
lib/model.cpp
lib/parser.tab.cpp
lib/prettyprinter.cpp
lib/typecheck.cpp
)

# add the executable
add_executable(mzn2fzn mzn2fzn.cpp)
target_link_libraries(mzn2fzn mzn)
