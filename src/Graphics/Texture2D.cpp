#include "Graphics/Texture2D.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

namespace AstralCanvas
{
    void Texture2D::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                if (!this->isDisposed && this->constructed)
                {
                    vkDestroyImageView(gpu->logicalDevice, (VkImageView)this->imageView, NULL);
                    vkDestroyImage(gpu->logicalDevice, (VkImage)this->imageHandle, NULL);
                    vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), this->allocatedMemory.vkAllocation);
                    this->isDisposed = true;
                }
                break;
            }
            #endif
            default:
                THROW_ERR("Unrecognised backend when attempting to create a texture");
                break;
        }
    }
    Texture2D CreateTextureFromData(u8* data, usize width, usize height, SamplerState *samplerForTexture, ImageFormat imageFormat, bool usedForRenderTarget)
    {
        Texture2D result = {};
        result.width = width;
        result.height = height;
        result.ownsHandle = true;
        result.bytes = data;
        result.imageFormat = imageFormat;
        result.mipLevels = 1;
        result.usedForRenderTarget = usedForRenderTarget;

        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkImage image;
                if (result.imageHandle == NULL)
                {
                    VkImageCreateInfo createInfo = {};
                    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    createInfo.imageType = VK_IMAGE_TYPE_2D;
                    createInfo.extent.width = result.width;
                    createInfo.extent.height = result.height;
                    createInfo.extent.depth = 1;

                    createInfo.mipLevels = result.mipLevels;
                    createInfo.arrayLayers = 1;
                    createInfo.format = AstralCanvasVk_FromImageFormat(result.imageFormat);
                    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    if (createInfo.format >= ImageFormat_DepthNone)
                    {
                        createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    }
                    else
                    {
                        if (result.usedForRenderTarget)
                        {
                            createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                        }
                        else
                        {
                            createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                        }
                    }
                    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                    createInfo.flags = 0;

                    if (vkCreateImage(gpu->logicalDevice, &createInfo, NULL, (VkImage*)&result.imageHandle) != VK_SUCCESS)
                    {
                        THROW_ERR("Failed to create image");
                    }
                }
                image = (VkImage)result.imageHandle;

                if (result.bytes != NULL && (result.width * result.height > 0) && result.ownsHandle)
                {
                    //VmaAllocation
                    result.allocatedMemory = AstralCanvasVk_AllocateMemoryForImage(image, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    if (!result.usedForRenderTarget)
                    {
                        VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, result.width * result.height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

                        AstralCanvasMemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), true);

                        memcpy(stagingMemory.vkAllocationInfo.pMappedData, result.bytes, (usize)(result.width * result.height * 4));

                        AstralCanvasVk_TransitionTextureLayout(gpu, (VkImage)result.imageHandle, result.mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                        AstralCanvasVk_CopyBufferToImage(gpu, stagingBuffer, (VkImage)result.imageHandle, result.width, result.height);

                        AstralCanvasVk_TransitionTextureLayout(gpu, (VkImage)result.imageHandle, result.mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                        vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);

                        vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);
                    }
                }
                else
                {
                    if (result.imageFormat >= ImageFormat_DepthNone)
                    {
                        AstralCanvasVk_TransitionTextureLayout(gpu, (VkImage)result.imageHandle, result.mipLevels, VK_IMAGE_ASPECT_DEPTH_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_ATTACHMENT_OPTIMAL);
                    }
                    else AstralCanvasVk_TransitionTextureLayout(gpu, (VkImage)result.imageHandle, result.mipLevels, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                }

                VkImageViewCreateInfo viewCreateInfo = {};
                viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewCreateInfo.image = image;
                viewCreateInfo.format = AstralCanvasVk_FromImageFormat(result.imageFormat);
                viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

                if (result.imageFormat >= ImageFormat_DepthNone)
                {
                    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
                }
                else 
                    viewCreateInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewCreateInfo.subresourceRange.baseMipLevel = 0;
                viewCreateInfo.subresourceRange.levelCount = result.mipLevels;
                viewCreateInfo.subresourceRange.baseArrayLayer = 0;
                viewCreateInfo.subresourceRange.layerCount = 1;

                viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                if (vkCreateImageView(gpu->logicalDevice, &viewCreateInfo, NULL, (VkImageView*)&result.imageView) != VK_SUCCESS)
                {
                    THROW_ERR("Failed to create image view");
                }

                result.constructed = true;
                break;
            }
            #endif
            default:
                THROW_ERR("Unrecognised backend when attempting to create a texture");
                break;
        }

        return result;
    }
}