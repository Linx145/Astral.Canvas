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

        bool constructed;

        IndexBuffer();
        IndexBuffer(IndexBufferSize thisIndexElementSize, usize indexCount);

        void SetData(u8* bytes, usize lengthOfBytes);
        void Construct();
        void deinit();
    };
}