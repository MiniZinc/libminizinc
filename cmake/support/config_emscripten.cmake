
if (DEFINED EMSCRIPTEN)
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                  COMMAND python ${EMSCRIPTEN_ROOT_PATH}/tools/file_packager.py minizinc.data --preload ${PROJECT_SOURCE_DIR}/share@/minizinc --from-emcc --js-output=${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                  COMMENT "building data store minizinc.data")
endif()

if (DEFINED EMSCRIPTEN)
  em_link_pre_js(minizinc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  #USE_PTHREADS USE_ZLIB 

  set_target_properties(minizinc PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(minizinc PROPERTIES LINK_FLAGS "-s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()

if (DEFINED EMSCRIPTEN)
  add_executable(minizinc_asm minizinc.cpp)
  target_link_libraries(minizinc_asm minizinc_solver)

  em_link_pre_js(minizinc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc_asm PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(minizinc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()

if (DEFINED EMSCRIPTEN)
  em_link_pre_js(mzn2doc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(mzn2doc PROPERTIES LINK_FLAGS "-s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()


if (DEFINED EMSCRIPTEN)
  add_executable(mzn2doc_asm mzn2doc.cpp)
  target_link_libraries(mzn2doc_asm minizinc_solver)

  em_link_pre_js(mzn2doc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc_asm PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(mzn2doc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()
