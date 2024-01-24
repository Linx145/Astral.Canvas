#pragma once
#include "Windowing/Window.h"
#include "vector.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "string.hpp"

using namespace collections;
using namespace Maths;

def_delegate(AstralCanvas_Update, void, float);
def_delegate(AstralCanvas_Init, void);
def_delegate(AstralCanvas_Deinit, void);

struct AstralCanvasApplication
{
	vector<AstralCanvasWindow> windows;
	IAllocator* allocator;
	double memoryForBuffersMiB;
	double memoryForImagesMiB;

	string appName;
	string engineName;
	u32 appVersion;
	u32 engineVersion;

	AstralCanvas_Init onInitialize;
	AstralCanvas_Update onUpdate;

	AstralCanvasApplication();
	static AstralCanvasApplication* init(IAllocator* ASTRALCORE_ALLOCATORS, string appName, string engineName, u32 appVersion, u32 engineVersion);
	bool AddWindow(i32 width, i32 height, i32 fps, bool resizeable = true);
	bool FinalizeGraphicsBackend();
	void Run(AstralCanvas_Update updateFunc, AstralCanvas_Init initFunc, AstralCanvas_Deinit deinitFunc);
};

AstralCanvasApplication* AstralCanvas_GetAppInstance();