#pragma once
#include "Linxc.h"
#include "sparseset.hpp"
#include "vector.hpp"
#include "string.hpp"
#include "Graphics/Enums.hpp"
#include "Graphics/UniformBuffer.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/SamplerState.hpp"

namespace AstralCanvas
{
    struct ShaderStagingMutableState
    {
        union
        {
            struct
            {
                UniformBuffer ub;
                bool ownsUniformBuffer;
            };
            struct
            {
                collections::Array<Texture2D*> textures;
                void *imageInfos;
            };
            struct
            {
                collections::Array<SamplerState*> samplers;
                void *samplerInfos;
            };
        };
        bool mutated;
    };
    struct ShaderResource
    {
        string variableName;
        u32 set;
        u32 binding;
        ShaderResourceType type;
        ShaderInputAccessedBy accessedBy;
        union
        {
            u32 arrayLength;
            u32 inputAttachmentIndex;
        };
        u32 size;

        /// A collection of all data that is awaiting submission
        collections::vector<ShaderStagingMutableState> stagingData;
    };
    struct ShaderVariables
    {
        IAllocator *allocator;
        collections::sparseset<ShaderResource> uniforms;

        ShaderVariables();
        ShaderVariables(IAllocator *allocator);
        void deinit();
    };
    typedef void *ShaderModule;

    typedef void *PipelineLayout;
}