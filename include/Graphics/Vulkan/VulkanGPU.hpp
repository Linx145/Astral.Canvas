#pragma once

#include <Linxc.h>
#include <vulkan/vulkan.h>

#include "vector.hpp"
#include "array.hpp"
#include "string.hpp"
#include "threading.hpp"

struct AstralVulkanCommandQueue
{
    Threading::Mutex mutex;
    VkQueue queue;
};

struct AstralVulkanGPU
{
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    VkQueueFamilyProperties queueInfo;
    VkPhysicalDeviceLimits limits;

    AstralVulkanCommandQueue *GeneralPurposeQueue;
    AstralVulkanCommandQueue *DedicatedGraphicsQueue;
    AstralVulkanCommandQueue *DedicatedComputeQueue;
    AstralVulkanCommandQueue *DedicatedTransferQueue;
};
bool AstralCanvasVk_SelectGPU(IAllocator *allocator);
u32 AstralCanvasVk_GetGPUScore(VkPhysicalDevice gpu);