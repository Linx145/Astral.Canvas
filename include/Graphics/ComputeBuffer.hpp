#pragma once
#include "Linxc.h"
#include "Graphics/Enums.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct ComputeBuffer
    {
        void *handle;
        usize elementSize;
        usize elementCount;
        bool accessedAsVertexBuffer;
        bool CPUCanRead;

        MemoryAllocation memoryAllocation;

        ComputeBuffer();
        ComputeBuffer(usize elementSize, usize elementCount, bool accessedAsVertexBuffer, bool CPUCanRead);

        void SetData(u8* bytes, usize elementsToSet);
        void Construct();
        void deinit();
    };
}