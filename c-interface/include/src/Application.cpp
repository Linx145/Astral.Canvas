#include "Astral.Canvas/Application.h"
#include "Application.hpp"

exportC const char *AstralCanvasApplication_GetApplicationName(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->appName.buffer;
}
exportC const char *AstralCanvasApplication_GetEngineName(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->engineName.buffer;
}
exportC float AstralCanvasApplication_GetFramesPerSecond(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->framesPerSecond;
}
exportC void AstralCanvasApplication_SetFramesPerSecond(AstralCanvasApplication ptr, float frames)
{
    ((AstralCanvas::Application *)ptr)->framesPerSecond = frames;
}
exportC void AstralCanvasApplication_AddWindow(AstralCanvasApplication ptr, i32 width, i32 height, bool resizeable)
{
    ((AstralCanvas::Application *)ptr)->AddWindow(width, height, resizeable);
}
exportC AstralCanvasApplication AstralCanvasApplication_Init(const char *appName, const char *engineName, u32 appVersion, u32 engineVersion, float framesPerSecond)
{
    SetDefaultAllocator(GetCAllocator());

    AstralCanvas::ApplicationInit(GetDefaultAllocator(), string(GetDefaultAllocator(), appName), string(GetDefaultAllocator(), engineName), appVersion, engineVersion, framesPerSecond);

    return (AstralCanvasApplication)AstralCanvas::GetAppInstance();
}
exportC AstralCanvasWindow AstralCanvasApplication_GetWindow(AstralCanvasApplication ptr, usize index)
{
    return (AstralCanvasWindow)((AstralCanvas::Application *)ptr)->windows.Get(index);
}