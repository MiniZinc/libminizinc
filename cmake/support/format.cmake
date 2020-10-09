
if (NOT CLANG_FORMAT_EXECUTABLE)
  find_program(CLANG_FORMAT_EXECUTABLE NAMES clang-format clang-format-11 clang-format-10 clang-format-9)
endif()

set(CLANG_FORMAT_FLAGS "-i" CACHE STRING "Flags passed to the clang-format executable")

if (CLANG_FORMAT_EXECUTABLE)
  file(GLOB_RECURSE FORMAT_FILES
    ${PROJECT_SOURCE_DIR}/*.c
    ${PROJECT_SOURCE_DIR}/*.cpp
    ${PROJECT_SOURCE_DIR}/*.h
    ${PROJECT_SOURCE_DIR}/*.hh
    ${PROJECT_SOURCE_DIR}/*.hpp
  )
  list(FILTER FORMAT_FILES EXCLUDE REGEX ${PROJECT_BINARY_DIR}/*)
  list(FILTER FORMAT_FILES EXCLUDE REGEX ${PROJECT_SOURCE_DIR}/lib/cached/*)
  list(FILTER FORMAT_FILES EXCLUDE REGEX ${PROJECT_SOURCE_DIR}/lib/thirdparty/*)
  list(FILTER FORMAT_FILES EXCLUDE REGEX ${PROJECT_SOURCE_DIR}/include/minizinc/thirdparty/*)

  separate_arguments(CLANG_FORMAT_FLAGS_LIST NATIVE_COMMAND ${CLANG_FORMAT_FLAGS})
  add_custom_target(format
    COMMAND ${CLANG_FORMAT_EXECUTABLE} ${CLANG_FORMAT_FLAGS_LIST} ${FORMAT_FILES}
    COMMENT "Running ${CLANG_FORMAT_EXECUTABLE} on all source files"
  )
endif()

