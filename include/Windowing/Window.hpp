#pragma once
#include "GLFW/glfw3.h"
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
	
	void *windowSurfaceHandle;

	AstralCanvasWindow();
	static option<AstralCanvasWindow> init(i32 width, i32 height, i32 fps, bool resizeable = true);
	inline void deinit()
	{
		glfwDestroyWindow(handle);
	}
};

#ifdef MACOS
CAMetalLayer AstralCanvasWindow_GetMetalLayer(AstralCanvasWindow* window);
#endif