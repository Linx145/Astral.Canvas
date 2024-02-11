#include "Astral.Canvas/Graphics/VertexDeclaration.h"
#include "Graphics/VertexDeclarations.hpp"

exportC void AstralCanvasVertexDeclaration_AddElement(AstralCanvasVertexDeclaration declaration, AstralCanvasVertexElement element)
{
    AstralCanvas::VertexElement newElement;
    newElement.format = (AstralCanvas::VertexElementFormat)element.format;
    newElement.offset = element.offset;
    ((AstralCanvas::VertexDeclaration *)declaration)->elements.Add(newElement);
}
exportC AstralCanvasVertexDeclaration AstralCanvasVertexDeclaration_Create(usize size, AstralCanvas_VertexInputRate vertexInputRate)
{
    AstralCanvas::VertexDeclaration *result = (AstralCanvas::VertexDeclaration *)GetDefaultAllocator()->Allocate(sizeof(AstralCanvas::VertexDeclaration));
    *result = AstralCanvas::VertexDeclaration(GetDefaultAllocator(), size, (AstralCanvas::VertexInputRate)vertexInputRate);
    return result;
}
exportC usize AstralCanvasVertexDeclaration_GetElementSize(AstralCanvasVertexDeclaration ptr)
{
    return ((AstralCanvas::VertexDeclaration *)ptr)->size;
}
exportC AstralCanvas_VertexInputRate AstralCanvasVertexDeclaration_GetInputRate(AstralCanvasVertexDeclaration ptr)
{
    return (AstralCanvas_VertexInputRate)((AstralCanvas::VertexDeclaration *)ptr)->inputRate;
}
exportC void AstralCanvasVertexDeclaration_GetElements(AstralCanvasVertexDeclaration ptr, usize* outputCount, AstralCanvasVertexElement *outputData)
{
    if (outputData != NULL)
    {
        for (usize i = 0; i < ((AstralCanvas::VertexDeclaration *)ptr)->elements.count; i++)
        {
            outputData[i].format = (AstralCanvas_VertexElementFormat)((AstralCanvas::VertexDeclaration *)ptr)->elements.ptr[i].format;
            outputData[i].offset = ((AstralCanvas::VertexDeclaration *)ptr)->elements.ptr[i].offset;
        }
    }
    if (outputCount != NULL)
    {
        *outputCount = ((AstralCanvas::VertexDeclaration *)ptr)->elements.count;
    }
}
exportC AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorDecl()
{
    return AstralCanvas::GetVertexPositionColorDecl();
}
exportC AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorTextureDecl()
{
    return AstralCanvas::GetVertexPositionColorTextureDecl();
}
exportC AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionTextureColorDecl()
{
    return AstralCanvas::GetVertexPositionTextureColorDecl();
}
exportC AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionNormalTextureDecl()
{
    return AstralCanvas::GetVertexPositionNormalTextureDecl();
}
exportC AstralCanvasVertexDeclaration AstralCanvasGetInstanceDataMatrixDecl()
{
    return AstralCanvas::GetInstanceDataMatrixDecl();
}