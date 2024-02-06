#pragma once
#include "Linxc.h"
#include "Graphics/Enums.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct IndexBuffer
    {
        void *handle;
        IndexBufferSize indexElementSize;
        usize indexCount;

        MemoryAllocation memoryAllocation;

        IndexBuffer();
        IndexBuffer(IndexBufferSize thisIndexElementSize, usize indexCount);

        void SetData(u8* bytes, usize sizeOfBytes);
        void Construct();
        void deinit();
    };
}