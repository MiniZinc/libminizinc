# Script to update the cached lexer and parser
# Run after modifying lexer.lxx and/or parser.yxx
#
# 1. generate lexer and parser by building libminizinc
# 2. run cmake -DPROJECT_SOURCE_DIR=. -DPROJECT_BINARY_DIR=build -P md5_gen.cmake
# replacing . and build with the source and binary directory

macro(MD5 filename md5sum)
	file(READ "${filename}" RAW_MD5_FILE)
	string(REGEX REPLACE "\r" "" STRIPPED_MD5_FILE "${RAW_MD5_FILE}")
	string(MD5 ${md5sum} "${STRIPPED_MD5_FILE}")
endmacro(MD5)

file(COPY "${PROJECT_BINARY_DIR}/lexer.yy.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
file(COPY "${PROJECT_BINARY_DIR}/parser.tab.cpp" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/")
file(COPY "${PROJECT_BINARY_DIR}/include/minizinc/parser.tab.hh" DESTINATION "${PROJECT_SOURCE_DIR}/lib/cached/minizinc/")

MD5("${PROJECT_SOURCE_DIR}/lib/lexer.lxx" lexer_lxx_md5)
MD5("${PROJECT_SOURCE_DIR}/lib/parser.yxx" parser_yxx_md5)

set(MD5_TEMPLATE "set(lexer_lxx_md5_cached \"${lexer_lxx_md5}\")")
set(MD5_TEMPLATE "${MD5_TEMPLATE}\nset(parser_yxx_md5_cached \"${parser_yxx_md5}\")")
file(WRITE "${PROJECT_SOURCE_DIR}/lib/cached/md5_cached.cmake" ${MD5_TEMPLATE})
