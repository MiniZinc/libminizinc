set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32)
  # _WIN32_WINNT is set for <Windows.h> such that the Vista+ process APIs used
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

set(CMAKE_REQUIRED_QUIET $<NOT:${VERBOSE}>)

include(CheckCXXCompilerFlag)

check_cxx_compiler_flag(-Werror HAS_WERROR)

if(HAS_WERROR)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
endif()

check_cxx_source_compiles("
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <libproc.h>
#include <unistd.h>

int main (int argc, char* argv[])
{
    pid_t pid = getpid();
    char path[PROC_PIDPATHINFO_MAXSIZE];
    (void) proc_pidpath (pid, path, sizeof(path));
    return 0;
}
" HAS_PIDPATH)

check_cxx_source_compiles("
#include <windows.h>
int main (int argc, char* argv[]) {
  char path[MAX_PATH];
  (void) GetModuleFileName(NULL, path, MAX_PATH);
  return 0;
}" HAS_GETMODULEFILENAME)

check_cxx_source_compiles("
#include <windows.h>
int main (int argc, char* argv[]) {
  (void) GetFileAttributes(NULL);
  return 0;
}" HAS_GETFILEATTRIBUTES)
