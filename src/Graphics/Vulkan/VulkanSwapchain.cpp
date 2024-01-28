#include "Graphics/Vulkan/VulkanSwapchain.hpp"
#include "Maths/Util.hpp"
#include "ErrorHandling.hpp"

using namespace AstralCanvas;

VkSurfaceFormatKHR AstralCanvasVk_FindSurfaceWith(VkColorSpaceKHR colorSpace, VkFormat format, collections::Array<VkSurfaceFormatKHR> toSearch)
{
    for (int i = 0; i < toSearch.length; i++)
    {
        if (toSearch.data[i].colorSpace == colorSpace && toSearch.data[i].format == format)
        {
            return toSearch.data[i];
        }
    }
    return toSearch.data[0];
}

bool AstralCanvasVk_CreateSwapchain(IAllocator *allocator, AstralVulkanGPU *gpu, AstralCanvasWindow *window, AstralVulkanSwapchain* result)
{
    IAllocator cAllocator = GetCAllocator();
    AstralVkSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(gpu->physicalDevice, (VkSurfaceKHR)window->windowSurfaceHandle,  &cAllocator);

    VkSurfaceFormatKHR surfaceFormat = AstralCanvasVk_FindSurfaceWith(VK_COLOR_SPACE_SRGB_NONLINEAR_KHR, VK_FORMAT_B8G8R8A8_UNORM, details.supportedSurfaceFormats);

    AstralVulkanSwapchain swapchain = {};
    swapchain.allocator = allocator;
    swapchain.colorSpace = surfaceFormat.colorSpace;
    swapchain.currentImageIndex = 0;
    swapchain.window = window;
    swapchain.imageFormat = AstralCanvasVk_FromVkFormat(surfaceFormat.format);
    swapchain.gpu = gpu;
    swapchain.presentMode = VK_PRESENT_MODE_MAILBOX_KHR;
    swapchain.imageArrayLayers = 1;
    swapchain.usageFlags = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    swapchain.depthFormat = ImageFormat_Depth32;
    swapchain.imageExtents.width = 0;
    swapchain.imageExtents.height = 0;
    swapchain.oldHandle = NULL;

    details.deinit();

    if (!AstralCanvasVk_SwapchainRecreate(&swapchain, gpu))
    {
        return false;
    }

    *result = swapchain;

    return true;
}
void AstralCanvasVk_DestroySwapchain(AstralVulkanSwapchain *swapchain)
{
    vkDeviceWaitIdle(swapchain->gpu->logicalDevice);
    for (usize i = 0; i < swapchain->renderTargets.length; i++)
    {
        swapchain->renderTargets.data[i].deinit();
    }
    swapchain->renderTargets.deinit();
    swapchain->imageHandles.deinit();
    vkDestroySwapchainKHR(swapchain->gpu->logicalDevice, swapchain->handle, NULL);
}

bool AstralCanvasVk_SwapchainRecreate(AstralVulkanSwapchain* swapchain, AstralVulkanGPU *gpu)
{
    u32 width = 0;
    u32 height = 0;

    while (width == 0 || height == 0)
    {
        width = swapchain->window->resolution.X;
        height = swapchain->window->resolution.Y;
    }

    IAllocator cAllocator = GetCAllocator();
    AstralVkSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(gpu->physicalDevice, (VkSurfaceKHR)swapchain->window->windowSurfaceHandle,  &cAllocator);

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
    swapchain->imageExtents = extents;

    vkDeviceWaitIdle(swapchain->gpu->logicalDevice);

    VkSwapchainCreateInfoKHR createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    createInfo.surface = (VkSurfaceKHR)swapchain->window->windowSurfaceHandle;
    createInfo.minImageCount = details.capabilities.minImageCount;
    createInfo.imageColorSpace = swapchain->colorSpace;
    createInfo.imageFormat = AstralCanvasVk_FromImageFormat(swapchain->imageFormat);
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
    createInfo.imageExtent = swapchain->imageExtents;

    swapchain->oldHandle = swapchain->handle;
    if (vkCreateSwapchainKHR(swapchain->gpu->logicalDevice, &createInfo, NULL, &swapchain->handle) != VK_SUCCESS)
    {
        return false;
    }

    //get swapchain images
    //recreate framebuffers
    if (vkGetSwapchainImagesKHR(swapchain->gpu->logicalDevice, swapchain->handle, &swapchain->imageCount, NULL)!= VK_SUCCESS)
    {
        return false;
    }
    //VkImage *images = (VkImage*)malloc(sizeof(VkImage) * swapchain->imageCount);
    swapchain->imageHandles = collections::Array<void *>(swapchain->allocator, sizeof(VkImage) * swapchain->imageCount);
    vkGetSwapchainImagesKHR(swapchain->gpu->logicalDevice, swapchain->handle, &swapchain->imageCount, (VkImage *)swapchain->imageHandles.data);

    printf("Got image handles for swapchain\n");

    //now that we have swapchain images, we store them into textures, which are in turn
    //stored into rendertarget
    swapchain->renderTargets = collections::Array<RenderTarget>(swapchain->allocator, swapchain->imageCount);
    AstralCanvasVk_SwapchainRecreateRendertargets(swapchain);

    details.deinit();

    return true;
}

void AstralCanvasVk_SwapchainRecreateRendertargets(AstralVulkanSwapchain* swapchain)
{
    //u8 *depthBufferData = (u8 *)calloc(swapchain->imageExtents.width * swapchain->imageExtents.height, 4);

    for (usize i = 0; i < swapchain->renderTargets.length; i++)
    {
        swapchain->renderTargets.data[i].deinit();

        Texture2D backingTexture = CreateTextureFromHandle(swapchain->imageHandles.data[i], swapchain->imageExtents.width, swapchain->imageExtents.height, swapchain->imageFormat, true);
        Texture2D backingDepthbuffer = CreateTextureFromData(NULL, swapchain->imageExtents.width, swapchain->imageExtents.height, swapchain->depthFormat, NULL, true);

        swapchain->renderTargets.data[i] = RenderTarget(backingTexture, backingDepthbuffer, true);
    }

    //free(depthBufferData);
}

bool AstralCanvasVk_SwapchainSwapBuffers(AstralVulkanGPU *gpu, AstralVulkanSwapchain* swapchain, VkSemaphore semaphore, VkFence fence)
{
    VkResult result = vkAcquireNextImageKHR(swapchain->gpu->logicalDevice, swapchain->handle, 1000000000, semaphore, fence, &swapchain->currentImageIndex);

    if (result != VK_SUCCESS)
    {
        if (result == VK_ERROR_OUT_OF_DATE_KHR)
        {
            AstralCanvasVk_DestroySwapchain(swapchain);
            AstralCanvasVk_SwapchainRecreate(swapchain, gpu);

            return true;
        }
        else
        {
            THROW_ERR("Failed to acquire next swapchain image");
            return false;
        }
    }
    return false;
}