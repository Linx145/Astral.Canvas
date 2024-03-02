#include "Astral.Canvas/ErrorHandling.h"
#include "ErrorHandling.hpp"

exportC void AstralCanvas_SetGlobalErrorCallback(AstralCanvas_ErrorCallback callback)
{
    AstralCanvas::SetGlobalErrorCallback((AstralCanvas::ErrorCallback)callback);
}

exportC void AstralCanvas_SetGlobalWarningCallback(AstralCanvas_ErrorCallback callback)
{
    AstralCanvas::SetGlobalWarningCallback((AstralCanvas::ErrorCallback)callback);
}