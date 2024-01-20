#pragma once
#include "Windowing/Window.h"
#include "vector.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "GraphicsBackend.h"
#include "string.hpp"

using namespace collections;
using namespace Maths;

def_delegate(AstralCanvas_Update, void, float);
def_delegate(AstralCanvas_Init, void);

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

	AstralGraphicsBackend graphicsBackend;

	AstralCanvasApplication();
	//AstralCanvasApplication(IAllocator* allocator, AstralGraphicsBackend graphicsBackendToUse);
	static AstralCanvasApplication* init(IAllocator* allocator, AstralGraphicsBackend graphicsBackendToUse, string appName, string engineName, u32 appVersion, u32 engineVersion);
	bool AddWindow(i32 width, i32 height, i32 fps, bool resizeable = true);
	bool FinalizeGraphicsBackend();
	void Run(AstralCanvas_Update updateFunc, AstralCanvas_Init initFunc);
};

AstralCanvasApplication* AstralCanvas_GetAppInstance();