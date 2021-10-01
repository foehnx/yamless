cmake_minimum_required(VERSION 2.8.12...3.13)
project(yamless VERSION 0.0.1)
set(YAMLESS_INCLUDE_DIR include REALPATH)
include_directories("include")

################################################################################
# Options
################################################################################

option(YAMLESS_QUIET "Suppress configuration output from efforless" ON)
option(YAMLESS_TESTS "Building the tests" OFF)
option(YAMLESS_DEBUG "Enable all debug logging" OFF)
option(YAMLESS_EIGEN "Build with Eigen interface" OFF)

set(YAMLESS_EIGEN_ALTERNATIVE "" CACHE STRING "Path to alternative Eigen")

if(YAMLESS_EIGEN)
  find_package(Eigen3 3.3.4 QUIET)
  if (NOT YAMLESS_EIGEN_ALTERNATIVE STREQUAL "")
    set(EIGEN3_INCLUDE_DIRS ${EIGEN_ALTERNATIVE})
    add_definitions(-DYAMLESS_EIGEN)
  elseif (EIGEN3_FOUND)
    if(NOT YAMLESS_QUIET)
      message(STATUS "Using system provided Eigen.")
    endif()
    add_definitions(-DYAMLESS_EIGEN)
  else()
    message(ERROR "No sufficient Eigen version (3.3.4) found.")
  endif()
endif()

# DebugLogging
if(YAMLESS_DEBUG)
  if(NOT YAMLESS_QUIET)
    message(STATUS "Enable Debug Logs!")
  endif()
  add_definitions(-DDEBUG_LOG)
endif()

# Change to minimal catkin build
if(DEFINED CATKIN_DEVEL_PREFIX)
  include(cmake/catkin.cmake)
  return()
endif()

# Build Tests
if(NOT YAMLESS_TESTS)
  return()
else()
  include(cmake/catch2.cmake)
endif()

################################################################################
# Setup Optional Compilation for Tests
################################################################################

# Check for ccache
if(NOT DEFINED CATKIN_DEVEL_PREFIX)
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE "${CCACHE_PROGRAM}")
endif()
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD 17)
set(CMAKE_CXX_ARCH_FLAGS " -march=native")

add_definitions(-std=c++17)
add_definitions(-Ofast)
add_definitions(-fno-finite-math-only)
add_definitions(-Werror)
add_definitions(-Wextra)
add_definitions(-Wall)
add_definitions(-Wpedantic)
add_definitions(-Wunused)
add_definitions(-Wundef)
add_definitions(-Wconversion)
add_definitions(-Wdouble-promotion)
add_definitions(-Wcast-align)
add_definitions(-Wmissing-declarations)
add_definitions(-Wmissing-include-dirs)
add_definitions(-Wnon-virtual-dtor)
add_definitions(-Wredundant-decls)
add_definitions(-Wodr)
add_definitions(-Wunreachable-code)

set(CMAKE_EXPORT_COMPILE_COMMANDS On)

# Compiler Specific
set(YAMLESS_COMPILER_LIBRARIES)
if (CMAKE_CXX_COMPILER_ID MATCHES "^(Apple)?Clang$")
  add_definitions(-Wno-shorten-64-to-32)
  add_definitions(-Wno-sign-conversion)
  add_definitions(-Wno-implicit-int-float-conversion)
  add_definitions(-Wno-c99-extensions)
  add_definitions(-Wno-implicit-int-conversion)
else()
  add_definitions(-Wno-deprecated-copy)
  set(YAMLESS_COMPILER_LIBRARIES 'stdc++fs')
endif()

# Set default build type
if (NOT EXISTS ${CMAKE_BINARY_DIR}/CMakeCache.txt)
  if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "" FORCE)
  endif()
endif()

################################################################################
# Specify Build Resources
################################################################################

file(GLOB_RECURSE YAMLESS_EXAMPLES
  examples/*.cpp
)

################################################################################
# Setup Build
################################################################################

# Build examples
enable_testing()
add_executable(example_tests ${YAMLESS_EXAMPLES})

target_link_libraries(example_tests PRIVATE
  Catch2::Catch2
  ${YAMLESS_COMPILER_LIBRARIES}
)
if(EIGEN3_FOUND OR NOT YAMLESS_EIGEN_ALTERNATIVE STREQUAL "")
  target_include_directories(example_tests PRIVATE ${EIGEN3_INCLUDE_DIRS})
endif()
add_test(example_tests example_tests)
