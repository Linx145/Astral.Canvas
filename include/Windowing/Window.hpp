#pragma once
#include "Input/InputState.hpp"
#include "Maths/All.h"
#include "Linxc.h"
#include "option.hpp"

namespace AstralCanvas
{
	struct Window
	{
		GLFWwindow* handle;
		Maths::Point2 resolution;
		Maths::Point2 position;
		InputState windowInputState;

		void *windowSurfaceHandle;

		Window();
		inline void deinit()
		{
			glfwDestroyWindow(handle);
		}
		inline Maths::Rectangle AsRectangle()
		{
			return Maths::Rectangle(0, 0, resolution.X, resolution.Y);
		}
	};

	bool WindowInit(IAllocator *allocator, Window * result, i32 width, i32 height, bool resizeable);
}