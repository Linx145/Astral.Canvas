#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

extern "C"
{
    typedef struct
    {
        AstralCanvas_VertexElementFormat format;
        usize offset;
    } AstralCanvasVertexElement;

    typedef void *AstralCanvasVertexDeclaration;

    usize AstralCanvasVertexDeclaration_GetElementSize(AstralCanvasVertexDeclaration ptr);
    AstralCanvas_VertexInputRate AstralCanvasVertexDeclaration_GetInputRate(AstralCanvasVertexDeclaration ptr);
    void AstralCanvasVertexDeclaration_GetElements(AstralCanvasVertexDeclaration ptr, usize* outputCount, AstralCanvasVertexElement *outputData);

    AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorDecl();
    AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionColorTextureDecl();
    AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionTextureColorDecl();
    AstralCanvasVertexDeclaration AstralCanvasGetVertexPositionNormalTextureDecl();
    AstralCanvasVertexDeclaration AstralCanvasGetInstanceDataMatrixDecl();
}