#include "Astral.Canvas/Graphics/SamplerState.h"
#include "Graphics/SamplerState.hpp"
#include "allocators.hpp"

exportC AstralCanvasSamplerState AstralCanvasSamplerState_Init(AstralCanvas_SampleMode thisSampleMode, AstralCanvas_RepeatMode thisRepeatMode, bool isAnisotropic, float thisAnisotropyLevel)
{
    AstralCanvas::SamplerState *result = (AstralCanvas::SamplerState *)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::SamplerState));
    *result = AstralCanvas::SamplerState((AstralCanvas::SampleMode)thisSampleMode, (AstralCanvas::RepeatMode)thisRepeatMode, isAnisotropic, thisAnisotropyLevel);
    return result;
}
exportC void AstralCanvasSamplerState_Deinit(AstralCanvasSamplerState ptr)
{
    ((AstralCanvas::SamplerState *)ptr)->deinit();
    GetDefaultAllocator()->Free(ptr);
}

/*exportC AstralCanvasSamplerState AstralCanvasSampler_GetPointClamp()
{
    return AstralCanvas::SamplerGetPointClamp();
}
exportC AstralCanvasSamplerState AstralCanvasSampler_GetLinearClamp()
{
    return AstralCanvas::SamplerGetLinearClamp();
}
exportC AstralCanvasSamplerState AstralCanvasSampler_GetPointWrap()
{
    return AstralCanvas::SamplerGetPointWrap();
}
exportC AstralCanvasSamplerState AstralCanvasSampler_GetLinearWrap()
{
    return AstralCanvas::SamplerGetLinearWrap();
}*/