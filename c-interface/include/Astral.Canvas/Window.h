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

    DynamicFunction AstralCanvasPoint2 AstralCanvasWindow_GetResolution(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetResolution(AstralCanvasWindow ptr, AstralCanvasPoint2 resolution);
    DynamicFunction AstralCanvasPoint2 AstralCanvasWindow_GetPosition(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetPosition(AstralCanvasWindow ptr, AstralCanvasPoint2 position);
    DynamicFunction AstralCanvasRectangle AstralCanvasWindow_AsRectangle(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_Deinit(AstralCanvasWindow ptr);
    DynamicFunction void AstralCanvasWindow_SetTitle(AstralCanvasWindow ptr, const char *title);
#ifdef __cplusplus
}
#endif