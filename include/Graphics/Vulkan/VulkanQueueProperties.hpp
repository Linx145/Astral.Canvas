#pragma once

#include "array.hpp"
#include "vulkan/vulkan.h"

enum AstralVulkanQueue_Type
{
    AstralVulkanQueue_GeneralPurpose,
    AstralVulkanQueue_Compute,
    AstralVulkanQueue_Graphics,
    AstralVulkanQueue_Transfer
};
struct AstralVulkanQueueProperties
{
    collections::Array<VkQueueFamilyProperties> all;
    i32 generalPurposeQueueIndex;
    i32 dedicatedGraphicsQueueIndex;
    i32 dedicatedComputeQueueIndex;
    i32 dedicatedTransferQueueIndex;
    VkSurfaceKHR windowSurface;

    inline AstralVulkanQueueProperties(VkSurfaceKHR thisWindowSurface)
    {
        all = collections::Array<VkQueueFamilyProperties>();
        generalPurposeQueueIndex = -1;
        dedicatedGraphicsQueueIndex = -1;
        dedicatedComputeQueueIndex = -1;
        dedicatedTransferQueueIndex = -1;
        windowSurface = thisWindowSurface;
    }
};
i32 AstralCanvasVk_GetQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, AstralVulkanQueue_Type type);
i32 AstralCanvasVk_GetGeneralPurposeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent);
i32 AstralCanvasVk_GetGraphicsQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent);
i32 AstralCanvasVk_GetTransferQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice);
i32 AstralCanvasVk_GetComputeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice);