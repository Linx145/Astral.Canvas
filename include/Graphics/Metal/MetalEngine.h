#pragma once
#ifdef ASTRALCANVAS_METAL
#include "Linxc.h"
#include "allocators.hpp"
#include "Windowing/Window.hpp"

#define GLFW_INCLUDE_NONE
#include "GLFW/glfw3.h"
#include "GLFW/glfw3native.h"

bool AstralCanvasMetal_Initialize(IAllocator *allocator, AstralCanvas::Window* window);
void AstralCanvasMetal_BeginDraw();
void AstralCanvasMetal_EndDraw();
#endif
