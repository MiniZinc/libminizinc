#### Differential test harness for the bison and tree-sitter parsers
option(BUILD_PARSE_DIFF "Build the bison/tree-sitter differential parse harness" OFF)
if(BUILD_PARSE_DIFF)
  add_executable(parse_diff tests/parse_diff.cpp)
  target_link_libraries(parse_diff mzn)
endif()
