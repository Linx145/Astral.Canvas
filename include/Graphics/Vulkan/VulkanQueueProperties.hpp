#pragma once
#ifdef ASTRALCANVAS_VULKAN
#include "Linxc.h"
#include "array.hpp"
#include "vulkan/vulkan.h"
#include "hash.hpp"
#include "hashmap.hpp"

enum AstralVulkanQueue_Type
{
    AstralVulkanQueue_Compute,
    AstralVulkanQueue_Graphics,
    AstralVulkanQueue_Transfer
};
struct AstralVulkanQueueResult
{
    AstralVulkanQueue_Type type;
    i32 queueID;
    i32 queueIndex;
};
struct AstralVulkanQueueProperties
{
    IAllocator *allocator;
    collections::vector<AstralVulkanQueueResult> queueCreationResults;
    collections::Array<VkQueueFamilyProperties> all;
    i32 generalPurposeQueueIndex;
    i32 dedicatedGraphicsQueueIndex;
    i32 dedicatedComputeQueueIndex;
    i32 dedicatedTransferQueueIndex;
    VkSurfaceKHR windowSurface;

    inline AstralVulkanQueueProperties()
    {
        queueCreationResults = collections::vector<AstralVulkanQueueResult>();
        all = collections::Array<VkQueueFamilyProperties>();
        generalPurposeQueueIndex = -1;
        dedicatedGraphicsQueueIndex = -1;
        dedicatedComputeQueueIndex = -1;
        dedicatedTransferQueueIndex = -1;
        windowSurface = NULL;
    }
    inline AstralVulkanQueueProperties(IAllocator *thisAllocator, collections::Array<VkQueueFamilyProperties> allFamilyProperties, VkSurfaceKHR thisWindowSurface)
    {
        this->allocator = thisAllocator;
        queueCreationResults = collections::vector<AstralVulkanQueueResult>(thisAllocator);
        all = allFamilyProperties;
        generalPurposeQueueIndex = -1;
        dedicatedGraphicsQueueIndex = -1;
        dedicatedComputeQueueIndex = -1;
        dedicatedTransferQueueIndex = -1;
        windowSurface = thisWindowSurface;
    }
    inline void deinit()
    {
        queueCreationResults.deinit();
        all.deinit();
    }
};
i32 AstralCanvasVk_GetQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, AstralVulkanQueue_Type type);
//i32 AstralCanvasVk_GetGeneralPurposeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent);
i32 AstralCanvasVk_GetGraphicsQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent);
i32 AstralCanvasVk_GetTransferQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice);
i32 AstralCanvasVk_GetComputeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice);
#ifdef ASTRALCANVAS_VULKAN
