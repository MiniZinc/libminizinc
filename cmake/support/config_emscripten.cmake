if (DEFINED EMSCRIPTEN)
  add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                     COMMAND python3 ${EMSCRIPTEN_ROOT_PATH}/tools/file_packager.py minizinc.data --lz4 --preload ${PROJECT_SOURCE_DIR}/share@/minizinc --from-emcc --js-output=${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                     COMMENT "building data store minizinc.data")

  set(EMSCRIPTEN_CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set(EMSCRIPTEN_LINK_FLAGS " -s FORCE_FILESYSTEM=1 -s LZ4=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['callMain', 'cwrap', 'FS', 'ENV']\" -s DISABLE_EXCEPTION_CATCHING=0 -s ALLOW_MEMORY_GROWTH=1 --no-heap-copy")

  # -------------------------------------------------------------------------------------------------------------------
  #  -- Web Assembly Configuration.

  # MiniZinc main executable
  em_link_pre_js(minizinc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_CXX_FLAGS})
  set_target_properties(minizinc PROPERTIES LINK_FLAGS "-s WASM=1 -s EXPORT_NAME=\"'MINIZINC'\" ${EMSCRIPTEN_LINK_FLAGS}")

  install(FILES ${PROJECT_BINARY_DIR}/minizinc.wasm DESTINATION bin)
  install(FILES ${PROJECT_BINARY_DIR}/minizinc.data DESTINATION bin)

  # mzn2doc executable
  em_link_pre_js(mzn2doc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_CXX_FLAGS})
  set_target_properties(mzn2doc PROPERTIES LINK_FLAGS "-s WASM=1 -s EXPORT_NAME=\"'MZN2DOC'\" ${EMSCRIPTEN_LINK_FLAGS}")
  install(FILES ${PROJECT_BINARY_DIR}/mzn2doc.wasm DESTINATION bin)

  # -------------------------------------------------------------------------------------------------------------------
  #  -- ASM.js (JavaScript) Configuration.

  # MiniZinc main executable
  add_executable(minizinc_asm minizinc.cpp)
  target_link_libraries(minizinc_asm mzn)

  em_link_pre_js(minizinc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc_asm PROPERTIES CXX_FLAGS ${EMSCRIPTEN_CXX_FLAGS})
  set_target_properties(minizinc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORT_NAME=\"'MINIZINC'\" ${EMSCRIPTEN_LINK_FLAGS}")

  install(TARGETS minizinc_asm RUNTIME DESTINATION bin)
  install(FILES $<TARGET_FILE_NAME:minizinc_asm>.mem DESTINATION bin)

  # mzn2doc executable
  add_executable(mzn2doc_asm mzn2doc.cpp)
  target_link_libraries(mzn2doc_asm mzn)

  em_link_pre_js(mzn2doc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc_asm PROPERTIES CXX_FLAGS ${EMSCRIPTEN_CXX_FLAGS})
  set_target_properties(mzn2doc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORT_NAME=\"'MZN2DOC'\" ${EMSCRIPTEN_LINK_FLAGS}")

  install(TARGETS mzn2doc_asm RUNTIME DESTINATION bin)
  install(FILES $<TARGET_FILE_NAME:mzn2doc_asm>.mem DESTINATION bin)
endif()
