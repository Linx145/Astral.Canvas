#pragma once
#include "Linxc.h"
#include "string.hpp"
#include "array.hpp"
#include "Graphics/ShaderResources.hpp"
#include "Graphics/Enums.hpp"
#include "Json.hpp"

#define MAX_UNIFORMS_IN_SHADER 16

#ifdef ASTRALCANVAS_VULKAN
#include "vulkan/vulkan.h"
#endif

namespace AstralCanvas
{
    struct Shader
    {
        IAllocator *allocator;
        ShaderType shaderType;
        ShaderModule shaderModule1;
        ShaderModule shaderModule2;
        ShaderVariables shaderVariables;
        PipelineLayout shaderPipelineLayout;
        bool uniformsHasBeenSet;

        usize descriptorForThisDrawCall;
        collections::vector<void *> descriptorSets;

        i32 GetVariableBinding(const char* variableName);
        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(void *commandEncoder);

        Shader();
        Shader(IAllocator *allocator, ShaderType type);
        void deinit();
    };

    void ParseShaderVariables(SomnialJson::JsonElement *json, ShaderVariables *results, ShaderInputAccessedBy accessedByShaderOfType);

    i32 CreateShaderFromString(IAllocator *allocator, ShaderType shaderType, string jsonString, Shader *result);
}

//i32 AstralCanvas_CreateShaderFromString(IAllocator *allocator, AstralCanvas_ShaderType shaderType, string jsonString, AstralCanvasShader* result);
