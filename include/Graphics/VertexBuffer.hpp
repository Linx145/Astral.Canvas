#pragma once
#include "Linxc.h"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct VertexBuffer
    {
        void *handle;
        bool canRead;
        VertexDeclaration *vertexType;
        usize vertexCount;

        MemoryAllocation memoryAllocation;

        VertexBuffer();
        VertexBuffer(VertexDeclaration *thisVertexType, usize vertexCount, bool canRead = false);

        void SetData(void* verticesData, usize count);
        void *GetData(IAllocator *allocator, usize* dataLength);
        void Construct();
        void deinit();
    };
}