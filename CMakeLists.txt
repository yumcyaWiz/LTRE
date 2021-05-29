cmake_minimum_required(VERSION 3.16)
project(LTRE LANGUAGES C CXX)

option(BUILD_TESTS "build tests" ON)

set(CMAKE_CXX_STANDARD 17 CACHE STRING "The C++ standard to use")
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# OpenMP
find_package(OpenMP)
if(NOT OpenMP_CXX_FOUND)
  message(WARNING "failed to find OpenMP")
endif()

# LTRE
add_library(LTRE INTERFACE)
target_include_directories(LTRE INTERFACE "include")
target_link_libraries(LTRE INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(LTRE INTERFACE assimp::assimp)
target_link_libraries(LTRE INTERFACE spdlog::spdlog)
target_compile_options(LTRE INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
)

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