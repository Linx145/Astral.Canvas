<p align="center">
  <img src="https://raw.githubusercontent.com/Linx145/Astral.Canvas/master/AstralCanvasLogo.png" width="256"/>
</p>

# Astral.Canvas
Astral Canvas is a collection of libraries and frameworks for writing graphical apps using C++, Vulkan and Metal, with a C interface coming soon. It can be seen as a port/further development of [Somnium.Framework](https://github.com/Linx145/Somnium/tree/master), and will succeed it in the near future. It uses GLFW as a backend for windowing and input.

## Building
The project is currently being developed and tested on Windows with MSVC, and MacOS with clang. After cloning the repository, cd into it and run `premake5 <target>`. 

The project needs several dynamic libraries to compile, primarily spirv-cross libraries, which must be inserted into the respective OS folders in `Astral.Canvas/Astral.Shaderc/dependencies` This is only needed for compiling Astral.Shaderc

An example for using the library can be found in the Test folder. In Program.cpp, change `#define INSTANCE_COUNT` to any number you like. If Instance_Count is 1, you can move the creature around with WASD keys.

## Notes
Although it utilises namespaces, class methods and templated types, Astral.Canvas does not use most of the C++ features such as exceptions and inheritance, doing things the C way instead. It also relies on it's own standard library, which may be a put off for some wanting to integrate it into their project. In which case, it may be better to use the upcoming C API instead.

Other projects under the repository include:
* Astral.Core: Astral.Canvas' standard library
* Astral.Reflect: A C-style language tokenizer and (in the future) reflector. Will probably be moved to it's own repo and gitmodule'd thereafter
* Astral.Shaderc: Wraps glslang and spirv-cross to allow parsing Astral.Canvas shader files, an example of which can be seen in the Test folder
* Astral.Json: Json tokenizer and parser, inspired by System.Text.Json