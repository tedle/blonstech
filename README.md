# blonstech
**blonstech** is a hobby-grade rendering engine, made mostly from scratch in C++. It exists more as a personal learning project and is not intended for general use.

![blonstech](https://user-images.githubusercontent.com/810467/66870775-0c5b1600-ef57-11e9-9171-2dce5f273d9b.png)

## Features
* Physically based deferred rendering engine
  - Realtime global illumination using precomputed radiance transfer, generated quickly at runtime
  - Probe based environment mapping for reflections
  - Filmic tonemapping
  - Variance shadowmapping
* Simple API built around scoped resource management
* GUI with smooth animations and post processing
* Console commands with simple API built around C++ templated callbacks

## Requirements
* OpenGL 4.3+
* Visual C++ 2019 runtime

## Building from source
Requires Visual Studio 2019. Simply open solution file and build.

## Binary release
See [GitHub releases](https://github.com/tedle/blonstech/releases) page.

## API Documentation
Can be built locally with Doxygen.
