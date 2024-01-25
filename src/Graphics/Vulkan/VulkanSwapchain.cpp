#include "Graphics/Vulkan/VulkanSwapchain.hpp"
#include "Maths/Util.hpp"

inline VulkanSwapchain AstralCanvasVk_SwapchainCreate(AstralCanvasWindow *window, VkPhysicalDevice physicalDevice, VkDevice logicalDevice)
{
    IAllocator cAllocator = GetCAllocator();
    AstralVkSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(physicalDevice, (VkSurfaceKHR)window->windowSurfaceHandle,  &cAllocator);

    VkSurfaceFormatKHR surfaceFormat = AstralCanvasVk_FindSurfaceWith(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_FORMAT_B8G8R8_UNORM, details.supportedSurfaceFormats);

    VulkanSwapchain swapchain = {};
    swapchain.colorSpace = surfaceFormat.colorSpace;
    swapchain.currentImageIndex = 0;
    swapchain.window = window;
    swapchain.imageFormat = surfaceFormat.format;
    swapchain.device = logicalDevice;
    swapchain.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swapchain.imageArrayLayers = 1;
    swapchain.usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain.imageExtents.width = 0;
    swapchain.imageExtents.height = 0;

    details.deinit();
}

void AstralCanvasVk_SwapchainRecreate(VulkanSwapchain* swapchain, VkPhysicalDevice physicalDevice)
{
    u32 width, height = 0;

    while (width == 0 || height == 0)
    {
        width = swapchain->window->resolution.X;
        height = swapchain->window->resolution.Y;
    }

    IAllocator cAllocator = GetCAllocator();
    AstralVkSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(physicalDevice, (VkSurfaceKHR)swapchain->window->windowSurfaceHandle,  &cAllocator);

    VkExtent2D extents = {};
    while (extents.width == 0 || extents.height == 0)
    {
        if (details.capabilities.currentExtent.width != UINT32_MAX)
        {
            extents = details.capabilities.currentExtent;
        }
        else
        {
            i32 width;
            i32 height;
            glfwGetFramebufferSize(swapchain->window->handle, &width, &height);
            extents.width = Maths::Clamp(details.capabilities.minImageExtent.width, details.capabilities.maxImageExtent.width, width);
            extents.height = Maths::Clamp(details.capabilities.minImageExtent.height, details.capabilities.maxImageExtent.height, height);
        }
    }

    vkDeviceWaitIdle(swapchain->device);

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = (VkSurfaceKHR)swapchain->window->windowSurfaceHandle;
    createInfo.minImageCount = details.capabilities.minImageCount;
    createInfo.imageColorSpace = swapchain->colorSpace;
    createInfo.imageFormat = swapchain->imageFormat;
    createInfo.imageArrayLayers = swapchain->imageArrayLayers;
    createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
    createInfo.queueFamilyIndexCount = 0;
    createInfo.pQueueFamilyIndices = NULL;

    createInfo.preTransform = details.capabilities.currentTransform; // VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
    createInfo.presentMode = swapchain->presentMode;
    createInfo.clipped = true;
    createInfo.oldSwapchain = swapchain->oldHandle;

    swapchain->oldHandle = swapchain->handle;
    vkCreateSwapchainKHR(swapchain->device, &createInfo, NULL, &swapchain->handle);

    //get swapchain images
    //recreate framebuffers

    details.deinit();
}