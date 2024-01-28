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

        inline bool operator==(BlendState B)
        {
            return 
            this->sourceColorBlend == B.sourceColorBlend &&
            this->sourceAlphaBlend == B.sourceAlphaBlend &&
            this->destinationColorBlend == B.destinationColorBlend &&
            this->destinationAlphaBlend == B.destinationAlphaBlend;
        }
        inline bool operator!=(BlendState B)
        {
            return 
            this->sourceColorBlend != B.sourceColorBlend ||
            this->sourceAlphaBlend != B.sourceAlphaBlend ||
            this->destinationColorBlend != B.destinationColorBlend ||
            this->destinationAlphaBlend != B.destinationAlphaBlend;
        }
    };
}
#define DISABLE_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_Disable, AstralCanvas::Blend_Disable, AstralCanvas::Blend_Disable, AstralCanvas::Blend_Disable}
#define ADDITIVE_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_One, AstralCanvas::Blend_One}
#define ALPHA_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_One, AstralCanvas::Blend_One, AstralCanvas::Blend_InverseSourceAlpha, AstralCanvas::Blend_InverseSourceAlpha}
#define NON_PREMULTIPLIED_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_SourceAlpha, AstralCanvas::Blend_InverseSourceAlpha, AstralCanvas::Blend_InverseSourceAlpha}
#define OPAQUE_BLEND AstralCanvas::BlendState{AstralCanvas::Blend_One, AstralCanvas::Blend_One, AstralCanvas::Blend_Zero, AstralCanvas::Blend_Zero}