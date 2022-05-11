cmake_minimum_required(VERSION 3.4.0)
# -------------------------------------------------------------------------------------------------------------------
#  -- Project information and versioning.

project(libminizinc
        VERSION 2.6.4
        LANGUAGES CXX C)

if(NOT BUILD_REF)
  set(BUILD_REF "")
endif()

# -------------------------------------------------------------------------------------------------------------------
#  -- Project build options

# Static vs. Dynamic linking
option(CPLEX_PLUGIN "Build CPLEX binding as a plugin" ON)

# CMake options default value
option(CMAKE_POSITION_INDEPENDENT_CODE "Default value for POSITION_INDEPENDENT_CODE of targets" TRUE)

# -------------------------------------------------------------------------------------------------------------------
#  -- CMake initialisation

include(GNUInstallDirs)

# Fix library suffixes for Web Assembly platform
include(cmake/support/emscripten_setup.cmake)

# Try to find possible dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
if(POLICY CMP0074)
  cmake_policy(SET CMP0074 NEW)
endif(POLICY CMP0074)
find_package(Geas)
find_package(Gecode 6.2 COMPONENTS Driver Float Int Kernel Minimodel Search Set Support)
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

include(cmake/support/ccache_setup.cmake)
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
