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
#  -- CMake initialisation

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules)

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
include(cmake/targets/libminizinc.cmake)

include(cmake/targets/libminizinc_cplex.cmake)
include(cmake/targets/libminizinc_gecode.cmake)
include(cmake/targets/libminizinc_gurobi.cmake)
include(cmake/targets/libminizinc_osicbc.cmake)
include(cmake/targets/libminizinc_xpress.cmake)

include(cmake/targets/libminizinc_fzn.cmake)

include(cmake/targets/libminizinc_solver.cmake)

# Executables
include(cmake/targets/minizinc.cmake)
include(cmake/targets/mzn2doc.cmake)

# -------------------------------------------------------------------------------------------------------------------
#  -- CMake configuration generation

include(cmake/support/config_export.cmake)