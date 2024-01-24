#pragma once

#include <Linxc.h>
#include <vulkan/vulkan.h>

#include "array.hpp"

struct AstralVulkanSwapchainSupportDetails
{
    VkSurfaceCapabilitiesKHR capabilities;
    collections::Array<VkSurfaceFormatKHR> supportedSurfaceFormats;
    collections::Array<VkPresentModeKHR> presentModes;

    inline void deinit()
    {
        supportedSurfaceFormats.deinit();
        presentModes.deinit();
    }
};

AstralVulkanSwapchainSupportDetails AstralCanvasVk_QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, IAllocator *allocator);