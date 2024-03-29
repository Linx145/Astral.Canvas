#pragma once
#include "Astral.Canvas/Graphics/Texture2D.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasRenderTarget;

    DynamicFunction AstralCanvasTexture2D AstralCanvasRenderTarget_GetTexture(AstralCanvasRenderTarget ptr, usize index);
    DynamicFunction u32 AstralCanvasRenderTarget_GetWidth(AstralCanvasRenderTarget ptr);
    DynamicFunction u32 AstralCanvasRenderTarget_GetHeight(AstralCanvasRenderTarget ptr);
    DynamicFunction bool AstralCanvasRenderTarget_IsBackbuffer(AstralCanvasRenderTarget ptr);
    DynamicFunction void *AstralCanvasRenderTarget_GetHandle(AstralCanvasRenderTarget ptr);
    DynamicFunction bool AstralCanvasRenderTarget_GetUseStatus(AstralCanvasRenderTarget ptr);
    DynamicFunction void AstralCanvasRenderTarget_SetUseStatus(AstralCanvasRenderTarget ptr, bool hasBeenUsedBefore);
    DynamicFunction AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateWithInputTextures(u32 width, u32 height, AstralCanvasTexture2D *textures, usize numTextures);
    DynamicFunction AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateFromTextures(AstralCanvasTexture2D thisBackendTexture, AstralCanvasTexture2D thisDepthBuffer, bool isBackbuffer);
    DynamicFunction AstralCanvasRenderTarget AstralCanvasRenderTarget_Create(u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, AstralCanvas_ImageFormat depthFormat);
    DynamicFunction void AstralCanvasRenderTarget_Deinit(AstralCanvasRenderTarget ptr);

#ifdef __cplusplus
}
#endif