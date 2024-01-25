#pragma once
#include "Windowing/Window.hpp"
#include "VulkanSwapchainSupportDetails.hpp"

struct VulkanSwapchain
{
    AstralVkSwapchainSupportDetails supportDetails;

    AstralCanvasWindow *window;
    VkDevice device;
    VkSwapchainKHR handle;
    VkSwapchainKHR oldHandle;
    VkFormat imageFormat;
    VkFormat depthFormat;
    VkColorSpaceKHR colorSpace;
    VkExtent2D imageExtents;
    u32 imageArrayLayers;
    VkImageUsageFlags usageFlags;
    VkPresentModeKHR presentMode;

    u32 currentImageIndex;

    inline void deinit()
    {
        vkDeviceWaitIdle(device);
        vkDestroySwapchainKHR(device, handle, NULL);
    }
};

void AstralCanvasVk_SwapchainRecreate(VulkanSwapchain* swapchain);

VulkanSwapchain AstralCanvasVk_SwapchainCreate(AstralCanvasWindow *window);

VkSurfaceFormatKHR AstralCanvasVk_FindSurfaceWith(VkColorSpaceKHR colorSpace, VkFormat format, collections::Array<VkSurfaceFormatKHR> toSearch);