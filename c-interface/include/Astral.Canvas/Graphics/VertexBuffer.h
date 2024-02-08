#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"
#include "Astral.Canvas/Graphics/VertexDeclaration.h"

#ifdef __cplusplus
extern "C"
{
#endif
    typedef void *AstralCanvasVertexBuffer;

    DynamicFunction AstralCanvasVertexDeclaration AstralCanvasVertexBuffer_GetVertexDeclaration(AstralCanvasVertexBuffer ptr);
    DynamicFunction usize AstralCanvasVertexBuffer_GetCount(AstralCanvasVertexBuffer ptr);
    DynamicFunction AstralCanvasVertexBuffer AstralCanvasVertexBuffer_Create(AstralCanvasVertexDeclaration thisVertexType, usize vertexCount, bool isDynamic, bool canRead);
    DynamicFunction void AstralCanvasVertexBuffer_Deinit(AstralCanvasVertexBuffer ptr);
    DynamicFunction void AstralCanvasVertexBuffer_SetData(AstralCanvasVertexBuffer ptr, void* verticesData, usize verticesCount);
#ifdef __cplusplus
}
#endif