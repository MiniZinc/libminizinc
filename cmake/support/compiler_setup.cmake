set(CMAKE_CXX_STANDARD 11)

option(USE_ADDRESS_SANITIZER "Use GCC Address Sanitizer" OFF)
if(USE_ADDRESS_SANITIZER)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fsanitize=address -fno-omit-frame-pointer")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -fsanitize=address")
endif()

if(APPLE)
  execute_process(COMMAND xcrun --show-sdk-path OUTPUT_VARIABLE OSX_SYSROOT OUTPUT_STRIP_TRAILING_WHITESPACE)
  set(CMAKE_OSX_SYSROOT ${OSX_SYSROOT})
  set(CMAKE_OSX_DEPLOYMENT_TARGET "10.9" CACHE STRING "Minimum OS X deployment version")
endif(APPLE)

set(CMAKE_REQUIRED_QUIET $<NOT:${VERBOSE}>)

include(CheckCXXCompilerFlag)

set(SAFE_CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS}")
check_cxx_compiler_flag(-Werror HAS_WERROR)

if(HAS_WERROR)
  set(CMAKE_REQUIRED_FLAGS "${CMAKE_REQUIRED_FLAGS} -Werror")
endif()

check_cxx_source_compiles("int main(void) { static __thread int x; (void)x; return 0;}" HAS_ATTR_THREAD)

if(NOT HAS_ATTR_THREAD)
  check_cxx_source_compiles("int main(void) { __declspec(thread) static int x; (void)x; return 0;}" HAS_DECLSPEC_THREAD)
endif()

check_cxx_source_compiles("#include <cstdlib>
int main(void) { long long int x = atoll(\"123\"); (void)x; }" HAS_ATOLL)
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

check_cxx_source_compiles("
#include <string.h>
int main (int argc, char* argv[]) {
  (void) memcpy_s(NULL,0,NULL,0);
  return 0;
}" HAS_MEMCPY_S)