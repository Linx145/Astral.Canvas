#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void *AstralCanvasInstanceBuffer;

    DynamicFunction usize AstralCanvasInstanceBuffer_GetInstanceSize(AstralCanvasInstanceBuffer ptr);
    DynamicFunction usize AstralCanvasInstanceBuffer_GetCount(AstralCanvasInstanceBuffer ptr);
    DynamicFunction AstralCanvasInstanceBuffer AstralCanvasInstanceBuffer_Create(usize instanceSize, usize instanceCount, bool canRead);
    DynamicFunction void AstralCanvasInstanceBuffer_Deinit(AstralCanvasInstanceBuffer ptr);
    DynamicFunction void AstralCanvasInstanceBuffer_SetData(AstralCanvasInstanceBuffer ptr, void* instanceData, usize instanceCount);
#ifdef __cplusplus
}
#endif