#include "Graphics/SamplerState.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalImplementations.h"
#endif

#include "ErrorHandling.hpp"

namespace AstralCanvas
{
    SamplerState AstralCanvas_SamplerState_PointClamp = {};
    SamplerState AstralCanvas_SamplerState_LinearClamp = {};
    SamplerState AstralCanvas_SamplerState_PointWrap = {};
    SamplerState AstralCanvas_SamplerState_LinearWrap = {};

    SamplerState::SamplerState()
    {
        this->backend = Backend_Vulkan;
        this->handle = NULL;
        this->sampleMode = SampleMode_Point;
        this->repeatMode = RepeatMode_Repeat;
        this->anisotropic = false;
        this->anisotropyLevel = 0.0f;
    }
    SamplerState::SamplerState(SampleMode thisSampleMode, RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel)
    {
        this->backend = GetActiveBackend();
        this->sampleMode = thisSampleMode;
        this->repeatMode = thisRepeatMode;
        this->anisotropic = isAnisotropic;
        this->anisotropyLevel = thisAnisotropyLevel;
        this->handle = NULL;

        this->Construct();
    }
    void SamplerState::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkSamplerCreateInfo createInfo = {};
                createInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
                createInfo.magFilter = AstralCanvasVk_FromSampleMode(this->sampleMode);
                createInfo.minFilter = AstralCanvasVk_FromSampleMode(this->sampleMode);

                VkSamplerAddressMode mode = AstralCanvasVk_FromRepeatMode(this->repeatMode);

                createInfo.addressModeU = mode;
                createInfo.addressModeV = mode;
                createInfo.addressModeW = mode;

                createInfo.anisotropyEnable = (i32)this->anisotropic;
                createInfo.maxAnisotropy = this->anisotropyLevel;
                createInfo.unnormalizedCoordinates = (i32)false;
                createInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
                createInfo.compareEnable = false;
                createInfo.compareOp = VK_COMPARE_OP_ALWAYS;

                createInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
                createInfo.mipLodBias = 0.0f;
                createInfo.minLod = 0.0f;
                createInfo.maxLod = 0.0f;

                VkSampler sampler;
                if (vkCreateSampler(gpu->logicalDevice, &createInfo, NULL, &sampler) != VK_SUCCESS)
                {
                    THROW_ERR("Failed to create sampler");
                }
                this->handle = sampler;
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_CreateSampler(this);
                break;
            }
#endif
            default:
            {
                THROW_ERR("Unimplemented backend: SamplerState Create");
                break;
            }
        }
    }
    void SamplerState::deinit()
    {
        if (handle == NULL)
        {
            return;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                vkDestroySampler(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkSampler)this->handle, NULL);
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_DestroySampler(this);
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: SamplerState deinit");
        }
    }

    SamplerState *SamplerGetPointClamp()
    {
        if (AstralCanvas_SamplerState_PointClamp.handle == NULL)
        {
            AstralCanvas_SamplerState_PointClamp = SamplerState(SampleMode_Point, RepeatMode_ClampToEdgeColor, false, 0.0f);
        }
        return &AstralCanvas_SamplerState_PointClamp;
    }

    SamplerState *SamplerGetLinearClamp()
    {
        if (AstralCanvas_SamplerState_LinearClamp.handle == NULL)
        {
            AstralCanvas_SamplerState_LinearClamp = SamplerState(SampleMode_Linear, RepeatMode_ClampToEdgeColor, false, 0.0f);
        }
        return &AstralCanvas_SamplerState_LinearClamp;
    }

    SamplerState *SamplerGetPointWrap()
    {
        if (AstralCanvas_SamplerState_PointWrap.handle == NULL)
        {
            AstralCanvas_SamplerState_PointWrap = SamplerState(SampleMode_Point, RepeatMode_Repeat, false, 0.0f);
        }
        return &AstralCanvas_SamplerState_PointWrap;
    }

    SamplerState *SamplerGetLinearWrap()
    {
        if (AstralCanvas_SamplerState_LinearWrap.handle == NULL)
        {
            AstralCanvas_SamplerState_LinearWrap = SamplerState(SampleMode_Linear, RepeatMode_Repeat, false, 0.0f);
        }
        return &AstralCanvas_SamplerState_LinearWrap;
    }

    void DestroyDefaultSamplerStates()
    {
        if (AstralCanvas_SamplerState_PointClamp.handle != NULL)
        {
            AstralCanvas_SamplerState_PointClamp.deinit();
        }
        if (AstralCanvas_SamplerState_LinearClamp.handle != NULL)
        {
            AstralCanvas_SamplerState_LinearClamp.deinit();
        }
        if (AstralCanvas_SamplerState_PointWrap.handle != NULL)
        {
            AstralCanvas_SamplerState_PointWrap.deinit();
        }
        if (AstralCanvas_SamplerState_LinearWrap.handle != NULL)
        {
            AstralCanvas_SamplerState_LinearWrap.deinit();
        }
    }
}
