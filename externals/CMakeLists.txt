# assimp
SET(ASSIMP_BUILD_ASSIMP_TOOLS OFF CACHE BOOL "" FORCE)
SET(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
SET(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(assimp)

# spdlog
SET(SPDLOG_MASTER_PROJECT ON CACHE BOOL "" FORCE)
add_subdirectory(spdlog)

# stb
add_library(stb INTERFACE)
target_include_directories(stb SYSTEM INTERFACE stb)