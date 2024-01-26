#include "Graphics/Texture2D.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#endif

void Texture2D::deinit()
{
    AstralCanvas_DestroyTexture(this);
}
void AstralCanvas_DestroyTexture(Texture2D* texture)
{
    switch (AstralCanvas_GetActiveBackend())
    {
        #ifdef ASTRALCANVAS_VULKAN
        case AstralCanvas_Vulkan:
        {
            AstralCanvasVk_DestroyTexture2D(AstralCanvasVk_GetCurrentGPU(), texture);
            break;
        }
        #endif
        default:
            THROW_ERR("Unrecognised backend when attempting to create a texture");
            break;
    }
}
Texture2D AstralCanvas_CreateTextureFromData(u8* data, usize width, usize height, SamplerState *samplerForTexture, ImageFormat imageFormat, bool usedForRenderTarget)
{
    Texture2D result = {};
    result.width = width;
    result.height = height;
    result.ownsHandle = true;
    result.bytes = data;
    result.imageFormat = imageFormat;
    result.mipLevels = 1;
    result.usedForRenderTarget = usedForRenderTarget;

    switch (AstralCanvas_GetActiveBackend())
    {
        #ifdef ASTRALCANVAS_VULKAN
        case AstralCanvas_Vulkan:
        {
            AstralCanvasVk_CreateTexture2D(AstralCanvasVk_GetCurrentGPU(), &result);
            break;
        }
        #endif
        default:
            THROW_ERR("Unrecognised backend when attempting to create a texture");
            break;
    }

    return result;
}