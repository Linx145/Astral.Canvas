#include "Astral.Canvas/Graphics/ComputeBuffer.h"
#include "allocators.hpp"
#include "Graphics/ComputeBuffer.hpp"

exportC usize AstralCanvasComputeBuffer_Create_GetSize(AstralCanvasComputeBuffer ptr)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    return buffer->elementSize;
}
exportC usize AstralCanvasComputeBuffer_Create_GetCount(AstralCanvasComputeBuffer ptr)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    return buffer->elementCount;
}
exportC AstralCanvasComputeBuffer AstralCanvasComputeBuffer_Create(usize elementSize, usize elementCount, bool accessedAsVertexBuffer, bool CPUCanRead)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer*)GetCAllocator().Allocate(sizeof(AstralCanvas::ComputeBuffer));
    *buffer = AstralCanvas::ComputeBuffer(elementSize, elementCount, accessedAsVertexBuffer, CPUCanRead);
    return buffer;
}
exportC void AstralCanvasComputeBuffer_Deinit(AstralCanvasComputeBuffer ptr)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    buffer->deinit();
    free(buffer);
}
exportC void AstralCanvasComputeBuffer_SetData(AstralCanvasComputeBuffer ptr, u8* bytes, usize elementsToSet)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    buffer->SetData(bytes, elementsToSet);
}
exportC void *AstralCanvasComputeBuffer_GetData(AstralCanvasComputeBuffer ptr, usize* dataLength)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    return buffer->GetData(GetCAllocator(), dataLength);
}
exportC bool AstralCanvasComputeBuffer_GetCanAccessAsVertexBuffer(AstralCanvasComputeBuffer ptr)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    return buffer->accessedAsVertexBuffer;
}
exportC bool AstralCanvasComputeBuffer_GetCPUCanRead(AstralCanvasComputeBuffer ptr)
{
    AstralCanvas::ComputeBuffer *buffer = (AstralCanvas::ComputeBuffer *)ptr;
    return buffer->CPUCanRead;
}
exportC void AstralCanvasComputeBuffer_DisposeGottenData(void* ptr)
{
    if (ptr != NULL)
    {
        free(ptr);
    }
}