#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void *AstralCanvasShaderVariable;
    typedef void *AstralCanvasShader;

    DynamicFunction AstralCanvas_ShaderType AstralCanvasShader_GetType(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule1(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule2(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetPipelineLayout(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_GetVariableBinding(AstralCanvasShader ptr, const char *varName);
    DynamicFunction AstralCanvasShaderVariable AstralCanvasShader_GetVariableAt(AstralCanvasShader ptr, usize at);
    DynamicFunction void AstralCanvasShader_Deinit(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_FromString(AstralCanvas_ShaderType shaderType, const char* jsonString, AstralCanvasShader *result);

    DynamicFunction const char *AstralCanvasShaderVariable_GetName(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetSet(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetBinding(AstralCanvasShaderVariable ptr);
    DynamicFunction AstralCanvas_ShaderResourceType AstralCanvasShaderVariable_GetType(AstralCanvasShaderVariable ptr);
    DynamicFunction AstralCanvas_ShaderInputAccessedBy AstralCanvasShaderVariable_GetAccessedBy(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetArrayLength(AstralCanvasShaderVariable ptr);
    DynamicFunction u32 AstralCanvasShaderVariable_GetSize(AstralCanvasShaderVariable ptr);

#ifdef __cplusplus
}
#endif