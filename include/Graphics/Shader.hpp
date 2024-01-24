#pragma once
#include "Linxc.h"
#include "string.hpp"
#include "array.hpp"
#include "Graphics/ShaderResources.hpp"

struct AstralCanvasShader
{
    AstralCanvas_ShaderType shaderType;
    AstralCanvasShaderModule shaderModule1;
    AstralCanvasShaderModule shaderModule2;
    AstralCanvasPipelineLayout shaderPipelineLayout1;
    AstralCanvasPipelineLayout shaderPipelineLayout2;
    AstralCanvasShaderVariables shaderVariables;

    AstralCanvasShader();
    AstralCanvasShader(IAllocator *allocator, AstralCanvas_ShaderType type);
    void deinit();
};

i32 AstralCanvas_CreateShaderFromString(IAllocator *allocator, AstralCanvas_ShaderType shaderType, string jsonString, AstralCanvasShader* result);