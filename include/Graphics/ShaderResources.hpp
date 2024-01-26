#pragma once
#include "Linxc.h"
#include "vector.hpp"
#include "string.hpp"

namespace AstralCanvas
{
    enum ShaderType
    {
        ShaderType_VertexFragment,
        ShaderType_Compute
    };
    struct ShaderUniform
    {
        string variableName;
        u32 set;
        u32 binding;
        usize size;
    };
    struct ShaderResource
    {
        string variableName;
        u32 set;
        u32 binding;
        u32 arrayLength;
    };
    struct ShaderVariables
    {
        collections::Array<ShaderUniform> uniforms;
        collections::Array<ShaderResource> textures;
        collections::Array<ShaderResource> samplers;

        ShaderVariables();
        ShaderVariables(IAllocator *allocator);
        void deinit();
        // inline ShaderVariables()
        // {
        //     uniforms = collections::Array<ShaderUniform>();
        //     textures = collections::Array<ShaderResource>();
        //     samplers = collections::Array<ShaderResource>();
        // }
        // inline ShaderVariables(IAllocator *allocator)
        // {
        //     uniforms = collections::Array<ShaderUniform>(allocator);
        //     textures = collections::Array<ShaderResource>(allocator);
        //     samplers = collections::Array<ShaderResource>(allocator);
        // }
        // inline void deinit()
        // {
        //     uniforms.deinit();
        //     textures.deinit();
        //     samplers.deinit();
        // }
    };
    typedef void *ShaderModule;

    typedef void *PipelineLayout;
}