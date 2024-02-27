#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

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
    DynamicFunction void AstralCanvasWindow_SetMouseVisible(AstralCanvasWindow ptr, bool visible);
#ifdef __cplusplus
}
#endif