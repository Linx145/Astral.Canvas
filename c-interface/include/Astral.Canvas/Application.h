#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Window.h"
#include "Astral.Canvas/Graphics/Graphics.h"

#ifdef __cplusplus
extern "C"
{
#endif
	def_delegate(AstralCanvasUpdateFunction, void, float);
	def_delegate(AstralCanvasInitFunction, void);
	def_delegate(AstralCanvasDeinitFunction, void);

    typedef void *AstralCanvasApplication;

    DynamicFunction void AstralCanvasApplication_ResetDeltaTimer(AstralCanvasApplication ptr);
    DynamicFunction const char *AstralCanvasApplication_GetApplicationName(AstralCanvasApplication ptr);
    DynamicFunction const char *AstralCanvasApplication_GetEngineName(AstralCanvasApplication ptr);
    DynamicFunction float AstralCanvasApplication_GetFramesPerSecond(AstralCanvasApplication ptr);
    DynamicFunction void AstralCanvasApplication_SetFramesPerSecond(AstralCanvasApplication ptr, float frames);
    DynamicFunction void AstralCanvasApplication_AddWindow(AstralCanvasApplication ptr, const char *name, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight);
    DynamicFunction AstralCanvasWindow AstralCanvasApplication_GetWindow(AstralCanvasApplication ptr, usize index);
    DynamicFunction AstralCanvasApplication AstralCanvasApplication_Init(const char *appName, const char *engineName, u32 appVersion, u32 engineVersion, float framesPerSecond);
    DynamicFunction void AstralCanvasApplication_Run(AstralCanvasApplication app, AstralCanvasUpdateFunction updateFunc, AstralCanvasUpdateFunction drawFunc, AstralCanvasUpdateFunction postEndDrawFunc, AstralCanvasInitFunction initFunc, AstralCanvasDeinitFunction deinitFunc);
    DynamicFunction AstralCanvasGraphics AstralCanvasApplication_GetGraphicsDevice(AstralCanvasApplication ptr);
    DynamicFunction AstralCanvasApplication AstralCanvasApplication_GetInstance();
    DynamicFunction const char *AstralCanvasApplication_GetClipboardText();
    DynamicFunction void AstralCanvasApplication_SetClipboardText(const char *text);
#ifdef __cplusplus
}
#endif