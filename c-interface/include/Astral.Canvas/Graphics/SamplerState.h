#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasSamplerState;

    DynamicFunction AstralCanvasSamplerState AstralCanvasSamplerState_Init(AstralCanvas_SampleMode thisSampleMode, AstralCanvas_RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel);
    DynamicFunction void AstralCanvasSamplerState_Deinit(AstralCanvasSamplerState ptr);
    DynamicFunction AstralCanvasSamplerState AstralCanvasSampler_GetPointClamp();
    DynamicFunction AstralCanvasSamplerState AstralCanvasSampler_GetLinearClamp();
    DynamicFunction AstralCanvasSamplerState AstralCanvasSampler_GetPointWrap();
    DynamicFunction AstralCanvasSamplerState AstralCanvasSampler_GetLinearWrap();

#ifdef __cplusplus
}
#endif