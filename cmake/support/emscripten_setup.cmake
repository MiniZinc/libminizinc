# Workaround for bug in emscripten cmake:
# add .bc (and on macOS .dylib) as library suffixes
if (DEFINED EMSCRIPTEN)
  list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".bc")
  if (CMAKE_HOST_APPLE)
      list(APPEND CMAKE_FIND_LIBRARY_SUFFIXES ".dylib")
  endif()
  set(MZN_STATIC_STDLIB_DIR "/usr/share/minizinc")
  set(HIGHS_PLUGIN OFF)
endif()
