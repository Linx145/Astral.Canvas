#pragma once
#include "Linxc.h"
#include "stdbool.h"
#include "Astral.Canvas/Graphics/Enums.h"
#include "Astral.Canvas/Graphics/SamplerState.h"

extern "C"
{
    typedef void *AstralCanvasTexture2D;

    u8 *AstralCanvasTexture2D_GetData(AstralCanvasTexture2D ptr);
    u32 AstralCanvasTexture2D_GetWidth(AstralCanvasTexture2D ptr);
    u32 AstralCanvasTexture2D_GetHeight(AstralCanvasTexture2D ptr);
    bool AstralCanvasTexture2D_StoreData(AstralCanvasTexture2D ptr);
    u32 AstralCanvasTexture2D_GetMipLevels(AstralCanvasTexture2D ptr);
    AstralCanvas_ImageFormat AstralCanvasTexture2D_GetImageFormat(AstralCanvasTexture2D ptr);
    // AstralCanvasSamplerState AstralCanvasTexture2D_GetSamplerState(AstralCanvasTexture2D ptr);
    bool AstralCanvasTexture2D_OwnsHandle(AstralCanvasTexture2D ptr);
    bool AstralCanvasTexture2D_UsedForRenderTarget(AstralCanvasTexture2D ptr);
    void *AstralCanvasTexture2D_GetImageHandle(AstralCanvasTexture2D ptr);
    void *AstralCanvasTexture2D_GetImageView(AstralCanvasTexture2D ptr);
    void AstralCanvasTexture2D_Deinit(AstralCanvasTexture2D ptr);

    AstralCanvasTexture2D AstralCanvasTexture2D_FromHandle(void *handle, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, bool usedForRenderTarget);
    AstralCanvasTexture2D AstralCanvasTexture2D_FromData(u8* data, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, AstralCanvasSamplerState *samplerState, bool usedForRenderTarget);
    AstralCanvasTexture2D AstralCanvasTexture2D_FromFile(const char *fileName);
}