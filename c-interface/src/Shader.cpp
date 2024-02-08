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
    AstralCanvas::Shader *shader = (AstralCanvas::Shader *)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::Shader));
    string inputString = string(GetDefaultAllocator(), jsonString);
    i32 errorLine = AstralCanvas::CreateShaderFromString(GetDefaultAllocator(), (AstralCanvas::ShaderType)shaderType, inputString, shader);

    *result = shader;
    return errorLine;
}