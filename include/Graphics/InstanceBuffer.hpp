#pragma once
#include "Linxc.h"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct InstanceBuffer
    {
        void *handle;
        bool canRead;
        usize instanceSize;
        usize instanceCount;

        MemoryAllocation memoryAllocation;

        InstanceBuffer();
        InstanceBuffer(usize instanceSize, usize instanceCount, bool canRead = false);

        void SetData(void* instancesData, usize count);
        void Construct();
        void deinit();
    };
}
