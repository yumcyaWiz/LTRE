cmake_minimum_required(VERSION 3.20)
project(LTRE LANGUAGES C CXX)

option(BUILD_TESTS "build tests" ON)

# OpenMP
find_package(OpenMP)
if(NOT OpenMP_CXX_FOUND)
  message(WARNING "failed to find OpenMP")
endif()

# externals
add_subdirectory("externals")

# LTRE
add_library(LTRE INTERFACE)
target_include_directories(LTRE INTERFACE "include")
target_compile_features(LTRE INTERFACE cxx_std_20)
set_target_properties(LTRE PROPERTIES CXX_EXTENSIONS OFF)
target_link_libraries(LTRE INTERFACE OpenMP::OpenMP_CXX)
target_link_libraries(LTRE INTERFACE assimp::assimp)
target_link_libraries(LTRE INTERFACE spdlog::spdlog)
target_link_libraries(LTRE INTERFACE stb)
target_compile_options(LTRE INTERFACE
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic>
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic>
)

# examples
add_subdirectory("examples")

# tests
if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()