# -------------------------------------------------------------------------------------------------------------------
## Parser Generation Targets

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

  file(MAKE_DIRECTORY ${PROJECT_BINARY_DIR}/include/minizinc/support/)
  BISON_TARGET(RegExParser
    ${PROJECT_SOURCE_DIR}/lib/support/regex/parser.yxx
    ${PROJECT_BINARY_DIR}/regex_parser.tab.cpp
    DEFINES_FILE ${PROJECT_BINARY_DIR}/include/minizinc/support/regex_parser.tab.hh
    COMPILE_FLAGS "-p regex_yy -l"
  )
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

  include_directories(${PROJECT_SOURCE_DIR}/lib/cached)
  set(BISON_MZNParser_OUTPUTS
    ${PROJECT_SOURCE_DIR}/lib/cached/parser.tab.cpp
    ${PROJECT_SOURCE_DIR}/lib/cached/minizinc/parser.tab.hh
  )
  set(BISON_RegExParser_OUTPUTS
    ${PROJECT_SOURCE_DIR}/lib/cached/regex_parser.tab.cpp
    ${PROJECT_SOURCE_DIR}/lib/cached/minizinc/support/regex_parser.tab.hh
  )
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
    COMPILE_FLAGS "-P regex_yy -L"
  )
  ADD_FLEX_BISON_DEPENDENCY(RegExLexer RegExParser)
else()
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

  set(FLEX_MZNLexer_OUTPUTS ${PROJECT_SOURCE_DIR}/lib/cached/lexer.yy.cpp)
  set(FLEX_RegExLexer_OUTPUTS ${PROJECT_SOURCE_DIR}/lib/cached/regex_lexer.yy.cpp)
endif()

if(NOT (GECODE_FOUND AND USE_GECODE))
  set(FLEX_RegExLexer_OUTPUTS "")
  set(BISON_RegExParser_OUTPUTS "")
endif()

add_library(minizinc_parser OBJECT
  ${BISON_MZNParser_OUTPUTS}
  ${FLEX_MZNLexer_OUTPUTS}
  ${BISON_RegExParser_OUTPUTS}
  ${FLEX_RegExLexer_OUTPUTS}
)

if(GECODE_FOUND AND USE_GECODE)
  target_include_directories(minizinc_parser PRIVATE "${GECODE_INCLUDE_DIRS}")
  target_compile_definitions(minizinc_parser PRIVATE HAS_GECODE)
endif()
