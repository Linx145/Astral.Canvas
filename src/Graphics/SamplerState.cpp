#include "Graphics/SamplerState.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#endif

#include "ErrorHandling.hpp"

SamplerState AstralCanvas_SamplerState_PointClamp = {};
SamplerState AstralCanvas_SamplerState_LinearClamp = {};
SamplerState AstralCanvas_SamplerState_PointWrap = {};
SamplerState AstralCanvas_SamplerState_LinearWrap = {};

SamplerState AstralCanvas_ConstructSamplerState(AstralCanvasGraphicsBackend preferredBackend, SampleMode thisSampleMode, RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel)
{
    SamplerState samplerState = {};

    samplerState.backend = preferredBackend;
    samplerState.sampleMode = thisSampleMode;
    samplerState.repeatMode = thisRepeatMode;
    samplerState.anisotropic = isAnisotropic;
    samplerState.anisotropyLevel = thisAnisotropyLevel;
    samplerState.handle = NULL;
    samplerState.constructed = false;

    switch (samplerState.backend)
    {
        #ifdef ASTRALCANVAS_VULKAN
        AstralCanvas_Vulkan:
        {
            AstralCanvasVk_CreateSamplerState(AstralCanvasVk_GetCurrentGPU(), &samplerState);
            break;
        }
        #endif
        default:
        {
            THROW_ERR("Unimplemented backend");
            break;
        }
    }
    return samplerState;
}
void AstralCanvas_DestroySamplerState(SamplerState* samplerState)
{
    if (samplerState->constructed)
    {
        switch (samplerState->backend)
        {
            AstralCanvas_Vulkan:
            {
                AstralCanvasVk_DestroySamplerState(AstralCanvasVk_GetCurrentGPU(), samplerState);
                break;
            }
            default:
                THROW_ERR("Unimplemented backend");
        }
    }
}

SamplerState *AstralCanvas_SamplerGetPointClamp()
{
    if (AstralCanvas_SamplerState_PointClamp.handle == NULL)
    {
        AstralCanvas_SamplerState_PointClamp = AstralCanvas_ConstructSamplerState(AstralCanvas_GetActiveBackend(), SampleMode_Point, RepeatMode_ClampToEdgeColor, false, 0.0f);
    }
    return &AstralCanvas_SamplerState_PointClamp;
}

SamplerState *AstralCanvas_SamplerGetLinearClamp()
{
    if (AstralCanvas_SamplerState_LinearClamp.handle == NULL)
    {
        AstralCanvas_SamplerState_LinearClamp = AstralCanvas_ConstructSamplerState(AstralCanvas_GetActiveBackend(), SampleMode_Linear, RepeatMode_ClampToEdgeColor, false, 0.0f);
    }
    return &AstralCanvas_SamplerState_LinearClamp;
}

SamplerState *AstralCanvas_SamplerGetPointWrap()
{
    if (AstralCanvas_SamplerState_PointWrap.handle == NULL)
    {
        AstralCanvas_SamplerState_PointWrap = AstralCanvas_ConstructSamplerState(AstralCanvas_GetActiveBackend(), SampleMode_Point, RepeatMode_Repeat, false, 0.0f);
    }
    return &AstralCanvas_SamplerState_PointWrap;
}

SamplerState *AstralCanvas_SamplerGetLinearWrap()
{
    if (AstralCanvas_SamplerState_LinearWrap.handle == NULL)
    {
        AstralCanvas_SamplerState_LinearWrap = AstralCanvas_ConstructSamplerState(AstralCanvas_GetActiveBackend(), SampleMode_Linear, RepeatMode_Repeat, false, 0.0f);
    }
    return &AstralCanvas_SamplerState_LinearWrap;
}