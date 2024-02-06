#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

extern "C"
{
    typedef void *AstralCanvasSamplerState;

    AstralCanvasSamplerState AstralCanvasSamplerState_Init(AstralCanvas_SampleMode thisSampleMode, AstralCanvas_RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel);
    void AstralCanvasSamplerState_Deinit(AstralCanvasSamplerState ptr);

    AstralCanvasSamplerState AstralCanvasSampler_GetPointClamp();
    AstralCanvasSamplerState AstralCanvasSampler_GetLinearClamp();
    AstralCanvasSamplerState AstralCanvasSampler_GetPointWrap();
    AstralCanvasSamplerState AstralCanvasSampler_GetLinearWrap();
}