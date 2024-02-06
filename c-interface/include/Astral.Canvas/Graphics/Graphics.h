#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/IndexBuffer.h"
#include "Astral.Canvas/Graphics/VertexBuffer.h"
#include "Astral.Canvas/Graphics/RenderTarget.h"
#include "Astral.Canvas/Graphics/Texture2D.h"
#include "Astral.Canvas/Graphics/SamplerState.h"
#include "Astral.Canvas/Graphics/RenderProgram.h"
#include "Astral.Canvas/Graphics/RenderPipeline.h"
#include "Astral.Canvas/Graphics/Color.h"

extern "C"
{
    typedef void *AstralCanvasGraphics;

    AstralCanvasRenderProgram AstralCanvasGraphics_GetCurrentRenderProgram(AstralCanvasGraphics ptr);
    AstralCanvasRenderTarget AstralCanvasGraphics_GetCurrentRenderTarget(AstralCanvasGraphics ptr);
    u32 AstralCanvasGraphics_GetCurrentRenderProgramPass(AstralCanvasGraphics ptr);

    void AstralCanvasGraphics_SetVertexBuffer(AstralCanvasGraphics ptr, const AstralCanvasVertexBuffer vb, u32 bindingPoint);
    void AstralCanvasGraphics_SetIndexBuffer(AstralCanvasGraphics ptr, const AstralCanvasIndexBuffer indexBuffer);
    void AstralCanvasGraphics_SetRenderTarget(AstralCanvasGraphics ptr, AstralCanvasRenderTarget target);
    void AstralCanvasGraphics_StartRenderProgram(AstralCanvasGraphics ptr, AstralCanvasRenderProgram program, AstralCanvasColor clearColor);
    void AstralCanvasGraphics_EndRenderProgram(AstralCanvasGraphics ptr);
    void AstralCanvasGraphics_UseRenderPipeline(AstralCanvasGraphics ptr, AstralCanvasRenderPipeline pipeline);

    void AstralCanvasGraphics_AwaitGraphicsIdle(AstralCanvasGraphics ptr);

    void AstralCanvasGraphics_SetShaderVariable(AstralCanvasGraphics ptr, const char* variableName, void* data, usize size);
    void AstralCanvasGraphics_SetShaderVariableTexture(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D texture);
    void AstralCanvasGraphics_SetShaderVariableTextures(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D *textures, usize count);
    void AstralCanvasGraphics_SetShaderVariableSampler(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState sampler);
    void AstralCanvasGraphics_SetShaderVariableSamplers(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState *samplers, usize count);

    void AstralCanvasGraphics_SendUpdatedUniforms(AstralCanvasGraphics ptr);

    void AstralCanvasGraphics_DrawIndexedPrimitives(AstralCanvasGraphics ptr, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance);
}