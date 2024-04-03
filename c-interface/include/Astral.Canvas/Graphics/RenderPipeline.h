#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Shader.h"
#include "Astral.Canvas/Graphics/VertexDeclaration.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        AstralCanvas_Blend srcColor;
        AstralCanvas_Blend destColor;
        AstralCanvas_Blend srcAlpha;
        AstralCanvas_Blend destAlpha;
    } AstralCanvasBlendState;

    #define DISABLE_BLEND AstralCanvasBlendState{AstralCanvas_Blend_Disable, AstralCanvas_Blend_Disable, AstralCanvas_Blend_Disable, AstralCanvas_Blend_Disable}
    #define ADDITIVE_BLEND AstralCanvasBlendState{AstralCanvas_Blend_SourceAlpha, AstralCanvas_Blend_SourceAlpha, AstralCanvas_Blend_One, AstralCanvas_Blend_One}
    #define ALPHA_BLEND AstralCanvasBlendState{AstralCanvas_Blend_One, AstralCanvas_Blend_One, AstralCanvas_Blend_InverseSourceAlpha, AstralCanvas_Blend_InverseSourceAlpha}
    #define NON_PREMULTIPLIED_BLEND AstralCanvasBlendState{AstralCanvas_Blend_SourceAlpha, AstralCanvas_Blend_SourceAlpha, AstralCanvas_Blend_InverseSourceAlpha, AstralCanvas_Blend_InverseSourceAlpha}
    #define OPAQUE_BLEND AstralCanvasBlendState{AstralCanvas_Blend_One, AstralCanvas_Blend_One, AstralCanvas_Blend_Zero, AstralCanvas_Blend_Zero}

    typedef void *AstralCanvasRenderPipeline;

    DynamicFunction void *AstralCanvasRenderPipeline_GetLayout(AstralCanvasRenderPipeline ptr);
    DynamicFunction AstralCanvasShader AstralCanvasRenderPipeline_GetShader(AstralCanvasRenderPipeline ptr);
    DynamicFunction AstralCanvas_CullMode AstralCanvasRenderPipeline_GetCullMode(AstralCanvasRenderPipeline ptr);
    DynamicFunction AstralCanvas_PrimitiveType AstralCanvasRenderPipeline_GetPrimitiveType(AstralCanvasRenderPipeline ptr);
    DynamicFunction AstralCanvasBlendState AstralCanvasRenderPipeline_GetBlendState(AstralCanvasRenderPipeline ptr);
    DynamicFunction bool AstralCanvasRenderPipeline_IsDepthWrite(AstralCanvasRenderPipeline ptr);
    DynamicFunction bool AstralCanvasRenderPipeline_IsDepthTest(AstralCanvasRenderPipeline ptr);
    DynamicFunction void AstralCanvasRenderPipeline_Deinit(AstralCanvasRenderPipeline ptr);
    DynamicFunction AstralCanvasRenderPipeline AstralCanvasRenderPipeline_Init(
        AstralCanvasShader pipelineShader, 
        AstralCanvas_CullMode pipelineCullMode, 
        AstralCanvas_PrimitiveType pipelinePrimitiveType, 
        AstralCanvasBlendState pipelineBlendState, 
        bool testDepth, 
        bool writeToDepth, 
        AstralCanvasVertexDeclaration* vertexDeclarations,
        usize vertexDeclarationCount);

#ifdef __cplusplus
}
#endif