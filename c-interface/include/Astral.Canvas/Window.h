#include "Linxc.h"

extern "C"
{

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

    AstralCanvasPoint2 AstralCanvasWindow_GetResolution(AstralCanvasWindow ptr);

    void AstralCanvasWindow_SetResolution(AstralCanvasWindow ptr, AstralCanvasPoint2 resolution);

    AstralCanvasPoint2 AstralCanvasWindow_GetPosition(AstralCanvasWindow ptr);

    void AstralCanvasWindow_SetPosition(AstralCanvasWindow ptr, AstralCanvasPoint2 position);

    AstralCanvasRectangle AstralCanvasWindow_AsRectangle(AstralCanvasWindow ptr);

    void AstralCanvasWindow_Deinit(AstralCanvasWindow ptr);

}