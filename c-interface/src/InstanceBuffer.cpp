#include "Astral.Canvas/Graphics/InstanceBuffer.h"
#include "Graphics/InstanceBuffer.hpp"

exportC usize AstralCanvasInstanceBuffer_GetInstanceSize(AstralCanvasInstanceBuffer ptr)
{
    return ((AstralCanvas::InstanceBuffer *)ptr)->instanceSize;
}
exportC usize AstralCanvasInstanceBuffer_GetCount(AstralCanvasInstanceBuffer ptr)
{
    return ((AstralCanvas::InstanceBuffer *)ptr)->instanceCount;
}
exportC AstralCanvasInstanceBuffer AstralCanvasInstanceBuffer_Create(usize instanceSize, usize instanceCount, bool canRead)
{
    AstralCanvas::InstanceBuffer *result = (AstralCanvas::InstanceBuffer *)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::InstanceBuffer));
    *result = AstralCanvas::InstanceBuffer(instanceSize, instanceCount, canRead);
    return result;
}
exportC void AstralCanvasInstanceBuffer_Deinit(AstralCanvasInstanceBuffer ptr)
{
    ((AstralCanvas::InstanceBuffer *)ptr)->deinit();
    GetDefaultAllocator()->Free(ptr);
}
exportC void AstralCanvasInstanceBuffer_SetData(AstralCanvasInstanceBuffer ptr, void* instanceData, usize instanceCount)
{
    ((AstralCanvas::InstanceBuffer *)ptr)->SetData(instanceData, instanceCount);
}