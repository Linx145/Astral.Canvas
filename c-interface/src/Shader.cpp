#include "Astral.Canvas/Graphics/Shader.h"
#include "Graphics/Shader.hpp"

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
exportC i32 AstralCanvasShader_FromString(AstralCanvas_ShaderType shaderType, const char* jsonString, AstralCanvasShader *result)
{
    if (jsonString == NULL)
    {
        return -1;
    }
    AstralCanvas::Shader *shader = (AstralCanvas::Shader *)GetCAllocator().Allocate(sizeof(AstralCanvas::Shader));
    string inputString = string(GetCAllocator(), jsonString);
    i32 errorLine = AstralCanvas::CreateShaderFromString(GetCAllocator(), (AstralCanvas::ShaderType)shaderType, inputString, shader);

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