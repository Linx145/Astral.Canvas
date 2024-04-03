#pragma once
#include "Linxc.h"
#include "Graphics/Enums.hpp"
#include "Graphics/MemoryAllocation.hpp"
#include "allocators.hpp"

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
        void *GetData(IAllocator allocator, usize* dataLength);
        void Construct();
        void FlagToClear();
        static void ClearAllFlagged();
        void deinit();
    };
}