#include "Graphics/SamplerState.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
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
        this->constructed = false;
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
        this->constructed = false;

        switch (this->backend)
        {
            #ifdef ASTRALCANVAS_VULKAN
            AstralCanvas_Vulkan:
            {
                AstralCanvasVk_CreateSamplerState(AstralCanvasVk_GetCurrentGPU(), this);
                break;
            }
            #endif
            default:
            {
                THROW_ERR("Unimplemented backend");
                break;
            }
        }
    }
    void SamplerState::deinit()
    {
        if (this->constructed)
        {
            switch (this->backend)
            {
                AstralCanvas_Vulkan:
                {
                    AstralCanvasVk_DestroySamplerState(AstralCanvasVk_GetCurrentGPU(), this);
                    break;
                }
                default:
                    THROW_ERR("Unimplemented backend");
            }
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
}