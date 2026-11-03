set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  # _WIN32_WINNT is set for <windows.h> such that the Vista+ process APIs used
  # in include/minizinc/process.hh (STARTUPINFOEXW,
  # PROC_THREAD_ATTRIBUTE_HANDLE_LIST) are available.
  add_definitions(-DNOMINMAX -D_WIN32_WINNT=0x0600)
endif()

option(USE_ADDRESS_SANITIZER "Use GCC Address Sanitizer" OFF)
if(USE_ADDRESS_SANITIZER)
  set(CMAKE_CXX_FLAGS
    "${CMAKE_CXX_FLAGS} -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS
    "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address -fsanitize=undefined -fno-omit-frame-pointer")
endif()
