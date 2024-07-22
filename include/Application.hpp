#pragma once
#include "Windowing/Window.hpp"
#include "vector.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "string.hpp"
#include "Graphics/Graphics.hpp"

namespace AstralCanvas
{
	def_delegate(ApplicationUpdateFunction, void, float);
	def_delegate(ApplicationInitFunction, void);
	def_delegate(ApplicationDeinitFunction, void);

	struct Application
	{
		collections::vector<Window> windows;
		IAllocator allocator;
		AstralCanvas::Graphics graphicsDevice;

		string appName;
		string engineName;
		u32 appVersion;
		u32 engineVersion;
		float startTime;
		float endTime;
		bool shouldResetDeltaTimer;

		float framesPerSecond;

		Application();
		Application* init(IAllocator allocators, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
		bool AddWindow(const char *name, i32 width, i32 height, bool resizeable = true, void *iconData = NULL, u32 iconWidth = 0, u32 iconHeight = 0);
		bool FinalizeGraphicsBackend();
		void Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc);
		void ResetDeltaTimer();
	};

	Application* ApplicationInit(IAllocator ASTRALCORE_ALLOCATORS, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
	Application* GetAppInstance();
	const char *GetClipboardText();
	void SetClipboardText(const char *text);
}