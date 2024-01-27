#pragma once
#include "Linxc.h"
#include "string.hpp"
#include "array.hpp"
#include "Graphics/ShaderResources.hpp"

namespace AstralCanvas
{
    struct Shader
    {
        ShaderType shaderType;
        ShaderModule shaderModule1;
        ShaderModule shaderModule2;
        PipelineLayout shaderPipelineLayout;
        //PipelineLayout shaderPipelineLayout1;
        //PipelineLayout shaderPipelineLayout2;
        ShaderVariables shaderVariables;

        Shader();
        Shader(IAllocator *allocator, ShaderType type);
        void deinit();
    };

    void ParseShaderVariables(SomnialJson::JsonElement *json, ShaderVariables *results, ShaderInputAccessedBy accessedByShaderOfType);

    i32 CreateShaderFromString(IAllocator *allocator, ShaderType shaderType, string jsonString, Shader* result);
}

//i32 AstralCanvas_CreateShaderFromString(IAllocator *allocator, AstralCanvas_ShaderType shaderType, string jsonString, AstralCanvasShader* result);