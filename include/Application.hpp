#pragma once
#include "Windowing/Window.hpp"
#include "vector.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "string.hpp"
#include "Graphics/Graphics.hpp"

def_delegate(AstralCanvas_Update, void, float);
def_delegate(AstralCanvas_Init, void);
def_delegate(AstralCanvas_Deinit, void);

namespace AstralCanvas
{
	struct Application
	{
		collections::vector<Window> windows;
		IAllocator* allocator;
		AstralCanvas::Graphics graphicsDevice;

		string appName;
		string engineName;
		u32 appVersion;
		u32 engineVersion;

		AstralCanvas_Init onInitialize;
		AstralCanvas_Update onUpdate;

		float framesPerSecond;

		Application();
		Application* init(IAllocator* ASTRALCORE_ALLOCATORS, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
		bool AddWindow(i32 width, i32 height, i32 fps, bool resizeable = true);
		bool FinalizeGraphicsBackend();
		void Run(AstralCanvas_Update updateFunc, AstralCanvas_Update drawFunc, AstralCanvas_Init initFunc, AstralCanvas_Deinit deinitFunc);
	};

	Application* ApplicationInit(IAllocator* ASTRALCORE_ALLOCATORS, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
	Application* GetAppInstance();
}