#include "Astral.Canvas/Graphics/Compute.h"
#include "Graphics/Compute.hpp"

exportC AstralCanvasComputePipeline AstralCanvasComputePipeline_Create(AstralCanvasShader shader)
{
    AstralCanvas::ComputePipeline* result = (AstralCanvas::ComputePipeline*)malloc(sizeof(AstralCanvas::ComputePipeline));
    *result = AstralCanvas::ComputePipeline((AstralCanvas::Shader*)shader);
    return (AstralCanvasComputePipeline)result;
}
exportC AstralCanvasShader AstralCanvasComputePipeline_GetShader(AstralCanvasComputePipeline ptr)
{
    return (AstralCanvasShader)((AstralCanvas::ComputePipeline *)ptr)->shader;
}
exportC void AstralCanvasComputePipeline_Deinit(AstralCanvasComputePipeline ptr)
{
    ((AstralCanvas::ComputePipeline *)ptr)->deinit();
    free(ptr);
}
exportC void AstralCanvasComputePipeline_DispatchNow(AstralCanvasComputePipeline ptr, i32 threadsX, i32 threadsY, i32 threadsZ)
{
    ((AstralCanvas::ComputePipeline *)ptr)->DispatchNow(threadsX, threadsY, threadsZ);
}