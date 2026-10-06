cmake_minimum_required(VERSION 3.20)
# -------------------------------------------------------------------------------------------------------------------
#  -- Project information and versioning.

project(libminizinc
        VERSION 2.10.2
        LANGUAGES CXX C)

if(NOT BUILD_REF)
  set(BUILD_REF "")
endif()

# -------------------------------------------------------------------------------------------------------------------
#  -- Project build options

# Static vs. Dynamic linking
option(CPLEX_PLUGIN "Build CPLEX binding as a plugin" ON)
option(HIGHS_PLUGIN "Build HiGHS binding as a plugin" ON)

# Memory: mimalloc allocates AST nodes (downloaded with FetchContent). Off by default for
# WebAssembly and with AddressSanitizer, which replaces malloc itself (also when it is enabled
# through the compiler flags).
if(DEFINED EMSCRIPTEN OR USE_ADDRESS_SANITIZER OR "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_FLAGS_DEBUG}" MATCHES "-fsanitize=[a-z,]*address")
  set(mzn_use_mimalloc_default OFF)
else()
  set(mzn_use_mimalloc_default ON)
endif()
option(MZN_USE_MIMALLOC "Use mimalloc as the memory allocator" ${mzn_use_mimalloc_default})
# The override replaces malloc for the whole process. A static override does not work on Windows.
if(WIN32)
  set(mzn_mimalloc_override_default OFF)
else()
  set(mzn_mimalloc_override_default ON)
endif()
option(MZN_MIMALLOC_OVERRIDE "Let mimalloc replace malloc for the whole process" ${mzn_mimalloc_override_default})
# Debug check: track every AST node and report the ones that are still counted at exit (leaks)
option(MZN_RC_CHECK "Report leaked reference-counted nodes at exit" OFF)

# CMake options default value
option(CMAKE_POSITION_INDEPENDENT_CODE "Default value for POSITION_INDEPENDENT_CODE of targets" TRUE)

# -------------------------------------------------------------------------------------------------------------------
#  -- CMake initialisation

include(GNUInstallDirs)

# Fix library suffixes for Web Assembly platform
include(cmake/support/emscripten_setup.cmake)

# Try to find possible dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/highs/lib/cmake/highs)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/highs/lib64/cmake/highs)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/chuffed/lib/cmake/chuffed)
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_SOURCE_DIR}/vendor/chuffed/lib64/cmake/chuffed)
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif(POLICY CMP0074)
find_package(Geas)
find_package(Gecode 6.2 COMPONENTS Driver Float Int Kernel Minimodel Search Set Support)
find_package(atlantis)
find_package(chuffed)
if(NOT HIGHS_PLUGIN)
  find_package(HIGHS REQUIRED)
endif()
find_package(OsiCBC)
if(NOT CPLEX_PLUGIN)
	find_package(CPlex REQUIRED)
endif()

# Set build type when none is selected
set(DEFAULT_BUILD_TYPE "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${DEFAULT_BUILD_TYPE}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${DEFAULT_BUILD_TYPE}" CACHE STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
               "Debug"
               "Release"
               "MinSizeRel"
               "RelWithDebInfo")
endif()

# -------------------------------------------------------------------------------------------------------------------
#  -- Compiler configuration

include(cmake/support/compiler_setup.cmake)

configure_file(
  ${PROJECT_SOURCE_DIR}/include/minizinc/config.hh.in
  ${PROJECT_BINARY_DIR}/include/minizinc/config.hh
)

install(
  FILES ${PROJECT_BINARY_DIR}/include/minizinc/config.hh
  DESTINATION include/minizinc
)

# -------------------------------------------------------------------------------------------------------------------
#  -- MiniZinc compilation targets.

find_package(Threads REQUIRED)
include_directories(${PROJECT_SOURCE_DIR}/include)
include_directories(${PROJECT_BINARY_DIR}/include)

# Libraries
include(cmake/targets/libmzn.cmake)

# Executables
include(cmake/targets/minizinc.cmake)
include(cmake/targets/mzn2doc.cmake)
include(cmake/targets/parse_diff.cmake)

# -------------------------------------------------------------------------------------------------------------------
#  -- Platform Specific configuration
include(cmake/support/config_emscripten.cmake)

# -------------------------------------------------------------------------------------------------------------------
#  -- CMake configuration generation

include(cmake/support/config_export.cmake)
include(cmake/support/config_output.cmake)

# -------------------------------------------------------------------------------------------------------------------
#  -- Support Actions
include(cmake/support/format.cmake)
