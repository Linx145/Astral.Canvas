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
    struct ShaderMaterialExportParam
    {
        string name;
        usize size;
    };
    struct ShaderMaterialExport
    {
        string name;
        collections::Array<ShaderMaterialExportParam> params;

        inline void deinit()
        {
            name.deinit();
            for (usize i = 0; i < params.length; i++)
            {
                params.data[i].name.deinit();
            }
            params.deinit();
        }
    };
    struct Shader
    {
        IAllocator allocator;
        ShaderType shaderType;
        ShaderModule shaderModule1;
        ShaderModule shaderModule2;
        ShaderVariables shaderVariables;
        PipelineLayout shaderPipelineLayout;
        bool uniformsHasBeenSet;

        collections::Array<ShaderMaterialExport> usedMaterials;

        usize descriptorForThisDrawCall;
        collections::vector<void *> descriptorSets;

        i32 GetVariableBinding(const char* variableName);
        void CheckDescriptorSetAvailability(bool forceAddNewDescriptor = false);
        void SyncUniformsWithGPU(void *commandEncoder);

        void SetShaderVariable(const char* variableName, void* ptr, usize size);
        void SetShaderVariableTexture(const char* variableName, Texture2D *texture);
        void SetShaderVariableTextures(const char* variableName, Texture2D **textures, usize count);
        void SetShaderVariableSampler(const char* variableName, SamplerState *sampler);
        void SetShaderVariableSamplers(const char* variableName, SamplerState **samplers, usize count);
        void SetShaderVariableComputeBuffer(const char* variableName, ComputeBuffer* computeBuffer);

        Shader();
        Shader(IAllocator allocator, ShaderType type);
        void deinit();
    };

    void ParseShaderVariables(Json::JsonElement *json, ShaderVariables *results, ShaderInputAccessedBy accessedByShaderOfType);

    i32 CreateShaderFromString(IAllocator allocator, ShaderType shaderType, string jsonString, Shader *result);
}
