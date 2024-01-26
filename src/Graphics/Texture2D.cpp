#include "Graphics/Texture2D.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#endif

namespace AstralCanvas
{
    void Texture2D::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralCanvasVk_DestroyTexture2D(AstralCanvasVk_GetCurrentGPU(), this);
                break;
            }
            #endif
            default:
                THROW_ERR("Unrecognised backend when attempting to create a texture");
                break;
        }
    }
    Texture2D CreateTextureFromData(u8* data, usize width, usize height, SamplerState *samplerForTexture, ImageFormat imageFormat, bool usedForRenderTarget)
    {
        Texture2D result = {};
        result.width = width;
        result.height = height;
        result.ownsHandle = true;
        result.bytes = data;
        result.imageFormat = imageFormat;
        result.mipLevels = 1;
        result.usedForRenderTarget = usedForRenderTarget;

        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
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
}