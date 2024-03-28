#include "Astral.Canvas/Application.h"
#include "Application.hpp"

exportC 
const char *AstralCanvasApplication_GetApplicationName(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->appName.buffer;
}
exportC 
const char *AstralCanvasApplication_GetEngineName(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->engineName.buffer;
}
exportC 
float AstralCanvasApplication_GetFramesPerSecond(AstralCanvasApplication ptr)
{
    return ((AstralCanvas::Application *)ptr)->framesPerSecond;
}
exportC 
void AstralCanvasApplication_SetFramesPerSecond(AstralCanvasApplication ptr, float frames)
{
    ((AstralCanvas::Application *)ptr)->framesPerSecond = frames;
}
exportC 
void AstralCanvasApplication_AddWindow(AstralCanvasApplication ptr, const char *name, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight)
{
    ((AstralCanvas::Application *)ptr)->AddWindow(name, width, height, resizeable, iconData, iconWidth, iconHeight);
}
exportC 
AstralCanvasApplication AstralCanvasApplication_Init(const char *appName, const char *engineName, u32 appVersion, u32 engineVersion, float framesPerSecond)
{
    AstralCanvas::ApplicationInit(GetCAllocator(), string(GetCAllocator(), appName), string(GetCAllocator(), engineName), appVersion, engineVersion, framesPerSecond);

    return (AstralCanvasApplication)AstralCanvas::GetAppInstance();
}
exportC AstralCanvasWindow AstralCanvasApplication_GetWindow(AstralCanvasApplication ptr, usize index)
{
    return (AstralCanvasWindow)((AstralCanvas::Application *)ptr)->windows.Get(index);
}

exportC void AstralCanvasApplication_Run(AstralCanvasApplication app, AstralCanvasUpdateFunction updateFunc, AstralCanvasUpdateFunction drawFunc, AstralCanvasUpdateFunction postEndDrawFunc, AstralCanvasInitFunction initFunc, AstralCanvasDeinitFunction deinitFunc)
{
    ((AstralCanvas::Application *)app)->Run(updateFunc, drawFunc, postEndDrawFunc, initFunc, deinitFunc);
}
exportC AstralCanvasGraphics AstralCanvasApplication_GetGraphicsDevice(AstralCanvasApplication ptr)
{
    return (AstralCanvasGraphics)&((AstralCanvas::Application *)ptr)->graphicsDevice;
}
exportC AstralCanvasApplication AstralCanvasApplication_GetInstance()
{
    return (AstralCanvasApplication)AstralCanvas::GetAppInstance();
}
exportC void AstralCanvasApplication_ResetDeltaTimer(AstralCanvasApplication ptr)
{
    ((AstralCanvas::Application *)ptr)->ResetDeltaTimer();
}