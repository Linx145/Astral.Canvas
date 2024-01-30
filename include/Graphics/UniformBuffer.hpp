#pragma once
#include "Linxc.h"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct UniformBuffer
    {
        void *handle;
        usize size;
        usize bindingPoint;

        MemoryAllocation memoryAllocation;

        UniformBuffer();
        UniformBuffer(usize bufferSize);
        void SetData(void *ptr, usize ptrSize);
        void Construct();
        void deinit();
    };
}