#pragma once
#include "Linxc.h"
#include "Astral.Canvas/Graphics/Enums.h"

extern "C"
{
    typedef void *AstralCanvasIndexBuffer;

    AstralCanvas_IndexBufferSize AstralCanvasIndexBuffer_GetSize(AstralCanvasIndexBuffer ptr);

    usize AstralCanvasIndexBuffer_GetCount(AstralCanvasIndexBuffer ptr);

    AstralCanvasIndexBuffer AstralCanvasIndexBuffer_Create(AstralCanvas_IndexBufferSize indexSize, usize indexCount);
    void AstralCanvasIndexBuffer_Deinit(AstralCanvasIndexBuffer ptr);
    void AstralCanvasIndexBuffer_SetData(AstralCanvasIndexBuffer ptr, u8* bytes, usize sizeOfBytes);
}