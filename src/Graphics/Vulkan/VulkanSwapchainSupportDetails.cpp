#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanSwapchainSupportDetails.hpp"

using namespace collections;

AstralVkSwapchainSupportDetails AstralCanvasVk_QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR windowSurface, IAllocator allocator)
{
    AstralVkSwapchainSupportDetails details;

    vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, windowSurface, &details.capabilities);

    u32 supportedSurfaceFormatCount;
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedSurfaceFormatCount, NULL);

    details.supportedSurfaceFormats = Array<VkSurfaceFormatKHR>(allocator, supportedSurfaceFormatCount);
    vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, windowSurface, &supportedSurfaceFormatCount, details.supportedSurfaceFormats.data);

    u32 supportedPresentModesCount;
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentModesCount, NULL);

    details.presentModes = Array<VkPresentModeKHR>(allocator);
    vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, windowSurface, &supportedPresentModesCount, details.presentModes.data);

    return details;
}
#endif
