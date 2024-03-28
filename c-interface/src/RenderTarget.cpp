#include "Astral.Canvas/Graphics/RenderTarget.h"
#include "Graphics/RenderTarget.hpp"
#include "stdio.h"

exportC AstralCanvasTexture2D AstralCanvasRenderTarget_GetTexture(AstralCanvasRenderTarget ptr, usize index)
{
    return &((AstralCanvas::RenderTarget *)ptr)->textures.data[index];
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
    AstralCanvasRenderTarget result = GetCAllocator().Allocate(sizeof(AstralCanvas::RenderTarget));
    *((AstralCanvas::RenderTarget *)result) = AstralCanvas::RenderTarget(GetCAllocator(), *((AstralCanvas::Texture2D*)thisBackendTexture), *((AstralCanvas::Texture2D*)thisDepthBuffer), isBackbuffer);
    if (thisBackendTexture != NULL)
        free(thisBackendTexture);
    if (thisDepthBuffer != NULL)
        free(thisDepthBuffer);
    return result;
}
exportC AstralCanvasRenderTarget AstralCanvasRenderTarget_Create(u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, AstralCanvas_ImageFormat depthFormat)
{
    AstralCanvasRenderTarget result = GetCAllocator().Allocate(sizeof(AstralCanvas::RenderTarget));
    *((AstralCanvas::RenderTarget *)result) = AstralCanvas::RenderTarget(GetCAllocator(), width, height, (AstralCanvas::ImageFormat)imageFormat, (AstralCanvas::ImageFormat)depthFormat);
    return result;
}
exportC AstralCanvasRenderTarget AstralCanvasRenderTarget_CreateWithInputTextures(u32 width, u32 height, AstralCanvasTexture2D* textures, usize numTextures)
{
    AstralCanvasRenderTarget result = GetCAllocator().Allocate(sizeof(AstralCanvas::RenderTarget));
    collections::Array<AstralCanvas::Texture2D> arr = collections::Array<AstralCanvas::Texture2D>(GetCAllocator(), numTextures);
    for (usize i = 0; i < numTextures; i++)
    {
        arr.data[i] = *(AstralCanvas::Texture2D*)textures[i];
        //each member of textures is allocated on the heap due to the C API.
        //Thus, since we are transferring ownership to arr, then to RenderTarget,
        //we need to free the heap allocation for each texture.
        free(textures[i]);
    }
    *((AstralCanvas::RenderTarget *)result) = AstralCanvas::RenderTarget(GetCAllocator(), width, height, arr);
    //dont deinit arr as it's ownership is now transferred to RenderTarget
    return result;
}
exportC void AstralCanvasRenderTarget_Deinit(AstralCanvasRenderTarget ptr)
{
    ((AstralCanvas::RenderTarget *)ptr)->deinit();
}