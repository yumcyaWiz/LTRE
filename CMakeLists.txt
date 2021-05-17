cmake_minimum_required(VERSION 3.16)
project(LTRE LANGUAGES C CXX)

option(BUILD_TESTS "build tests" ON)

add_library(LTRE INTERFACE)
target_compile_features(LTRE INTERFACE cxx_std_20)
target_include_directories(LTRE INTERFACE "include")

# externals
add_subdirectory("externals")

# examples
add_subdirectory("examples")

# tests
if(BUILD_TESTS)
endif()