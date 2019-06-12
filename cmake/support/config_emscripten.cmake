
if (DEFINED EMSCRIPTEN)
add_custom_command(OUTPUT ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                  COMMAND python ${EMSCRIPTEN_ROOT_PATH}/tools/file_packager.py minizinc.data --preload ${PROJECT_SOURCE_DIR}/share@/minizinc --from-emcc --js-output=${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js
                  COMMENT "building data store minizinc.data")
endif()

set(EMSCRIPTEN_MINZINC_CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
set(EMSCRIPTEN_MINZINC_COMMON_LINK_FLAGS " -s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS', 'ENV']\"")
set(EMSCRIPTEN_MINZINC_LINK_FLAGS "-s EXPORT_NAME=\"'MINIZINC'\" ${EMSCRIPTEN_MINZINC_COMMON_LINK_FLAGS}")

# WASM version
if (DEFINED EMSCRIPTEN)
  em_link_pre_js(minizinc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(minizinc PROPERTIES LINK_FLAGS ${EMSCRIPTEN_MINZINC_LINK_FLAGS})
endif()

# WASM version with exception catching
if (DEFINED EMSCRIPTEN)
  add_executable(minizinc_exc minizinc.cpp)
  target_link_libraries(minizinc_exc minizinc_solver)

  em_link_pre_js(minizinc_exc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc_exc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc_exc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(minizinc_exc PROPERTIES LINK_FLAGS "-s DISABLE_EXCEPTION_CATCHING=0 ${EMSCRIPTEN_MINZINC_LINK_FLAGS}")
endif()

# ASM.js version
if (DEFINED EMSCRIPTEN)
  add_executable(minizinc_asm minizinc.cpp)
  target_link_libraries(minizinc_asm minizinc_solver)

  em_link_pre_js(minizinc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc_asm PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(minizinc_asm PROPERTIES LINK_FLAGS "-s WASM=0 ${EMSCRIPTEN_MINZINC_LINK_FLAGS}")
endif()

# ASM.js version with exception catching
if (DEFINED EMSCRIPTEN)
  add_executable(minizinc_asm_exc minizinc.cpp)
  target_link_libraries(minizinc_asm_exc minizinc_solver)

  em_link_pre_js(minizinc_asm_exc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(minizinc_asm_exc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(minizinc_asm_exc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(minizinc_asm_exc PROPERTIES LINK_FLAGS "-s WASM=0 -s DISABLE_EXCEPTION_CATCHING=0 ${EMSCRIPTEN_MINZINC_LINK_FLAGS}")
endif()

# Emscripten Interpreter version
# if (DEFINED EMSCRIPTEN)
#   add_executable(minizinc_in minizinc.cpp)
#   target_link_libraries(minizinc_in minizinc_solver)

#   em_link_pre_js(minizinc_in ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
#   em_link_pre_js(minizinc_in ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

#   set_target_properties(minizinc_in PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
#   set_target_properties(minizinc_in PROPERTIES LINK_FLAGS "-s SWAPPABLE_ASM_MODULE -s EMTERPRETIFY=1 -s EMTERPRETIFY_FILE=minizinc.bin ${EMSCRIPTEN_MINZINC_LINK_FLAGS}")

# endif()

if (DEFINED EMSCRIPTEN)
  em_link_pre_js(mzn2doc ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(mzn2doc PROPERTIES LINK_FLAGS "-s EXPORT_NAME=\"'MZN2DOC'\" ${EMSCRIPTEN_MINZINC_COMMON_LINK_FLAGS}")
endif()


if (DEFINED EMSCRIPTEN)
  add_executable(mzn2doc_asm mzn2doc.cpp)
  target_link_libraries(mzn2doc_asm minizinc_solver)

  em_link_pre_js(mzn2doc_asm ${PROJECT_SOURCE_DIR}/cmake/support/emscripten_file_packager_patch.js)
  em_link_pre_js(mzn2doc_asm ${PROJECT_BINARY_DIR}/CMakeFiles/file_packager.js)

  set_target_properties(mzn2doc_asm PROPERTIES CXX_FLAGS ${EMSCRIPTEN_MINZINC_CXX_FLAGS})
  set_target_properties(mzn2doc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s EXPORT_NAME=\"'MZN2DOC'\" ${EMSCRIPTEN_MINZINC_COMMON_LINK_FLAGS}")
endif()
