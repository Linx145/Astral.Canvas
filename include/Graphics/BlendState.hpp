#pragma once
#include "Graphics/Enums.hpp"

namespace AstralCanvas
{
    struct BlendState
    {
        Blend sourceColorBlend;
        Blend sourceAlphaBlend;
        Blend destinationColorBlend;
        Blend destinationAlphaBlend;
    };
}
#define ADDITIVE_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_One, AstralCanvas::Blend_One}
#define ALPHA_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_One, AstralCanvas::Blend_One, AstralCanvas::Blend_InverseSourceAlpha, AstralCanvas::Blend_InverseSourceAlpha}
#define NON_PREMULTIPLIED_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_InverseSourceAlpha, AstralCanvas::Blend_InverseSourceAlpha}
#define OPAQUE_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_One, AstralCanvas::Blend_One, AstralCanvas::Blend_Zero, AstralCanvas::Blend_Zero}