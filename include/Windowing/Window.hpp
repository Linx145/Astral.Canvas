#pragma once
#include "Input/InputState.hpp"
#include "Maths/All.h"
#include "Linxc.h"
#include "option.hpp"
#include "string.hpp"

namespace AstralCanvas
{
	def_delegate(WindowOnTextInputFunction, void, void *window, u32 characterUnicode);
	def_delegate(WindowOnKeyInteractedFunction, void, void *window, AstralCanvas::Keys key, i32 action);
	struct Window
	{
		void* handle;
		Maths::Point2 resolution;
		Maths::Point2 position;
		InputState windowInputState;
		string windowTitle;
		bool isFullscreen;

		void *windowSurfaceHandle;

		WindowOnTextInputFunction onTextInputFunc;
		WindowOnKeyInteractedFunction onKeyInteractFunc;

		Window();
		void deinit();
		inline Maths::Rectangle AsRectangle()
		{
			return Maths::Rectangle(0, 0, resolution.X, resolution.Y);
		}
		void SetWindowTitle(string title);
		void SetFullscreen(bool value);
		void CloseWindow();
	};

	bool WindowInit(IAllocator *allocator, const char *name, Window * result, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight);
}