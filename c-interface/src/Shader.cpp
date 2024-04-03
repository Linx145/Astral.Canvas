#include "Astral.Canvas/Graphics/Shader.h"
#include "Graphics/Shader.hpp"

exportC void AstralCanvasExportedMaterial_Deinit(AstralCanvasExportedMaterial material)
{
    free(material.params);
}
exportC void AstralCanvasShader_GetExportedMaterials(AstralCanvasShader ptr, AstralCanvasExportedMaterial* materials, u32* numExportedMaterials)
{
    if (numExportedMaterials != NULL)
    {
        *numExportedMaterials = ((AstralCanvas::Shader *)ptr)->usedMaterials.length;
    }
    if (materials != NULL)
    {
        for (usize i = 0; i < ((AstralCanvas::Shader *)ptr)->usedMaterials.length; i++)
        {
            AstralCanvas::ShaderMaterialExport *in = &((AstralCanvas::Shader *)ptr)->usedMaterials.data[i];
            AstralCanvasExportedMaterial out;
            out.name = in->name.buffer;
            out.paramsCount = in->params.length;
            out.params = (AstralCanvasExportedMaterialParam *)malloc(sizeof(AstralCanvasExportedMaterialParam) * in->params.length);
            for (usize j = 0; j < out.paramsCount; j++)
            {
                out.params[j].name = in->params.data[j].name.buffer;
                out.params[j].size = in->params.data[j].size;
            }
            materials[i] = out;
        }
    }
}
exportC AstralCanvas_ShaderType AstralCanvasShader_GetType(AstralCanvasShader ptr)
{
    return (AstralCanvas_ShaderType)((AstralCanvas::Shader *)ptr)->shaderType;
}
exportC void *AstralCanvasShader_GetModule1(AstralCanvasShader ptr)
{
    return ((AstralCanvas::Shader *)ptr)->shaderModule1;
}
exportC void *AstralCanvasShader_GetModule2(AstralCanvasShader ptr)
{
    return ((AstralCanvas::Shader *)ptr)->shaderModule2;
}
exportC void *AstralCanvasShader_GetPipelineLayout(AstralCanvasShader ptr)
{
    return ((AstralCanvas::Shader *)ptr)->shaderPipelineLayout;
}
exportC i32 AstralCanvasShader_GetVariableBinding(AstralCanvasShader ptr, const char *varName)
{
    return ((AstralCanvas::Shader *)ptr)->GetVariableBinding(varName);
}
exportC AstralCanvasShaderVariable AstralCanvasShader_GetVariableAt(AstralCanvasShader ptr, usize at)
{
    return ((AstralCanvas::Shader *)ptr)->shaderVariables.uniforms.Get(at);
}
exportC void AstralCanvasShader_Deinit(AstralCanvasShader ptr)
{
    ((AstralCanvas::Shader *)ptr)->deinit();
}
exportC i32 AstralCanvasShader_FromString(const char* jsonString, AstralCanvasShader *result)
{
    if (jsonString == NULL)
    {
        return -1;
    }
    AstralCanvas::Shader *shader = (AstralCanvas::Shader *)GetCAllocator().Allocate(sizeof(AstralCanvas::Shader));
    string inputString = string(GetCAllocator(), jsonString);
    i32 errorLine = AstralCanvas::CreateShaderFromString(GetCAllocator(), inputString, shader);

    *result = shader;
    return errorLine;
}

exportC const char *AstralCanvasShaderVariable_GetName(AstralCanvasShaderVariable ptr)
{
    return ((AstralCanvas::ShaderResource*)ptr)->variableName.buffer;
}
exportC u32 AstralCanvasShaderVariable_GetSet(AstralCanvasShaderVariable ptr)
{
    return ((AstralCanvas::ShaderResource *)ptr)->set;
}
exportC u32 AstralCanvasShaderVariable_GetBinding(AstralCanvasShaderVariable ptr)
{
    return ((AstralCanvas::ShaderResource *)ptr)->binding;
}
exportC AstralCanvas_ShaderResourceType AstralCanvasShaderVariable_GetType(AstralCanvasShaderVariable ptr)
{
    return (AstralCanvas_ShaderResourceType)((AstralCanvas::ShaderResource *)ptr)->type;
}
exportC AstralCanvas_ShaderInputAccessedBy AstralCanvasShaderVariable_GetAccessedBy(AstralCanvasShaderVariable ptr)
{
    return (AstralCanvas_ShaderInputAccessedBy)((AstralCanvas::ShaderResource *)ptr)->accessedBy;
}
exportC u32 AstralCanvasShaderVariable_GetArrayLength(AstralCanvasShaderVariable ptr)
{
    return ((AstralCanvas::ShaderResource *)ptr)->arrayLength;
}
exportC u32 AstralCanvasShaderVariable_GetSize(AstralCanvasShaderVariable ptr)
{
    return ((AstralCanvas::ShaderResource *)ptr)->size;
}

exportC void AstralCanvasShader_SetShaderVariable(AstralCanvasShader ptr, const char* variableName, void* varPtr, usize size)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariable(variableName, varPtr, size);
}
exportC void AstralCanvasShader_SetShaderVariableTexture(AstralCanvasShader ptr, const char* variableName, AstralCanvasTexture2D texture)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariableTexture(variableName, (AstralCanvas::Texture2D*)texture);
}
exportC void AstralCanvasShader_SetShaderVariableTextures(AstralCanvasShader ptr, const char* variableName, AstralCanvasTexture2D *textures, usize count)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariableTextures(variableName, (AstralCanvas::Texture2D**)textures, count);
}
exportC void AstralCanvasShader_SetShaderVariableSampler(AstralCanvasShader ptr, const char* variableName, AstralCanvasSamplerState sampler)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariableSampler(variableName, (AstralCanvas::SamplerState*)sampler);
}
exportC void AstralCanvasShader_SetShaderVariableSamplers(AstralCanvasShader ptr, const char* variableName, AstralCanvasSamplerState *samplers, usize count)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariableSamplers(variableName, (AstralCanvas::SamplerState**)samplers, count);
}
exportC void AstralCanvasShader_SetShaderVariableComputeBuffer(AstralCanvasShader ptr, const char* variableName, AstralCanvasComputeBuffer computeBuffer)
{
    ((AstralCanvas::Shader *)ptr)->SetShaderVariableComputeBuffer(variableName, (AstralCanvas::ComputeBuffer*)computeBuffer);
}