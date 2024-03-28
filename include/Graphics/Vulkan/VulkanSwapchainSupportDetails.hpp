#pragma once
#ifdef ASTRALCANVAS_VULKAN
#include <Linxc.h>
#include <vulkan/vulkan.h>

#include "array.hpp"

struct AstralVkSwapchainSupportDetails
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

AstralVkSwapchainSupportDetails AstralCanvasVk_QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, IAllocator allocator);
#endif
