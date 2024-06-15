#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef enum
    {
        AstralCanvas_WindowMouseState_Normal,
        AstralCanvas_WindowMouseState_Disabled,
        AstralCanvas_WindowMouseState_Hidden
    } AstralCanvasWindowMouseState;

    typedef struct
    {
        i32 X;
        i32 Y;
    } AstralCanvasPoint2;

    typedef struct
    {
        i32 X;
        i32 Y;
        i32 Width;
        i32 Height;
    } AstralCanvasRectangle;

    typedef void *AstralCanvasWindow;

    def_delegate(AstralCanvasWindowOnTextInputFunction, void, AstralCanvasWindow window, u32 characterUnicode);
	def_delegate(AstralCanvasWindowOnKeyInteractedFunction, void, AstralCanvasWindow window, i32 key, i32 action);

    DynamicFunction void AstralCanvasWindow_CloseWindow(AstralCanvasWindow ptr);
    DynamicFunction AstralCanvasPoint2 AstralCanvasWindow_GetResolution(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetResolution(AstralCanvasWindow ptr, AstralCanvasPoint2 resolution);
    DynamicFunction AstralCanvasPoint2 AstralCanvasWindow_GetPosition(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetPosition(AstralCanvasWindow ptr, AstralCanvasPoint2 position);
    DynamicFunction AstralCanvasRectangle AstralCanvasWindow_AsRectangle(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_Deinit(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetTitle(AstralCanvasWindow ptr, const char *title);
    DynamicFunction bool AstralCanvasWindow_GetIsFullscreen(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetFullscreen(AstralCanvasWindow ptr, bool value);
    DynamicFunction void AstralCanvasWindow_SetOnKeyInteractCallback(AstralCanvasWindow ptr, AstralCanvasWindowOnKeyInteractedFunction callback);
    DynamicFunction void AstralCanvasWindow_SetOnTextInputCallback(AstralCanvasWindow ptr, AstralCanvasWindowOnTextInputFunction callback);
    DynamicFunction void AstralCanvasWindow_SetMouseState(AstralCanvasWindow ptr, AstralCanvasWindowMouseState state);
    DynamicFunction AstralCanvasWindowMouseState AstralCanvasWindow_GetMouseState(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetMouseIcon(AstralCanvasWindow ptr, void *iconData, u32 iconWidth, u32 iconHeight, i32 originX, i32 originY);
    DynamicFunction i32 AstralCanvasWindow_GetCurrentMonitorFramerate(AstralCanvasWindow ptr);
#ifdef __cplusplus
}
#endif