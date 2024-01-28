#pragma once
#include "Linxc.h"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct VertexBuffer
    {
        void *handle;
        VertexDeclaration *vertexType;
        usize vertexCount;

        MemoryAllocation memoryAllocation;

        bool constructed;

        VertexBuffer();
        VertexBuffer(VertexDeclaration *thisVertexType, usize vertexCount);

        void SetData(u8* verticesByteData, usize lengthOfBytes);
        void Construct();
        void deinit();
    };
}