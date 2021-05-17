cmake_minimum_required(VERSION 3.16)
project(LTRE LANGUAGES C CXX)

option(BUILD_TESTS "build tests" ON)

set(CMAKE_CXX_STANDARD 20 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# LTRE
add_library(LTRE INTERFACE)
target_include_directories(LTRE INTERFACE "include")

# externals
add_subdirectory("externals")

# examples
add_subdirectory("examples")

# tests
if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()