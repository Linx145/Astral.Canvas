#pragma once
#include "Astral.Canvas/Graphics/Texture2D.h"
#include "stdbool.h"

extern "C"
{
    typedef void *AstralCanvasRenderTarget;

    AstralCanvasTexture2D AstralCanvasRenderTarget_GetTexture(AstralCanvasRenderTarget ptr);
    AstralCanvasTexture2D AstralCanvasRenderTarget_GetDepthBuffer(AstralCanvasRenderTarget ptr);
    u32 AstralCanvasRenderTarget_GetWidth(AstralCanvasRenderTarget ptr);
    u32 AstralCanvasRenderTarget_GetHeight(AstralCanvasRenderTarget ptr);
    bool AstralCanvasRenderTarget_IsBackbuffer(AstralCanvasRenderTarget ptr);
    void *AstralCanvasRenderTarget_GetHandle(AstralCanvasRenderTarget ptr);
    bool AstralCanvasRenderTarget_GetUseStatus(AstralCanvasRenderTarget ptr);
    void AstralCanvasRenderTarget_SetUseStatus(AstralCanvasRenderTarget ptr, bool hasBeenUsedBefore);

    AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateFromTextures(AstralCanvasTexture2D thisBackendTexture, AstralCanvasTexture2D thisDepthBuffer, bool isBackbuffer);
    AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateFromHandle(u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, AstralCanvas_ImageFormat depthFormat);
    void AstralCanvasRenderTarget_Deinit(AstralCanvasRenderTarget ptr);
}