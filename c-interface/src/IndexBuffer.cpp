#include "Astral.Canvas/Graphics/IndexBuffer.h"
#include "Graphics/IndexBuffer.hpp"
#include "allocators.hpp"

exportC AstralCanvas_IndexBufferSize AstralCanvasIndexBuffer_GetSize(AstralCanvasIndexBuffer ptr)
{
    return (AstralCanvas_IndexBufferSize)((AstralCanvas::IndexBuffer *)ptr)->indexElementSize;
}
exportC usize AstralCanvasIndexBuffer_GetCount(AstralCanvasIndexBuffer ptr)
{
    return ((AstralCanvas::IndexBuffer *)ptr)->indexCount;
}
exportC AstralCanvasIndexBuffer AstralCanvasIndexBuffer_Create(AstralCanvas_IndexBufferSize indexSize, usize indexCount)
{
    AstralCanvas::IndexBuffer *result = (AstralCanvas::IndexBuffer *)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::IndexBuffer));
    *result = AstralCanvas::IndexBuffer((AstralCanvas::IndexBufferSize)indexSize, indexCount);
    return result;
}
exportC void AstralCanvasIndexBuffer_Deinit(AstralCanvasIndexBuffer ptr)
{
    ((AstralCanvas::IndexBuffer *)ptr)->deinit();
    GetDefaultAllocator()->Free(ptr);
}
exportC void AstralCanvasIndexBuffer_SetData(AstralCanvasIndexBuffer ptr, u8* bytes, usize sizeOfBytes)
{
    ((AstralCanvas::IndexBuffer *)ptr)->SetData(bytes, sizeOfBytes);
}