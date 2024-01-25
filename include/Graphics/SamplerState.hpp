#pragma once
#include "Linxc.h"
#include "Enums.hpp"

struct SamplerState
{
    AstralCanvasGraphicsBackend backend;
    /// Whether the sampler state handle has been created
    bool constructed;
    /// The handle of the sampler in the graphics API. Corresponds to a sampler state in the current graphics backend
    void *handle;

    /// The filter applied after sampling a pixel
    SampleMode sampleMode;
    /// The behaviour of the sampler if it were to sample outside of the texture bounds
    RepeatMode repeatMode;
    /// Whether the sampler should be anisotropic
    bool anisotropic;
    /// The level of anisotropy, if anisotropic is enabled
    float anisotropyLevel;
};

SamplerState AstralCanvas_ConstructSamplerState(AstralCanvasGraphicsBackend preferredBackend, SampleMode thisSampleMode, RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel);

void AstralCanvas_DestroySamplerState(SamplerState* samplerState);

SamplerState *AstralCanvas_SamplerGetPointClamp();

SamplerState *AstralCanvas_SamplerGetLinearClamp();

SamplerState *AstralCanvas_SamplerGetPointWrap();

SamplerState *AstralCanvas_SamplerGetLinearWrap();