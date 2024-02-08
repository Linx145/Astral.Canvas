#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasIndexBuffer;

    DynamicFunction AstralCanvas_IndexBufferSize AstralCanvasIndexBuffer_GetSize(AstralCanvasIndexBuffer ptr);
    DynamicFunction usize AstralCanvasIndexBuffer_GetCount(AstralCanvasIndexBuffer ptr);
    DynamicFunction AstralCanvasIndexBuffer AstralCanvasIndexBuffer_Create(AstralCanvas_IndexBufferSize indexSize, usize indexCount);
    DynamicFunction void AstralCanvasIndexBuffer_Deinit(AstralCanvasIndexBuffer ptr);
    DynamicFunction void AstralCanvasIndexBuffer_SetData(AstralCanvasIndexBuffer ptr, u8* bytes, usize sizeOfBytes);

#ifdef __cplusplus
}
#endif