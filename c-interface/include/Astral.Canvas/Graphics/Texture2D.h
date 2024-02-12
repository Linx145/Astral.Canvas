#pragma once
#include "Linxc.h"
#include "stdbool.h"
#include "Astral.Canvas/Graphics/Enums.h"
#include "Astral.Canvas/Graphics/SamplerState.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasTexture2D;

    DynamicFunction u8 *AstralCanvasTexture2D_GetData(AstralCanvasTexture2D ptr);
    DynamicFunction u32 AstralCanvasTexture2D_GetWidth(AstralCanvasTexture2D ptr);
    DynamicFunction u32 AstralCanvasTexture2D_GetHeight(AstralCanvasTexture2D ptr);
    DynamicFunction bool AstralCanvasTexture2D_StoreData(AstralCanvasTexture2D ptr);
    DynamicFunction u32 AstralCanvasTexture2D_GetMipLevels(AstralCanvasTexture2D ptr);
    DynamicFunction AstralCanvas_ImageFormat AstralCanvasTexture2D_GetImageFormat(AstralCanvasTexture2D ptr);
    // AstralCanvasSamplerState AstralCanvasTexture2D_GetSamplerState(AstralCanvasTexture2D ptr);
    DynamicFunction bool AstralCanvasTexture2D_OwnsHandle(AstralCanvasTexture2D ptr);
    DynamicFunction bool AstralCanvasTexture2D_UsedForRenderTarget(AstralCanvasTexture2D ptr);
    DynamicFunction void *AstralCanvasTexture2D_GetImageHandle(AstralCanvasTexture2D ptr);
    DynamicFunction void *AstralCanvasTexture2D_GetImageView(AstralCanvasTexture2D ptr);
    DynamicFunction void AstralCanvasTexture2D_Deinit(AstralCanvasTexture2D ptr);
    DynamicFunction AstralCanvasTexture2D AstralCanvasTexture2D_FromHandle(void *handle, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, bool usedForRenderTarget);
    DynamicFunction AstralCanvasTexture2D AstralCanvasTexture2D_FromData(u8* data, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, bool usedForRenderTarget);
    DynamicFunction AstralCanvasTexture2D AstralCanvasTexture2D_FromFile(const char *fileName);
#ifdef __cplusplus
}
#endif