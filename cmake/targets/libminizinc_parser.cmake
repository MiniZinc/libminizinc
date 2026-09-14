# -------------------------------------------------------------------------------------------------------------------
## Parser Generation Targets

# When updating the cached files, update MD5 sums defined in this file
include(${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake)

macro(MD5 filename md5sum)
  file(READ "${filename}" RAW_MD5_FILE)
  string(REGEX REPLACE "\r" "" STRIPPED_MD5_FILE "${RAW_MD5_FILE}")
  string(MD5 ${md5sum} "${STRIPPED_MD5_FILE}")
endmacro(MD5)

find_package(BISON 3.4)
find_package(FLEX 2.5)

if(BISON_FOUND AND FLEX_FOUND)
  BISON_TARGET(MZNParser
    ${PROJECT_SOURCE_DIR}/lib/parser.yxx
    ${PROJECT_BINARY_DIR}/parser.tab.cpp
    DEFINES_FILE ${PROJECT_BINARY_DIR}/include/minizinc/parser.tab.hh
    COMPILE_FLAGS "-p mzn_yy -l -Werror"
  )

  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/minizinc/support/)
  BISON_TARGET(RegExParser
    ${PROJECT_SOURCE_DIR}/lib/support/regex/parser.yxx
    ${PROJECT_BINARY_DIR}/regex_parser.tab.cpp
    DEFINES_FILE ${PROJECT_BINARY_DIR}/include/minizinc/support/regex_parser.tab.hh
    COMPILE_FLAGS "-p regex_yy -l -Werror"
  )

  FLEX_TARGET(MZNLexer
    ${PROJECT_SOURCE_DIR}/lib/lexer.lxx
    ${PROJECT_BINARY_DIR}/lexer.yy.cpp
    COMPILE_FLAGS "-P mzn_yy -L"
    )
  ADD_FLEX_BISON_DEPENDENCY(MZNLexer MZNParser)

  FLEX_TARGET(RegExLexer
    ${PROJECT_SOURCE_DIR}/lib/support/regex/lexer.lxx
    ${PROJECT_BINARY_DIR}/regex_lexer.yy.cpp
    COMPILE_FLAGS "-P regex_yy -L"
    )
  ADD_FLEX_BISON_DEPENDENCY(RegExLexer RegExParser)
else()
  MD5(${PROJECT_SOURCE_DIR}/lib/parser.yxx parser_yxx_md5)
  if(NOT "${parser_yxx_md5}" STREQUAL "${parser_yxx_md5_cached}")
    message(FATAL_ERROR
      "The file parser.yxx has been modified but bison cannot be run.\n"
      "If you are sure parser.tab.cpp and minizinc/parser.tab.hh in ${PROJECT_SOURCE_DIR}/lib/cached/ are correct "
        "then copy parser.yxx's md5 ${parser_yxx_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake"
      )
  endif()

  MD5(${PROJECT_SOURCE_DIR}/lib/support/regex/parser.yxx regex_parser_yxx_md5)
  if(NOT "${regex_parser_yxx_md5}" STREQUAL "${regex_parser_yxx_md5_cached}")
    message(FATAL_ERROR
      "The file regex/parser.yxx has been modified but bison cannot be run.\n"
      "If you are sure regex_parser.tab.cpp and minizinc/support/regex_parser.tab.hh in "
        "${PROJECT_SOURCE_DIR}/lib/cached/ are correct then copy regex_parser.yxx's md5 ${regex_parser_yxx_md5} into "
        "${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake"
      )
  endif()

  MD5(${PROJECT_SOURCE_DIR}/lib/lexer.lxx lexer_lxx_md5)
  if(NOT "${lexer_lxx_md5}" STREQUAL "${lexer_lxx_md5_cached}")
    message(FATAL_ERROR
      "The file lexer.lxx has been modified but flex cannot be run.\n"
      "If you are sure ${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp is correct then "
      "copy lexer.lxx's md5 ${lexer_lxx_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake"
      )
  endif()

  MD5(${PROJECT_SOURCE_DIR}/lib/support/regex/lexer.lxx regex_lexer_lxx_md5)
  if(NOT "${regex_lexer_lxx_md5}" STREQUAL "${regex_lexer_lxx_md5_cached}")
    message(FATAL_ERROR
      "The file regex/lexer.lxx has been modified but flex cannot be run.\n"
      "If you are sure ${PROJECT_SOURCE_DIR}/lib/cached/regex_lexer.yy.cpp is correct then "
      "copy regex/lexer.lxx's md5 ${regex_lexer_lxx_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake"
      )
  endif()

  include_directories(${PROJECT_SOURCE_DIR}/lib/cached)
  set(BISON_MZNParser_OUTPUTS
    ${PROJECT_SOURCE_DIR}/lib/cached/parser.tab.cpp
    ${PROJECT_SOURCE_DIR}/lib/cached/minizinc/parser.tab.hh
  )
  set(BISON_RegExParser_OUTPUTS
    ${PROJECT_SOURCE_DIR}/lib/cached/regex_parser.tab.cpp
    ${PROJECT_SOURCE_DIR}/lib/cached/minizinc/support/regex_parser.tab.hh
  )
  set(FLEX_MZNLexer_OUTPUTS ${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp)
  set(FLEX_RegExLexer_OUTPUTS ${PROJECT_SOURCE_DIR}/lib/cached/regex_lexer.yy.cpp)
endif()

if(NOT GECODE_FOUND)
  set(FLEX_RegExLexer_OUTPUTS "")
  set(BISON_RegExParser_OUTPUTS "")
endif()

# -------------------------------------------------------------------------------------------------------------------
## Tree-sitter runtime and tree-feller

# These versions share tree-sitter grammar ABI 15.
include(FetchContent)

# Link the tree-sitter objects statically.
set(mzn_build_shared_libs "${BUILD_SHARED_LIBS}")
set(BUILD_SHARED_LIBS OFF)

# Give extracted files the extraction time, so a changed URL rebuilds them.
if(POLICY CMP0135)
  cmake_policy(SET CMP0135 NEW)
endif()

FetchContent_Declare(tree-sitter
  URL https://github.com/tree-sitter/tree-sitter/archive/refs/tags/v0.26.12.tar.gz
  URL_HASH SHA256=428e2b182fe38eddc100d8bd851e47c96921a69281b66abafc25ba4b0aaeeeab)
FetchContent_Declare(tree_feller
  URL https://github.com/Dekker1/tree-feller/archive/refs/tags/v0.1.1.tar.gz
  URL_HASH SHA256=4a6b463da95ec6a9a1014732df6e77dabdb5e20c95cd9005107d1d4596e92cf9)
FetchContent_MakeAvailable(tree-sitter tree_feller)

set(BUILD_SHARED_LIBS "${mzn_build_shared_libs}")

# libmzn.cmake embeds these objects; exclude their install rules.
set_property(DIRECTORY "${tree-sitter_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)
set_property(DIRECTORY "${tree_feller_SOURCE_DIR}" PROPERTY EXCLUDE_FROM_ALL TRUE)

# Fail if a vendored grammar was edited without regenerating its parser.
foreach(ts_grammar minizinc datazinc)
  MD5(${PROJECT_SOURCE_DIR}/lib/thirdparty/tree_sitter_${ts_grammar}/grammar.js ts_grammar_js_md5)
  if(NOT "${ts_grammar_js_md5}" STREQUAL "${ts_${ts_grammar}_grammar_js_md5_cached}")
    message(FATAL_ERROR
      "The vendored ${ts_grammar} grammar.js has been modified but the parser was not regenerated.\n"
      "Run ${PROJECT_SOURCE_DIR}/lib/thirdparty/update-tree-sitter.sh, then copy the md5 "
        "${ts_grammar_js_md5} into ${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake as "
        "ts_${ts_grammar}_grammar_js_md5_cached"
      )
  endif()
endforeach()

add_library(minizinc_parser OBJECT
  ${PROJECT_SOURCE_DIR}/lib/thirdparty/tree_sitter_minizinc.c
  ${PROJECT_SOURCE_DIR}/lib/thirdparty/tree_sitter_datazinc.c
  ${BISON_MZNParser_OUTPUTS}
  ${FLEX_MZNLexer_OUTPUTS}
  ${BISON_RegExParser_OUTPUTS}
  ${FLEX_RegExLexer_OUTPUTS}
)
set_target_properties(minizinc_parser PROPERTIES
  CXX_CLANG_TIDY ""
  C_CLANG_TIDY ""
  C_STANDARD 11
  C_STANDARD_REQUIRED ON
)
# Supply tree_sitter/parser.h to the generated grammars.
target_link_libraries(minizinc_parser PRIVATE tree_feller::parser_header)

if(GECODE_FOUND)
  target_include_directories(minizinc_parser PRIVATE "${GECODE_INCLUDE_DIRS}")
  target_compile_definitions(minizinc_parser PRIVATE HAS_GECODE)
endif()
