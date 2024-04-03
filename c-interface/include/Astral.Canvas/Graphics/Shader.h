#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Texture2D.h"
#include "Astral.Canvas/Graphics/SamplerState.h"
#include "Astral.Canvas/Graphics/ComputeBuffer.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void *AstralCanvasShaderVariable;
    typedef void *AstralCanvasShader;

    struct AstralCanvasExportedMaterialParam
    {
        const char *name;
        usize size;
    };
    struct AstralCanvasExportedMaterial
    {
        const char *name;
        usize paramsCount;
        AstralCanvasExportedMaterialParam *params;
    };
    DynamicFunction void AstralCanvasExportedMaterial_Deinit(AstralCanvasExportedMaterial material);

    DynamicFunction AstralCanvas_ShaderType AstralCanvasShader_GetType(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule1(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule2(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetPipelineLayout(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_GetVariableBinding(AstralCanvasShader ptr, const char *varName);
    DynamicFunction AstralCanvasShaderVariable AstralCanvasShader_GetVariableAt(AstralCanvasShader ptr, usize at);
    DynamicFunction void AstralCanvasShader_Deinit(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_FromString(AstralCanvas_ShaderType shaderType, const char* jsonString, AstralCanvasShader *result);

    DynamicFunction void AstralCanvasShader_GetExportedMaterials(AstralCanvasShader ptr, AstralCanvasExportedMaterial* materials, u32* numExportedMaterials);

    DynamicFunction const char *AstralCanvasShaderVariable_GetName(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetSet(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetBinding(AstralCanvasShaderVariable ptr);
    DynamicFunction AstralCanvas_ShaderResourceType AstralCanvasShaderVariable_GetType(AstralCanvasShaderVariable ptr);
    DynamicFunction AstralCanvas_ShaderInputAccessedBy AstralCanvasShaderVariable_GetAccessedBy(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetArrayLength(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetSize(AstralCanvasShaderVariable ptr);

    DynamicFunction void AstralCanvasShader_SetShaderVariable(AstralCanvasShader ptr, const char* variableName, void* varPtr, usize size);
    DynamicFunction void AstralCanvasShader_SetShaderVariableTexture(AstralCanvasShader ptr, const char* variableName, AstralCanvasTexture2D texture);
    DynamicFunction void AstralCanvasShader_SetShaderVariableTextures(AstralCanvasShader ptr, const char* variableName, AstralCanvasTexture2D *textures, usize count);
    DynamicFunction void AstralCanvasShader_SetShaderVariableSampler(AstralCanvasShader ptr, const char* variableName, AstralCanvasSamplerState sampler);
    DynamicFunction void AstralCanvasShader_SetShaderVariableSamplers(AstralCanvasShader ptr, const char* variableName, AstralCanvasSamplerState *samplers, usize count);
    DynamicFunction void AstralCanvasShader_SetShaderVariableComputeBuffer(AstralCanvasShader ptr, const char* variableName, AstralCanvasComputeBuffer computeBuffer);

#ifdef __cplusplus
}
#endif