#pragma once
#include "Linxc.h"
#include "hashmap.hpp"
#include "string.hpp"
#include "Graphics/Enums.hpp"

namespace AstralCanvas
{
    struct ShaderResource
    {
        string variableName;
        u32 set;
        u32 binding;
        ShaderResourceType type;
        ShaderInputAccessedBy accessedBy;
        u32 arrayLength;
        u32 size;
    };
    struct ShaderVariables
    {
        IAllocator *allocator;
        collections::hashmap<i32, ShaderResource> uniforms;

        ShaderVariables();
        ShaderVariables(IAllocator *allocator);
        void deinit();
    };
    typedef void *ShaderModule;

    typedef void *PipelineLayout;
}