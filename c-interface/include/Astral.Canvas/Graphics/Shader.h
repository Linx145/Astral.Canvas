#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasShader;

    DynamicFunction AstralCanvas_ShaderType AstralCanvasShader_GetType(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule1(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetModule2(AstralCanvasShader ptr);
    DynamicFunction void *AstralCanvasShader_GetPipelineLayout(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_GetVariableBinding(AstralCanvasShader ptr, const char *varName);
    DynamicFunction void AstralCanvasShader_Deinit(AstralCanvasShader ptr);
    DynamicFunction i32 AstralCanvasShader_FromString(AstralCanvas_ShaderType shaderType, const char* jsonString, AstralCanvasShader *result);

#ifdef __cplusplus
}
#endif