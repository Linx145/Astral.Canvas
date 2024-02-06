#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"
#include "Astral.Canvas/Graphics/VertexDeclaration.h"

extern "C"
{
    typedef void *AstralCanvasVertexBuffer;

    AstralCanvasVertexDeclaration AstralCanvasVertexBuffer_GetVertexDeclaration(AstralCanvasVertexBuffer ptr);

    usize AstralCanvasVertexBuffer_GetCount(AstralCanvasVertexBuffer ptr);

    AstralCanvasVertexBuffer AstralCanvasVertexBuffer_Create(AstralCanvasVertexDeclaration thisVertexType, usize vertexCount, bool isDynamic, bool canRead);
    void AstralCanvasVertexBuffer_Deinit(AstralCanvasVertexBuffer ptr);
    void AstralCanvasVertexBuffer_SetData(AstralCanvasVertexBuffer ptr, void* verticesData, usize verticesCount);
}