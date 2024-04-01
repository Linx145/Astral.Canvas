#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/IndexBuffer.h"
#include "Astral.Canvas/Graphics/InstanceBuffer.h"
#include "Astral.Canvas/Graphics/VertexBuffer.h"
#include "Astral.Canvas/Graphics/RenderTarget.h"
#include "Astral.Canvas/Graphics/Texture2D.h"
#include "Astral.Canvas/Graphics/SamplerState.h"
#include "Astral.Canvas/Graphics/RenderProgram.h"
#include "Astral.Canvas/Graphics/RenderPipeline.h"
#include "Astral.Canvas/Graphics/Color.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        i32 X;
        i32 Y;
        i32 width;
        i32 height;
    } AstralCanvasClipArea;
    typedef void *AstralCanvasGraphics;

    DynamicFunction AstralCanvasRenderProgram AstralCanvasGraphics_GetCurrentRenderProgram(AstralCanvasGraphics ptr);
    DynamicFunction AstralCanvasRenderTarget AstralCanvasGraphics_GetCurrentRenderTarget(AstralCanvasGraphics ptr);
    DynamicFunction u32 AstralCanvasGraphics_GetCurrentRenderProgramPass(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_SetVertexBuffer(AstralCanvasGraphics ptr, const AstralCanvasVertexBuffer vb, u32 bindingPoint);
    DynamicFunction void AstralCanvasGraphics_SetInstanceBuffer(AstralCanvasGraphics ptr, const AstralCanvasInstanceBuffer instanceBuffer, u32 bindingPoint);
    DynamicFunction void AstralCanvasGraphics_SetIndexBuffer(AstralCanvasGraphics ptr, const AstralCanvasIndexBuffer indexBuffer);
    DynamicFunction void AstralCanvasGraphics_SetRenderTarget(AstralCanvasGraphics ptr, AstralCanvasRenderTarget target);
    DynamicFunction void AstralCanvasGraphics_StartRenderProgram(AstralCanvasGraphics ptr, AstralCanvasRenderProgram program, AstralCanvasColor clearColor);
    DynamicFunction void AstralCanvasGraphics_EndRenderProgram(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_UseRenderPipeline(AstralCanvasGraphics ptr, AstralCanvasRenderPipeline pipeline);
    DynamicFunction void AstralCanvasGraphics_AwaitGraphicsIdle(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_SetShaderVariable(AstralCanvasGraphics ptr, const char* variableName, void* data, usize size);
    DynamicFunction void AstralCanvasGraphics_SetShaderVariableTexture(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D texture);
    DynamicFunction void AstralCanvasGraphics_SetShaderVariableTextures(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D *textures, usize count);
    DynamicFunction void AstralCanvasGraphics_SetShaderVariableSampler(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState sampler);
    DynamicFunction void AstralCanvasGraphics_SetShaderVariableSamplers(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState *samplers, usize count);
    DynamicFunction void AstralCanvasGraphics_SendUpdatedUniforms(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_DrawIndexedPrimitives(AstralCanvasGraphics ptr, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance);
    DynamicFunction void AstralCanvasGraphics_DrawIndexedPrimitivesIndirectCount(AstralCanvasGraphics ptr, AstralCanvasComputeBuffer drawDataBuffer, usize drawDataBufferOffset, AstralCanvasComputeBuffer drawCountBuffer, usize drawCountBufferOffset, u32 maxDrawCount);
    DynamicFunction void AstralCanvasGraphics_DrawIndexedPrimitivesIndirect(AstralCanvasGraphics ptr, AstralCanvasComputeBuffer drawDataBuffer, usize drawDataBufferOffset, u32 drawCount);
    DynamicFunction void AstralCanvasGraphics_NextRenderPass(AstralCanvasGraphics ptr);
    DynamicFunction AstralCanvasClipArea AstralCanvasGraphics_GetClipArea(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_SetClipArea(AstralCanvasGraphics ptr, i32 x, i32 y, i32 w, i32 h);
    DynamicFunction AstralCanvasClipArea AstralCanvasGraphics_GetViewport(AstralCanvasGraphics ptr);
    DynamicFunction void AstralCanvasGraphics_SetViewport(AstralCanvasGraphics ptr, i32 x, i32 y, i32 w, i32 h);

#ifdef __cplusplus
}
#endif