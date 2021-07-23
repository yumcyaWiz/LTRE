# LTRE

montecarlo raytracer for my research

## Features

* Unidirectional Path Tracing
* Path Tracing with Next Event Estimation 
* BVH
* Model Loading
* Disney BRDF([Burley 2012](https://www.disneyanimation.com/publications/physically-based-shading-at-disney/))

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

## Gallery

![](img/sponza.png)
![](img/salle_de_bain.png)
![](img/disney_brdf.png)

## Externals

* [stb](https://github.com/nothings/stb)
* [assimp](https://github.com/assimp/assimp)
* [spdlog](https://github.com/gabime/spdlog)
* [GoogleTest](https://github.com/google/googletest)