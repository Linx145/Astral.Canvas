#include "Astral.Canvas/Graphics/RenderTarget.h"
#include "Graphics/RenderTarget.hpp"

exportC AstralCanvasTexture2D AstralCanvasRenderTarget_GetTexture(AstralCanvasRenderTarget ptr)
{
    return &((AstralCanvas::RenderTarget *)ptr)->backendTexture;
}
exportC AstralCanvasTexture2D AstralCanvasRenderTarget_GetDepthBuffer(AstralCanvasRenderTarget ptr)
{
    return &((AstralCanvas::RenderTarget *)ptr)->depthBuffer;
}
exportC exportC u32 AstralCanvasRenderTarget_GetWidth(AstralCanvasRenderTarget ptr)
{
    return ((AstralCanvas::RenderTarget *)ptr)->width;
}
exportC u32 AstralCanvasRenderTarget_GetHeight(AstralCanvasRenderTarget ptr)
{
    return ((AstralCanvas::RenderTarget *)ptr)->height;
}
exportC bool AstralCanvasRenderTarget_IsBackbuffer(AstralCanvasRenderTarget ptr)
{
    return ((AstralCanvas::RenderTarget *)ptr)->isBackbuffer;
}
exportC void *AstralCanvasRenderTarget_GetHandle(AstralCanvasRenderTarget ptr)
{
    return ((AstralCanvas::RenderTarget *)ptr)->renderTargetHandle;
}
exportC bool AstralCanvasRenderTarget_GetUseStatus(AstralCanvasRenderTarget ptr)
{
    return ((AstralCanvas::RenderTarget *)ptr)->hasBeenUsedBefore;
}
exportC void AstralCanvasRenderTarget_SetUseStatus(AstralCanvasRenderTarget ptr, bool hasBeenUsedBefore)
{
    ((AstralCanvas::RenderTarget *)ptr)->hasBeenUsedBefore = hasBeenUsedBefore;
}
exportC AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateFromTextures(AstralCanvasTexture2D thisBackendTexture, AstralCanvasTexture2D thisDepthBuffer, bool isBackbuffer)
{
    AstralCanvasRenderTarget result = GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::RenderTarget));
    *((AstralCanvas::RenderTarget *)result) = AstralCanvas::RenderTarget(*((AstralCanvas::Texture2D*)thisBackendTexture), *((AstralCanvas::Texture2D*)thisDepthBuffer), isBackbuffer);
    if (thisBackendTexture != NULL)
        GetDefaultAllocator()->Free(thisBackendTexture);
    if (thisDepthBuffer != NULL)
        GetDefaultAllocator()->Free(thisDepthBuffer);
    return result;
}
exportC AstralCanvasRenderTarget AstralCanvasRenderTarget_Create(u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, AstralCanvas_ImageFormat depthFormat)
{
    AstralCanvasRenderTarget result = GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::RenderTarget));
    *((AstralCanvas::RenderTarget *)result) = AstralCanvas::RenderTarget(width, height, (AstralCanvas::ImageFormat)imageFormat, (AstralCanvas::ImageFormat)depthFormat);
    return result;
}
exportC void AstralCanvasRenderTarget_Deinit(AstralCanvasRenderTarget ptr)
{
    ((AstralCanvas::RenderTarget *)ptr)->deinit();
}