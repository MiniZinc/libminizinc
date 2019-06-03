
if (DEFINED EMSCRIPTEN)
  #em_link_pre_js(minizinc "file_packager_patch.js")
  #em_link_pre_js(minizinc "file_packager.js")

  set_target_properties(minizinc PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(minizinc PROPERTIES LINK_FLAGS "-s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()

if (DEFINED EMSCRIPTEN)
  add_executable(minizinc_asm minizinc.cpp)
  target_link_libraries(minizinc_asm minizinc_solver)

  #em_link_pre_js(minizinc_asm "file_packager_patch.js")
  #em_link_pre_js(minizinc_asm "file_packager.js")

  set_target_properties(minizinc_asm PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(minizinc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()

if (DEFINED EMSCRIPTEN)
  #em_link_pre_js(mzn2doc "file_packager_patch.js")
  #em_link_pre_js(mzn2doc "file_packager.js")

  set_target_properties(mzn2doc PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(mzn2doc PROPERTIES LINK_FLAGS "-s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()


if (DEFINED EMSCRIPTEN)
  add_executable(mzn2doc_asm mzn2doc.cpp)
  target_link_libraries(mzn2doc_asm minizinc_solver)

  #em_link_pre_js(mzn2doc_asm "file_packager_patch.js")
  #em_link_pre_js(mzn2doc_asm "file_packager.js")

  set_target_properties(mzn2doc_asm PROPERTIES CXX_FLAGS "-s MINIZ_NO_ARCHIVE_APIS -s MINIZ_NO_ZLIB_APIS")
  set_target_properties(mzn2doc_asm PROPERTIES LINK_FLAGS "-s WASM=0 -s FORCE_FILESYSTEM=1 -s MODULARIZE=1 -s EXTRA_EXPORTED_RUNTIME_METHODS=\"['cwrap', 'FS']\"")
endif()
