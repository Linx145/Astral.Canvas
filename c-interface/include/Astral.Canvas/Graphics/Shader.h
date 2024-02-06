#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

extern "C"
{
    typedef void *AstralCanvasShader;

    AstralCanvas_ShaderType AstralCanvasShader_GetType(AstralCanvasShader ptr);
    void *AstralCanvasShader_GetModule1(AstralCanvasShader ptr);
    void *AstralCanvasShader_GetModule2(AstralCanvasShader ptr);
    void *AstralCanvasShader_GetPipelineLayout(AstralCanvasShader ptr);
    i32 AstralCanvasShader_GetVariableBinding(AstralCanvasShader ptr, const char *varName);

    void AstralCanvasShader_Deinit(AstralCanvasShader ptr);

    i32 AstralCanvasShader_FromString(AstralCanvas_ShaderType shaderType, const char* jsonString, AstralCanvasShader *result);
}