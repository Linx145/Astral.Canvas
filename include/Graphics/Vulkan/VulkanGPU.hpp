#pragma once

#include <Linxc.h>
#include <vulkan/vulkan.h>
#include <Graphics/Vulkan/VulkanQueueProperties.hpp>

#include "vector.hpp"
#include "array.hpp"
#include "string.hpp"
#include "threading.hpp"

struct AstralVulkanCommandQueue
{
    Threading::Mutex mutex;
    VkQueue queue;

    inline AstralVulkanCommandQueue()
    {
        queue = NULL;
        mutex = Threading::Mutex();
    }
    inline AstralVulkanCommandQueue(VkQueue thisQueue)
    {
        queue = thisQueue;
        mutex = Threading::Mutex::init();
    }
    inline void deinit()
    {
        mutex.deinit();
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

    AstralVulkanCommandQueue DedicatedGraphicsQueue;
    AstralVulkanCommandQueue DedicatedComputeQueue;
    AstralVulkanCommandQueue DedicatedTransferQueue;

    inline AstralVulkanGPU()
    {
        requiredExtensions = collections::Array<const char *>();
        physicalDevice = NULL;
        logicalDevice = NULL;
        queueInfo = AstralVulkanQueueProperties();
        properties = {};
        features = {};
        DedicatedGraphicsQueue = AstralVulkanCommandQueue();
        DedicatedComputeQueue = AstralVulkanCommandQueue();
        DedicatedTransferQueue = AstralVulkanCommandQueue();
    }
    inline AstralVulkanGPU(collections::Array<const char *> thisRequiredExtensions, VkPhysicalDevice thisPhysicalDevice, AstralVulkanQueueProperties thisQueueInfos)
    {
        this->queueInfo = thisQueueInfos;
        this->physicalDevice = thisPhysicalDevice;
        this->requiredExtensions = thisRequiredExtensions;

        vkGetPhysicalDeviceFeatures(thisPhysicalDevice, &this->features);
        vkGetPhysicalDeviceProperties(thisPhysicalDevice, &this->properties);

        DedicatedGraphicsQueue = AstralVulkanCommandQueue();
        DedicatedComputeQueue = AstralVulkanCommandQueue();
        DedicatedTransferQueue = AstralVulkanCommandQueue();
    }
    inline void deinit()
    {
        if (logicalDevice != NULL)
        {
            vkDestroyDevice(logicalDevice, NULL);
        }
        requiredExtensions.deinit();
    }
};
option<AstralVulkanGPU> AstralCanvasVk_SelectGPU(IAllocator *allocator, VkInstance instance, VkSurfaceKHR windowSurface, collections::Array<const char*> requiredExtensions);
bool AstralCanvasVk_GPUExtensionsSupported(AstralVulkanGPU *gpu);
u32 AstralCanvasVk_GetGPUScore(AstralVulkanGPU* gpu, VkSurfaceKHR windowSurface);
bool AstralCanvasVk_CreateLogicalDevice(AstralVulkanGPU* gpu, IAllocator* allocator);