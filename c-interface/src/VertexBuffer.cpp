#include "Astral.Canvas/Graphics/VertexBuffer.h"
#include "Graphics/VertexBuffer.hpp"

exportC AstralCanvasVertexDeclaration AstralCanvasVertexBuffer_GetVertexDeclaration(AstralCanvasVertexBuffer ptr)
{
    return (AstralCanvasVertexDeclaration)((AstralCanvas::VertexBuffer *)ptr)->vertexType;
}
exportC usize AstralCanvasVertexBuffer_GetCount(AstralCanvasVertexBuffer ptr)
{
    return ((AstralCanvas::VertexBuffer *)ptr)->vertexCount;
}
exportC AstralCanvasVertexBuffer AstralCanvasVertexBuffer_Create(AstralCanvasVertexDeclaration thisVertexType, usize vertexCount, bool isDynamic, bool canRead)
{
    AstralCanvas::VertexBuffer *buffer = (AstralCanvas::VertexBuffer*)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::VertexBuffer));
    *buffer = AstralCanvas::VertexBuffer((AstralCanvas::VertexDeclaration*)thisVertexType, vertexCount, isDynamic, canRead);
    return buffer;
}
exportC void AstralCanvasVertexBuffer_Deinit(AstralCanvasVertexBuffer ptr)
{
    ((AstralCanvas::VertexBuffer *)ptr)->deinit();
}
exportC void AstralCanvasVertexBuffer_SetData(AstralCanvasVertexBuffer ptr, void *verticesData, usize verticesCount)
{
    ((AstralCanvas::VertexBuffer *)ptr)->SetData(verticesData, verticesCount);
}