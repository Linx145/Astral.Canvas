#pragma once
#include "Input/InputState.hpp"
#include "Maths/All.h"
#include "Linxc.h"
#include "option.hpp"

#ifdef MACOS
#include <Foundation/Foundation.h>
#include <QuartzCore/CAMetalLayer.h>
#endif

struct AstralCanvasWindow
{
	static bool windowLibraryInitialized;
	GLFWwindow* handle;
	Maths::Point2 resolution;
	Maths::Point2 position;
	AstralCanvas::InputState windowInputState;

	void *windowSurfaceHandle;

	AstralCanvasWindow();
	inline void deinit()
	{
		glfwDestroyWindow(handle);
	}
	inline Maths::Rectangle AsRectangle()
	{
		return Maths::Rectangle(0, 0, resolution.X, resolution.Y);
	}
};

bool AstralCanvasWindow_Init(IAllocator *allocator, AstralCanvasWindow * result, i32 width, i32 height, i32 fps, bool resizeable);

#ifdef MACOS
CAMetalLayer AstralCanvasWindow_GetMetalLayer(AstralCanvasWindow* window);
#endif