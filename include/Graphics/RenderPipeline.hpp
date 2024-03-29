#pragma once
#include "Linxc.h"
#include "hashmap.hpp"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/BlendState.hpp"
#include "Graphics/RenderProgram.hpp"

namespace AstralCanvas
{
    //Because the creation of a renderpipeline handles requires a render program and pass,
    //we should cache and reuse renderpipeline handles wherever possible, like when
    //using the same pipeline for the same pass
    struct RenderPipelineBindZone
    {
        void *renderProgramHandle;
        u32 subPassHandle;
    };
    inline u32 RenderPipelineBindZoneHash(RenderPipelineBindZone zone)
    {
        u32 hash = 7;
        hash = hash * 31 + (u32)(usize)zone.renderProgramHandle;
        hash = hash * 31 + zone.subPassHandle;
        return hash;
    }
    inline bool RenderPipelineBindZoneEql(RenderPipelineBindZone A, RenderPipelineBindZone B)
    {
        return A.renderProgramHandle == B.renderProgramHandle && A.subPassHandle == B.subPassHandle;
    }
    struct RenderPipeline
    {
        collections::hashmap<RenderPipelineBindZone, void *> zoneToPipelineInstance;
        void *layout;
        Shader *shader;
        collections::Array<VertexDeclaration*> vertexDeclarations;
        CullMode cullMode;
        PrimitiveType primitiveType;
        BlendState blendState;
        bool depthWrite;
        bool depthTest;

        void deinit();
        /// Retrieves or creates an instance of this pipeline for use in the given render program and pass.
        void *GetOrCreateFor(AstralCanvas::RenderProgram *renderProgram, u32 renderPassToUse);

        RenderPipeline();
        RenderPipeline(IAllocator allocator, Shader *pipelineShader, CullMode pipelineCullMode, PrimitiveType pipelinePrimitiveType, BlendState pipelineBlendState, bool testDepth, bool writeToDepth, collections::Array<VertexDeclaration*> pipelineVertexDeclarations);
    };
}