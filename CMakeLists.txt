cmake_minimum_required(VERSION 3.20)
project(LTRE LANGUAGES C CXX)

option(BUILD_TESTS "build tests" OFF)

# OpenMP
find_package(OpenMP)
if(NOT OpenMP_CXX_FOUND)
  message(WARNING "failed to find OpenMP")
endif()

# externals
add_subdirectory("externals")

# LTRE
add_library(LTRE STATIC)
target_compile_features(LTRE PUBLIC cxx_std_20)
set_target_properties(LTRE PROPERTIES CXX_EXTENSIONS OFF)
target_include_directories(LTRE PUBLIC "include")
target_sources(LTRE PUBLIC
  "src/bsdf/bsdf.cpp"
  "src/bsdf/bxdf/bxdf.cpp"
  "src/bsdf/bxdf/diffuse.cpp"
  "src/bsdf/bxdf/microfacet.cpp"
  "src/bsdf/bxdf/disney.cpp"
  "src/camera/camera.cpp"
  "src/camera/pinhole-camera.cpp"
  "src/camera/thin-lens.cpp"
  "src/core/aabb.cpp"
  "src/core/material.cpp"
  "src/core/model.cpp"
  "src/core/io.cpp"
  "src/core/primitive.cpp"
  "src/core/ray.cpp"
  "src/core/renderer.cpp"
  "src/core/scene.cpp"
  "src/integrator/ao.cpp"
  "src/integrator/pt.cpp"
  "src/integrator/nee.cpp"
  "src/light/area-light.cpp"
  "src/light/sky/ibl.cpp"
  "src/light/sky/uniform-sky.cpp"
)
target_link_libraries(LTRE PUBLIC OpenMP::OpenMP_CXX)
target_link_libraries(LTRE PUBLIC assimp::assimp)
target_link_libraries(LTRE PUBLIC spdlog::spdlog)
target_link_libraries(LTRE PUBLIC stb)
target_compile_options(LTRE PUBLIC
  $<$<CXX_COMPILER_ID:MSVC>:/W4>
  $<$<CXX_COMPILER_ID:GNU>:-Wall -Wextra -pedantic -march=native
    $<$<CONFIG:DEBUG>: -O0 -g>
    $<$<CONFIG:Release>: -O3>
  >
  $<$<CXX_COMPILER_ID:Clang>:-Wall -Wextra -pedantic
    $<$<CONFIG:DEBUG>: -O0 -g>
    $<$<CONFIG:Release>: -O3>
  >
)

# examples
add_subdirectory("examples")

# tests
if(BUILD_TESTS)
  enable_testing()
  include(GoogleTest)
  add_subdirectory(tests)
endif()