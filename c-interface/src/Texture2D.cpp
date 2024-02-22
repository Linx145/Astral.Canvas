#include "Astral.Canvas/Graphics/Texture2D.h"
#include "Graphics/Texture2D.hpp"
#include "allocators.hpp"

exportC void *AstralCanvasTexture2D_RetrieveCurrentData(AstralCanvasTexture2D ptr)
{
    if (((AstralCanvas::Texture2D *)ptr)->bytes == NULL)
    {
        ((AstralCanvas::Texture2D *)ptr)->GetData();
    }
    return ((AstralCanvas::Texture2D *)ptr)->bytes;
}
exportC void *AstralCanvasTexture2D_GetData(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->GetData();
}
exportC u32 AstralCanvasTexture2D_GetWidth(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->width;
}
exportC u32 AstralCanvasTexture2D_GetHeight(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->height;
}
exportC bool AstralCanvasTexture2D_StoreData(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->storeData;
}
exportC u32 AstralCanvasTexture2D_GetMipLevels(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->mipLevels;
}
exportC AstralCanvas_ImageFormat AstralCanvasTexture2D_GetImageFormat(AstralCanvasTexture2D ptr)
{
    return (AstralCanvas_ImageFormat)((AstralCanvas::Texture2D *)ptr)->imageFormat;
}
exportC bool AstralCanvasTexture2D_OwnsHandle(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->ownsHandle;
}
exportC bool AstralCanvasTexture2D_UsedForRenderTarget(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->usedForRenderTarget;
}
exportC void *AstralCanvasTexture2D_GetImageHandle(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->imageHandle;
}
exportC void *AstralCanvasTexture2D_GetImageView(AstralCanvasTexture2D ptr)
{
    return ((AstralCanvas::Texture2D *)ptr)->imageView;
}
exportC void AstralCanvasTexture2D_Deinit(AstralCanvasTexture2D ptr)
{
    ((AstralCanvas::Texture2D *)ptr)->deinit();
    GetDefaultAllocator()->Free(ptr);
}

exportC AstralCanvasTexture2D AstralCanvasTexture2D_FromHandle(void *handle, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, bool usedForRenderTarget)
{
    AstralCanvasTexture2D result = GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::Texture2D));
    AstralCanvas::Texture2D texture = AstralCanvas::CreateTextureFromHandle(handle, width, height, (AstralCanvas::ImageFormat)imageFormat, usedForRenderTarget);
    *((AstralCanvas::Texture2D *)result) = texture;
    return result;
}
exportC AstralCanvasTexture2D AstralCanvasTexture2D_FromData(u8* data, u32 width, u32 height, AstralCanvas_ImageFormat imageFormat, bool usedForRenderTarget, bool storeData)
{
    AstralCanvasTexture2D result = GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::Texture2D));
    AstralCanvas::Texture2D texture = AstralCanvas::CreateTextureFromData(data, width, height, (AstralCanvas::ImageFormat)imageFormat, usedForRenderTarget, storeData);
    *((AstralCanvas::Texture2D *)result) = texture;
    return result;
}
exportC AstralCanvasTexture2D AstralCanvasTexture2D_FromFile(const char *fileName, bool storeData)
{
    AstralCanvasTexture2D result = GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::Texture2D));
    AstralCanvas::Texture2D texture = AstralCanvas::CreateTextureFromFile(fileName, storeData);
    *((AstralCanvas::Texture2D *)result) = texture;
    return result;
}