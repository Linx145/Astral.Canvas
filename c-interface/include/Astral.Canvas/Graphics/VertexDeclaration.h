#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef struct
    {
        AstralCanvas_VertexElementFormat format;
        usize offset;
    } AstralCanvasVertexElement;

    typedef void *AstralCanvasVertexDeclaration;

    DynamicFunction void AstralCanvasVertexDeclaration_AddElement(AstralCanvasVertexElement element);
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasVertexDeclaration_Create(usize size, AstralCanvas_VertexInputRate vertexInputRate);
    DynamicFunction usize AstralCanvasVertexDeclaration_GetElementSize(AstralCanvasVertexDeclaration ptr);
    DynamicFunction AstralCanvas_VertexInputRate AstralCanvasVertexDeclaration_GetInputRate(AstralCanvasVertexDeclaration ptr);
    DynamicFunction void AstralCanvasVertexDeclaration_GetElements(AstralCanvasVertexDeclaration ptr, usize* outputCount, AstralCanvasVertexElement *outputData);
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorDecl();
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorTextureDecl();
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionTextureColorDecl();
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionNormalTextureDecl();
    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasGetInstanceDataMatrixDecl();
#ifdef __cplusplus
}
#endif