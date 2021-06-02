# LTRE

header only montecarlo raytracer for my research

![](img/sponza.png)

## Features

* Unidirectional Path Tracing
* BVH
* Model Loading

## Requirements

* C++20
* CMake 3.20
* OpenMP

## Build

```
mkdir build
cd build
cmake ..
cmake --build .
```

## Externals

* [stb](https://github.com/nothings/stb)
* [assimp](https://github.com/assimp/assimp)
* [spdlog](https://github.com/gabime/spdlog)
* [GoogleTest](https://github.com/google/googletest)