#pragma once
#include "Linxc.h"
#include "Window.h"

extern "C"
{
    typedef void *AstralCanvasApplication;

    const char *AstralCanvasApplication_GetApplicationName(AstralCanvasApplication ptr);

    const char *AstralCanvasApplication_GetEngineName(AstralCanvasApplication ptr);

    float AstralCanvasApplication_GetFramesPerSecond(AstralCanvasApplication ptr);

    void AstralCanvasApplication_SetFramesPerSecond(AstralCanvasApplication ptr, float frames);

    void AstralCanvasApplication_AddWindow(AstralCanvasApplication ptr, i32 width, i32 height, bool resizeable);

    AstralCanvasWindow AstralCanvasApplication_GetWindow(AstralCanvasApplication ptr, usize index);

    AstralCanvasApplication AstralCanvasApplication_Init(const char *appName, const char *engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
}