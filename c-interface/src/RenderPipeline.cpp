#include "Astral.Canvas/Graphics/RenderPipeline.h"
#include "Graphics/RenderPipeline.hpp"

exportC void *AstralCanvasRenderPipeline_GetLayout(AstralCanvasRenderPipeline ptr)
{
    return ((AstralCanvas::RenderPipeline *)ptr)->layout;
}
exportC AstralCanvasShader AstralCanvasRenderPipeline_GetShader(AstralCanvasRenderPipeline ptr)
{
    return (AstralCanvasShader)&((AstralCanvas::RenderPipeline *)ptr)->shader;
}
exportC AstralCanvas_CullMode AstralCanvasRenderPipeline_GetCullMode(AstralCanvasRenderPipeline ptr)
{
    return (AstralCanvas_CullMode)((AstralCanvas::RenderPipeline *)ptr)->cullMode;
}
exportC AstralCanvas_PrimitiveType AstralCanvasRenderPipeline_GetPrimitiveType(AstralCanvasRenderPipeline ptr)
{
    return (AstralCanvas_PrimitiveType)((AstralCanvas::RenderPipeline *)ptr)->primitiveType;
}
exportC AstralCanvasBlendState AstralCanvasRenderPipeline_GetBlendState(AstralCanvasRenderPipeline ptr)
{
    AstralCanvasBlendState blendState = {
        (AstralCanvas_Blend)((AstralCanvas::RenderPipeline *)ptr)->blendState.sourceColorBlend,
        (AstralCanvas_Blend)((AstralCanvas::RenderPipeline *)ptr)->blendState.destinationColorBlend,
        (AstralCanvas_Blend)((AstralCanvas::RenderPipeline *)ptr)->blendState.sourceAlphaBlend,
        (AstralCanvas_Blend)((AstralCanvas::RenderPipeline *)ptr)->blendState.destinationAlphaBlend
    };
    return blendState;
}
exportC bool AstralCanvasRenderPipeline_IsDepthWrite(AstralCanvasRenderPipeline ptr)
{
    return ((AstralCanvas::RenderPipeline *)ptr)->depthWrite;
}
exportC bool AstralCanvasRenderPipeline_IsDepthTest(AstralCanvasRenderPipeline ptr)
{
    return ((AstralCanvas::RenderPipeline *)ptr)->depthTest;
}
exportC void AstralCanvasRenderPipeline_Deinit(AstralCanvasRenderPipeline ptr)
{
    ((AstralCanvas::RenderPipeline *)ptr)->deinit();
}

exportC AstralCanvasRenderPipeline AstralCanvasRenderPipeline_Init(
    AstralCanvasShader pipelineShader, 
    AstralCanvas_CullMode pipelineCullMode, 
    AstralCanvas_PrimitiveType pipelinePrimitiveType, 
    AstralCanvasBlendState pipelineBlendState, 
    bool testDepth, 
    bool writeToDepth, 
    AstralCanvasVertexDeclaration* vertexDeclarations,
    usize vertexDeclarationCount)
{
    AstralCanvas::RenderPipeline* result = (AstralCanvas::RenderPipeline*)GetCAllocator().Allocate(sizeof(AstralCanvas::RenderPipeline));
    AstralCanvas::BlendState blendState;
    blendState.sourceAlphaBlend = (AstralCanvas::Blend)pipelineBlendState.srcAlpha;
    blendState.destinationAlphaBlend = (AstralCanvas::Blend)pipelineBlendState.destAlpha;
    blendState.sourceColorBlend = (AstralCanvas::Blend)pipelineBlendState.srcColor;
    blendState.destinationColorBlend = (AstralCanvas::Blend)pipelineBlendState.destColor;

    collections::Array<AstralCanvas::VertexDeclaration *> vertexDecls = collections::Array<AstralCanvas::VertexDeclaration *>(GetCAllocator(), vertexDeclarationCount);
    for (usize i = 0; i < vertexDeclarationCount; i++)
    {
        vertexDecls.data[i] = (AstralCanvas::VertexDeclaration*)vertexDeclarations[i];
    }

    *result = AstralCanvas::RenderPipeline(GetCAllocator(), (AstralCanvas::Shader *)pipelineShader, (AstralCanvas::CullMode)pipelineCullMode, (AstralCanvas::PrimitiveType)pipelinePrimitiveType, blendState, testDepth, writeToDepth, vertexDecls);
    return result;
}