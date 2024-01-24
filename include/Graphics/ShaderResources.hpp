#pragma once
#include "Linxc.h"
#include "vector.hpp"
#include "string.hpp"

enum AstralCanvas_ShaderType
{
    AstralCanvasShader_VertexFragment,
    AstralCanvasShader_Compute
};
struct AstralCanvasShaderUniform
{
    string variableName;
    u32 set;
    u32 binding;
    usize size;
};
struct AstralCanvasShaderResource
{
    string variableName;
    u32 set;
    u32 binding;
    u32 arrayLength;
};
struct AstralCanvasShaderVariables
{
    collections::Array<AstralCanvasShaderUniform> uniforms;
    collections::Array<AstralCanvasShaderResource> textures;
    collections::Array<AstralCanvasShaderResource> samplers;

    inline AstralCanvasShaderVariables()
    {
        uniforms = collections::Array<AstralCanvasShaderUniform>();
        textures = collections::Array<AstralCanvasShaderResource>();
        samplers = collections::Array<AstralCanvasShaderResource>();
    }
    inline AstralCanvasShaderVariables(IAllocator *allocator)
    {
        uniforms = collections::Array<AstralCanvasShaderUniform>(allocator);
        textures = collections::Array<AstralCanvasShaderResource>(allocator);
        samplers = collections::Array<AstralCanvasShaderResource>(allocator);
    }
    inline void deinit()
    {
        uniforms.deinit();
        textures.deinit();
        samplers.deinit();
    }
};
typedef void *AstralCanvasShaderModule;

typedef void *AstralCanvasPipelineLayout;