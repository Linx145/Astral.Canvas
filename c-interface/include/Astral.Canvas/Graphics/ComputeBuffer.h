#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasComputeBuffer;

    DynamicFunction usize AstralCanvasComputeBuffer_ElementGetSize(AstralCanvasComputeBuffer ptr);
    DynamicFunction usize AstralCanvasComputeBuffer_ElementGetCount(AstralCanvasComputeBuffer ptr);
    DynamicFunction AstralCanvasComputeBuffer AstralCanvasComputeBuffer_Create(usize elementSize, usize elementCount, bool accessedAsVertexBuffer, bool CPUCanRead);
    DynamicFunction void AstralCanvasComputeBuffer_Deinit(AstralCanvasComputeBuffer ptr);
    DynamicFunction void AstralCanvasComputeBuffer_SetData(AstralCanvasComputeBuffer ptr, u8* bytes, usize elementsToSet);
    DynamicFunction void *AstralCanvasComputeBuffer_GetData(AstralCanvasComputeBuffer ptr, usize* dataLength);
    DynamicFunction bool AstralCanvasComputeBuffer_GetCanAccessAsVertexBuffer(AstralCanvasComputeBuffer ptr);
    DynamicFunction bool AstralCanvasComputeBuffer_GetCPUCanRead(AstralCanvasComputeBuffer ptr);
    DynamicFunction void AstralCanvasComputeBuffer_DisposeGottenData(void* ptr);

#ifdef __cplusplus
}
#endif