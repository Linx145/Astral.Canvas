#pragma once
#include "Linxc.h"
#include "Maths/Rectangle.hpp"
#include "Graphics/RenderProgram.hpp"
#include "Graphics/RenderPipeline.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/SamplerState.hpp"

namespace AstralCanvas
{
    struct Graphics
    {
        RenderProgram *currentRenderProgram;
        u32 currentRenderPass;
        RenderTarget *currentRenderTarget;
        RenderPipeline *currentRenderPipeline;
        
        void* currentCommandEncoderInstance;

        Maths::Rectangle Viewport;
        Maths::Rectangle ClipArea;

        Graphics();

        void SetVertexBuffer(const VertexBuffer *vb, u32 bindingPoint = 0);
        void SetIndexBuffer(const IndexBuffer *indexBuffer);
        void SetRenderTarget(RenderTarget *target);
        void StartRenderProgram(RenderProgram *program, const Color clearColor);
        void EndRenderProgram();
        void UseRenderPipeline(RenderPipeline *pipeline);

        void SetShaderVariable(const char* variableName, void* ptr, usize size);
        void SetShaderVariableTexture(const char* variableName, Texture2D *texture);
        void SetShaderVariableSampler(const char* variableName, SamplerState *sampler);

        void SendUpdatedUniforms();

        void DrawIndexedPrimitives(u32 indexCount, u32 instanceCount, u32 firstIndex = 0, u32 vertexOffset = 0, u32 firstInstance = 0);
    };
}
