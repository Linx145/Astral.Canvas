#pragma once
#include "Windowing/Window.hpp"
#include "VulkanSwapchainSupportDetails.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/Vulkan/VulkanGPU.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"

struct AstralVulkanSwapchain
{
    IAllocator *allocator;
    AstralVkSwapchainSupportDetails supportDetails;
    AstralCanvasWindow *window;
    AstralVulkanGPU *gpu;
    VkSwapchainKHR handle;
    VkSwapchainKHR oldHandle;
    AstralCanvas::ImageFormat imageFormat;
    AstralCanvas::ImageFormat depthFormat;
    VkColorSpaceKHR colorSpace;
    VkExtent2D imageExtents;
    u32 imageArrayLayers;
    u32 imageCount;
    VkImageUsageFlags usageFlags;
    VkPresentModeKHR presentMode;

    u32 currentImageIndex;
    collections::Array<AstralCanvas::RenderTarget> renderTargets;
    collections::Array<void *> imageHandles;
};

AstralVulkanSwapchain AstralCanvasVk_CreateSwapchain(IAllocator *allocator, AstralVulkanGPU *gpu, AstralCanvasWindow *window);
void AstralCanvasVk_DestroySwapchain(AstralVulkanSwapchain *swapchain);

void AstralCanvasVk_SwapchainRecreate(AstralVulkanSwapchain *swapchain, AstralVulkanGPU *gpu);
void AstralCanvasVk_SwapchainRecreateRendertargets(AstralVulkanSwapchain* swapchain);

VkSurfaceFormatKHR AstralCanvasVk_FindSurfaceWith(VkColorSpaceKHR colorSpace, VkFormat format, collections::Array<VkSurfaceFormatKHR> toSearch);