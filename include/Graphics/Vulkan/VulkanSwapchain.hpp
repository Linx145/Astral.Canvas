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
    bool recreatedThisFrame;

    u32 currentImageIndex;
    collections::Array<AstralCanvas::RenderTarget> renderTargets;
    collections::Array<void *> imageHandles;
};

bool AstralCanvasVk_CreateSwapchain(IAllocator *allocator, AstralVulkanGPU *gpu, AstralCanvasWindow *window, AstralVulkanSwapchain *swapchain);
void AstralCanvasVk_DestroySwapchain(AstralVulkanSwapchain *swapchain);

bool AstralCanvasVk_SwapchainRecreate(AstralVulkanSwapchain *swapchain, AstralVulkanGPU *gpu);
void AstralCanvasVk_SwapchainRecreateRendertargets(AstralVulkanSwapchain* swapchain);
VkSurfaceFormatKHR AstralCanvasVk_FindSurfaceWith(VkColorSpaceKHR colorSpace, VkFormat format, collections::Array<VkSurfaceFormatKHR> toSearch);

bool AstralCanvasVk_SwapchainSwapBuffers(AstralVulkanGPU *gpu, AstralVulkanSwapchain* swapchain, VkSemaphore semaphore, VkFence fence);