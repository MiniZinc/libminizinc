# Script to update the cached lexer and parser
# Run after modifying lexer.lxx and/or parser.yxx
#
# 1. generate lexer and parser by building libminizinc
# 2. run cmake -DPROJECT_SOURCE_DIR=. -DPROJECT_BINARY_DIR=build -P md5_gen.cmake
# replacing . and build with the source and binary directory
# add -DFORCE=ON to force the renewing of generated files when md5's still match.

option(FORCE "Force the renewing of generated files" OFF)

macro(MD5 filename md5sum)
	file(READ "${filename}" RAW_MD5_FILE)
	string(REGEX REPLACE "\r" "" STRIPPED_MD5_FILE "${RAW_MD5_FILE}")
	string(MD5 ${md5sum} "${STRIPPED_MD5_FILE}")
endmacro(MD5)

# When updating the cached files, update MD5 sums defined in this file
include(${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake)

MD5("${PROJECT_SOURCE_DIR}/lib/lexer.lxx" lexer_lxx_md5)
MD5("${PROJECT_SOURCE_DIR}/lib/parser.yxx" parser_yxx_md5)
MD5("${PROJECT_SOURCE_DIR}/lib/support/regex/lexer.lxx" regex_lexer_lxx_md5)
MD5("${PROJECT_SOURCE_DIR}/lib/support/regex/parser.yxx" regex_parser_yxx_md5)


if(FORCE OR (NOT "${lexer_lxx_md5}" STREQUAL "${lexer_lxx_md5_cached}"))
  file(COPY "${PROJECT_BINARY_DIR}/lexer.yy.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
endif()
if(FORCE OR (NOT "${parser_yxx_md5}" STREQUAL "${parser_yxx_md5_cached}"))
  file(COPY "${PROJECT_BINARY_DIR}/parser.tab.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
  file(COPY "${PROJECT_BINARY_DIR}/include/minizinc/parser.tab.hh" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/minizinc/")
endif()
if(FORCE OR (NOT "${regex_lexer_lxx_md5}" STREQUAL "${regex_lexer_lxx_md5_cached}"))
  file(COPY "${PROJECT_BINARY_DIR}/regex_lexer.yy.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
endif()
if(FORCE OR (NOT "${regex_parser_yxx_md5}" STREQUAL "${regex_parser_yxx_md5_cached}"))
  file(COPY "${PROJECT_BINARY_DIR}/regex_parser.tab.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
  file(COPY "${PROJECT_BINARY_DIR}/include/minizinc/support/regex_parser.tab.hh" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/minizinc/support/")
endif()

set(MD5_TEMPLATE "set(lexer_lxx_md5_cached \"${lexer_lxx_md5}\")")
set(MD5_TEMPLATE "${MD5_TEMPLATE}\nset(parser_yxx_md5_cached \"${parser_yxx_md5}\")")
set(MD5_TEMPLATE "${MD5_TEMPLATE}\nset(regex_lexer_lxx_md5_cached \"${regex_lexer_lxx_md5}\")")
set(MD5_TEMPLATE "${MD5_TEMPLATE}\nset(regex_parser_yxx_md5_cached \"${regex_parser_yxx_md5}\")")
file(WRITE "${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake" ${MD5_TEMPLATE})
