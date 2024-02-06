#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Shader.h"
#include "Astral.Canvas/Graphics/VertexDeclaration.h"

extern "C"
{
    typedef struct
    {
        AstralCanvas_Blend srcColor;
        AstralCanvas_Blend destColor;
        AstralCanvas_Blend srcAlpha;
        AstralCanvas_Blend dstAlpha;
    } AstralCanvasBlendState;

    typedef void *AstralCanvasRenderPipeline;

    void *AstralCanvasRenderPipeline_GetLayout(AstralCanvasRenderPipeline ptr);
    AstralCanvasShader AstralCanvasRenderPipeline_GetShader(AstralCanvasRenderPipeline ptr);
    AstralCanvas_CullMode AstralCanvasRenderPipeline_GetCullMode(AstralCanvasRenderPipeline ptr);
    AstralCanvas_PrimitiveType AstralCanvasRenderPipeline_GetPrimitiveType(AstralCanvasRenderPipeline ptr);
    AstralCanvasBlendState AstralCanvasRenderPipeline_GetBlendState(AstralCanvasRenderPipeline ptr);
    bool AstralCanvasRenderPipeline_IsDepthWrite(AstralCanvasRenderPipeline ptr);
    bool AstralCanvasRenderPipeline_IsDepthTest(AstralCanvasRenderPipeline ptr);
    void AstralCanvasRenderPipeline_Deinit(AstralCanvasRenderPipeline ptr);

    AstralCanvasRenderPipeline AstralCanvasRenderPipeline_Init(
        AstralCanvasShader pipelineShader, 
        AstralCanvas_CullMode pipelineCullMode, 
        AstralCanvas_PrimitiveType pipelinePrimitiveType, 
        AstralCanvasBlendState pipelineBlendState, 
        bool testDepth, 
        bool writeToDepth, 
        AstralCanvasVertexDeclaration* vertexDeclarations,
        usize vertexDeclarationCount);
}