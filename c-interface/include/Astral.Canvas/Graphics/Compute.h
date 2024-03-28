#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Shader.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasComputePipeline;
    DynamicFunction AstralCanvasComputePipeline AstralCanvasComputePipeline_Create(AstralCanvasShader shader);
    DynamicFunction AstralCanvasShader AstralCanvasComputePipeline_GetShader(AstralCanvasComputePipeline *ptr);
    DynamicFunction void AstralCanvasComputePipeline_Deinit(AstralCanvasComputePipeline *ptr);
    DynamicFunction void AstralCanvasComputePipeline_DispatchNow(AstralCanvasComputePipeline *ptr, i32 threadsX, i32 threadsY, i32 threadsZ);

#ifdef __cplusplus
}
#endif