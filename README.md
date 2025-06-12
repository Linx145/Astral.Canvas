<p align="center">
  <img src="https://raw.githubusercontent.com/Linx145/Astral.Canvas/master/AstralCanvasLogo.png" width="256"/>
</p>

# Archival Notice
This repository has been superseded by [this project](https://github.com/Clouds-of-Eternity/LGFX/). It's the same thing but cleaner and cooler!!!!

# Astral.Canvas
Astral Canvas is a collection of libraries and frameworks for writing graphical apps with Vulkan and Metal, using either C, C++ or C# (Or through self-written bindings, any language that can call C really, which is all of them).

It can be seen as a port/further development of [Somnium.Framework](https://github.com/Linx145/Somnium/tree/master), and will succeed it in the near future. It uses GLFW as a backend for windowing and input.

## Building
The project is currently being developed and tested on Windows with MSVC, and MacOS with clang/xcodebuild. After cloning the repository, cd into it and run `premake5 <target>`. 

Astral.Shaderc, the executable responsible for generating .shaderobj json files from .shader text input needs several dynamic libraries to compile, primarily spirv-cross libraries, which must be inserted into the respective OS folders in `Astral.Canvas/Astral.Shaderc/dependencies`

An example for using the library's C++ API can be found in the Test folder. Meanwhile, c-examples contains an incomplete list of examples written in the C API.

## Notes
Although it utilises namespaces, class methods and templated types, Astral.Canvas does not use most of the C++ features such as exceptions and inheritance, doing things the C way instead. It also relies on it's own standard library, which may be a put off for some wanting to integrate it into their project. In which case, it may be better to use the C API instead.
