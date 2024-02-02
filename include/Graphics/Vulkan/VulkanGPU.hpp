#pragma once

#ifdef ASTRALCANVAS_VULKAN
#include <Linxc.h>
#include <vulkan/vulkan.h>
#include <Graphics/Vulkan/VulkanQueueProperties.hpp>

#include "vector.hpp"
#include "array.hpp"
#include "string.hpp"
#include "threading.hpp"

struct AstralCanvasVkCommandQueue
{
    VkDevice logicalDevice;

    VkQueue queue;
    Threading::Mutex queueMutex;
    VkFence queueFence;

    VkCommandPool transientCommandPool;
    Threading::Mutex commandPoolMutex;

    inline AstralCanvasVkCommandQueue()
    {
        logicalDevice = NULL;
        queue = NULL;
        queueMutex = Threading::Mutex();
        transientCommandPool = NULL;
        commandPoolMutex = Threading::Mutex();
        queueFence = NULL;
    }
    inline AstralCanvasVkCommandQueue(VkDevice thisLogicalDevice, VkQueue thisQueue, VkFence thisQueueFence, VkCommandPool thisTransientCommandPool)
    {
        logicalDevice = thisLogicalDevice;
        queue = thisQueue;
        queueFence = thisQueueFence;
        transientCommandPool = thisTransientCommandPool;
        queueMutex = Threading::Mutex::init();
        commandPoolMutex = Threading::Mutex::init();
    }
    inline void deinit()
    {
        if (queueFence != NULL)
        {
            vkDestroyFence(logicalDevice, queueFence, NULL);
        }
        if (transientCommandPool != NULL)
        {
            vkDestroyCommandPool(logicalDevice, transientCommandPool, NULL);
        }
        queueMutex.deinit();
        commandPoolMutex.deinit();
    }
};

struct AstralVulkanGPU
{
    collections::Array<const char *> requiredExtensions;
    VkPhysicalDevice physicalDevice;
    VkDevice logicalDevice;
    AstralVulkanQueueProperties queueInfo;
    VkPhysicalDeviceProperties properties;
    VkPhysicalDeviceFeatures features;

    AstralCanvasVkCommandQueue DedicatedGraphicsQueue;
    AstralCanvasVkCommandQueue DedicatedComputeQueue;
    AstralCanvasVkCommandQueue DedicatedTransferQueue;

    inline AstralVulkanGPU()
    {
        requiredExtensions = collections::Array<const char *>();
        physicalDevice = NULL;
        logicalDevice = NULL;
        queueInfo = AstralVulkanQueueProperties();
        properties = {};
        features = {};
        DedicatedGraphicsQueue = AstralCanvasVkCommandQueue();
        DedicatedComputeQueue = AstralCanvasVkCommandQueue();
        DedicatedTransferQueue = AstralCanvasVkCommandQueue();
    }
    inline AstralVulkanGPU(collections::Array<const char *> thisRequiredExtensions, VkPhysicalDevice thisPhysicalDevice, AstralVulkanQueueProperties thisQueueInfos)
    {
        this->queueInfo = thisQueueInfos;
        this->physicalDevice = thisPhysicalDevice;
        this->requiredExtensions = thisRequiredExtensions;

        vkGetPhysicalDeviceFeatures(thisPhysicalDevice, &this->features);
        vkGetPhysicalDeviceProperties(thisPhysicalDevice, &this->properties);

        DedicatedGraphicsQueue = AstralCanvasVkCommandQueue();
        DedicatedComputeQueue = AstralCanvasVkCommandQueue();
        DedicatedTransferQueue = AstralCanvasVkCommandQueue();
    }
};
bool AstralCanvasVk_SelectGPU(IAllocator *allocator, VkInstance instance, VkSurfaceKHR windowSurface, collections::Array<const char *> requiredExtensions, AstralVulkanGPU *output);
void AstralCanvasVk_ReleaseGPU(AstralVulkanGPU *gpu);
bool AstralCanvasVk_GPUExtensionsSupported(AstralVulkanGPU *gpu);
u32 AstralCanvasVk_GetGPUScore(AstralVulkanGPU* gpu, VkSurfaceKHR windowSurface);
bool AstralCanvasVk_CreateLogicalDevice(AstralVulkanGPU* gpu, IAllocator* allocator);
#endif
