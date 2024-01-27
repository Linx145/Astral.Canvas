#pragma once
#include "Linxc.h"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/vk_mem_alloc.h"
#endif

union AstralCanvasMemoryAllocation
{
    #ifdef ASTRALCANVAS_VULKAN
    struct
    {
        VmaAllocation vkAllocation;
        VmaAllocationInfo vkAllocationInfo;
    };
    #endif
    struct
    {
        i32 unused;
    };
};