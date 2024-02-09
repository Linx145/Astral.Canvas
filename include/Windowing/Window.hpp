#pragma once
#include "Input/InputState.hpp"
#include "Maths/All.h"
#include "Linxc.h"
#include "option.hpp"
#include "string.hpp"

namespace AstralCanvas
{
	struct Window
	{
		void* handle;
		Maths::Point2 resolution;
		Maths::Point2 position;
		InputState windowInputState;
		string windowTitle;

		void *windowSurfaceHandle;

		Window();
		void deinit();
		inline Maths::Rectangle AsRectangle()
		{
			return Maths::Rectangle(0, 0, resolution.X, resolution.Y);
		}
		void SetWindowTitle(string title);
	};

	bool WindowInit(IAllocator *allocator, Window * result, i32 width, i32 height, bool resizeable);
}