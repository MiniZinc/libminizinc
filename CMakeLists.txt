cmake_minimum_required(VERSION 3.4.0)

# -------------------------------------------------------------------------------------------------------------------
#  -- Project information and versioning.

project(libminizinc
        VERSION 2.2.3
        LANGUAGES CXX C)

if(NOT BUILD_REF)
  set(BUILD_REF "")
endif()

# -------------------------------------------------------------------------------------------------------------------
#  -- Project build options

# Driver compilation selection
option(USE_CPLEX "Enable the CPLEX solving target" TRUE)
option(USE_GEAS "Enable the Geas solving target" TRUE)
option(USE_GECODE "Enable the Gecode solving target" TRUE)
option(USE_GUROBI "Enable the Gurobi solving target" TRUE)
option(USE_OSICBC "Enable the Osi CBC solving target" TRUE)
option(USE_SCIP "Enable the SCIP solving target" TRUE)
option(USE_XPRESS "Enable the Xpress solving target" TRUE)

# Static vs. Dynamic linking
option(CPLEX_PLUGIN "Build CPLEX binding as a plugin" ON)
option(GUROBI_PLUGIN "Build Gurobi binding as a plugin" ON)

# Enforce non proprietary build
option(USE_PROPRIETARY "Enable static linking of proprietary solvers" OFF)
if(NOT USE_PROPRIETARY)
  set(CPLEX_PLUGIN ON)
  set(GUROBI_PLUGIN ON)
  set(USE_SCIP OFF)
endif()

# Export compile commands (useful when working with visual studio code)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# -------------------------------------------------------------------------------------------------------------------
#  -- CMake initialisation

# Try to find possible dependencies
list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)
include(cmake/support/emscripten_setup.cmake)

find_package(CPlex)
find_package(Geas)
find_package(Gecode 6.0 COMPONENTS Driver Float Int Kernel Minimodel Search Set Support)
find_package(Gurobi)
find_package(OsiCBC)
find_package(SCIP CONFIG)
find_package(Xpress)

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
#  -- CMake configuration generation

include(cmake/support/config_export.cmake)
include(cmake/support/config_output.cmake)

include(cmake/support/config_emscripten.cmake)
