#include "Graphics/Texture2D.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#include "Graphics/stb_image.h"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalImplementations.h"
#endif

namespace AstralCanvas
{
    void Texture2D::Construct()
    {
        if (this->constructed)
        {
            return;
        }
        if (this->imageFormat == ImageFormat_BackbufferFormat)
        {
            switch (GetActiveBackend())
            {
#ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    this->imageFormat = AstralCanvasVk_GetCurrentSwapchain()->imageFormat;
                    break;
                }
#endif
#ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    this->imageFormat = AstralCanvasMetal_GetSwapchainFormat();
                    break;
                }
#endif
                default:
                    break;
            }
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkImage image;
                if (this->imageHandle == NULL)
                {
                    VkImageCreateInfo createInfo = {};
                    createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
                    createInfo.imageType = VK_IMAGE_TYPE_2D;
                    createInfo.extent.width = this->width;
                    createInfo.extent.height = this->height;
                    createInfo.extent.depth = 1;
                    
                    createInfo.mipLevels = this->mipLevels;
                    createInfo.arrayLayers = 1;
                    createInfo.format = AstralCanvasVk_FromImageFormat(this->imageFormat);
                    if (createInfo.format == VK_FORMAT_UNDEFINED)
                    {
                        fprintf(stderr, "image format: %i\n", (i32)this->imageFormat);
                        THROW_ERR("Image format is undefined\n");
                    }
                    createInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
                    createInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                    if (this->imageFormat >= ImageFormat_DepthNone)
                    {
                        createInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
                    }
                    else
                    {
                        if (this->usedForRenderTarget)
                        {
                            createInfo.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                        }
                        else
                        {
                            createInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
                        }
                    }
                    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
                    createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
                    createInfo.flags = 0;

                    if (vkCreateImage(gpu->logicalDevice, &createInfo, NULL, (VkImage*)&this->imageHandle) != VK_SUCCESS)
                    {
                        THROW_ERR("Failed to create image");
                    }
                    this->imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                }
                image = (VkImage)this->imageHandle;
                VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
                if (imageFormat == ImageFormat_Depth16 || imageFormat == ImageFormat_Depth32)
                {
                    imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                }
                else if (imageFormat > ImageFormat_DepthNone)
                {
                    imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                }

                bool transitionToAttachmentOptimal = true;
                if ((this->width * this->height > 0) && this->ownsHandle)
                {
                    //VmaAllocation
                    this->allocatedMemory = AstralCanvasVk_AllocateMemoryForImage(image, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

                    //for non-rendertarget textures
                    if (this->bytes != NULL && !this->usedForRenderTarget)
                    {
                        transitionToAttachmentOptimal = false;
                        VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, this->width * this->height * 4, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

                        AstralCanvas::MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), true);

                        memcpy(stagingMemory.vkAllocationInfo.pMappedData, this->bytes, (usize)(this->width * this->height * 4));

                        AstralCanvasVk_TransitionImageLayout(gpu, NULL, (VkImage)this->imageHandle, this->mipLevels, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);

                        AstralCanvasVk_CopyBufferToImage(gpu, stagingBuffer, (VkImage)this->imageHandle, this->width, this->height);

                        AstralCanvasVk_TransitionImageLayout(gpu, NULL, (VkImage)this->imageHandle, this->mipLevels, imageAspect, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

                        vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);

                        vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);

                        this->imageLayout = (u64)VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    }
                }

                if (transitionToAttachmentOptimal)
                {
                    if (this->imageFormat >= ImageFormat_DepthNone)
                    {
                        AstralCanvasVk_TransitionImageLayout(gpu, NULL, image, this->mipLevels, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
                        this->imageLayout = (u64)VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    }
                    else 
                    {
                        AstralCanvasVk_TransitionImageLayout(gpu, NULL, image, this->mipLevels, imageAspect, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);
                        this->imageLayout = (u64)VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    }
                }

                VkImageViewCreateInfo viewCreateInfo = {};
                viewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewCreateInfo.image = image;
                viewCreateInfo.format = AstralCanvasVk_FromImageFormat(this->imageFormat);
                viewCreateInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;

                viewCreateInfo.subresourceRange.aspectMask = imageAspect;
                viewCreateInfo.subresourceRange.baseMipLevel = 0;
                viewCreateInfo.subresourceRange.levelCount = this->mipLevels;
                viewCreateInfo.subresourceRange.baseArrayLayer = 0;
                viewCreateInfo.subresourceRange.layerCount = 1;

                viewCreateInfo.components.r = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.g = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.b = VK_COMPONENT_SWIZZLE_IDENTITY;
                viewCreateInfo.components.a = VK_COMPONENT_SWIZZLE_IDENTITY;

                if (vkCreateImageView(gpu->logicalDevice, &viewCreateInfo, NULL, (VkImageView*)&this->imageView) != VK_SUCCESS)
                {
                    THROW_ERR("Failed to create image view");
                }

                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_CreateTexture(this);
                break;
            }
#endif
            default:
                THROW_ERR("Unrecognised backend when attempting to create a texture");
                break;
        }
        this->constructed = true;
    }
    void *Texture2D::GetData()
    {
        if (!this->constructed || this->isDisposed)
        {
            return NULL;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                usize expectedSize = width * height * 4;
                if (bytes == NULL)
                {
                    bytes = (u8 *)malloc(expectedSize);
                }
                VkImageAspectFlags imageAspect = VK_IMAGE_ASPECT_COLOR_BIT;
                if (imageFormat == ImageFormat_Depth16 || imageFormat == ImageFormat_Depth32)
                {
                    imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT;
                }
                else if (imageFormat > ImageFormat_DepthNone)
                {
                    imageAspect = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                }

                VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, this->width * this->height * 4, VK_BUFFER_USAGE_TRANSFER_DST_BIT);

                AstralCanvas::MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_GPU_TO_CPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT), true);

                AstralCanvasVk_TransitionImageLayout(gpu, NULL, (VkImage)this->imageHandle, this->mipLevels, imageAspect, (VkImageLayout)this->imageLayout, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

                AstralCanvasVk_CopyImageToBuffer(gpu, (VkImage)this->imageHandle, stagingBuffer, width, height);

                AstralCanvasVk_TransitionImageLayout(gpu, NULL, (VkImage)this->imageHandle, this->mipLevels, imageAspect, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, (VkImageLayout)this->imageLayout);

                memcpy(bytes, stagingMemory.vkAllocationInfo.pMappedData, expectedSize);

                vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);

                vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);


                return bytes;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: Texture2D GetData");
                break;
        }
    }
    void Texture2D::deinit()
    {
        if (!this->constructed || this->isDisposed)
        {
            return;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();

                vkDestroyImageView(gpu->logicalDevice, (VkImageView)this->imageView, NULL);
                if (this->ownsHandle)
                {
                    vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), this->allocatedMemory.vkAllocation);
                    vkDestroyImage(gpu->logicalDevice, (VkImage)this->imageHandle, NULL);
                }
                
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_DestroyTexture(this);
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: Texture2D deinit");
                break;
        }
        if (this->bytes != NULL)
        {
            free(this->bytes);
        }
        this->isDisposed = true;
    }
    Texture2D CreateTextureFromFile(const char *fileName)
    {
        Texture2D result = {};
        result.width = 0;
        result.height = 0;
        result.imageHandle = NULL;
        result.ownsHandle = true;
        result.usedForRenderTarget = false;
        result.mipLevels = 1;
        result.imageFormat = ImageFormat_R8G8B8A8Unorm;
        result.bytes = NULL;
        result.constructed = false;
        result.isDisposed = false;

        i32 channelsInFile = 0;
        i32 width;
        i32 height;
        result.bytes = stbi_load(fileName, &width, &height, &channelsInFile, 4);

        result.width = width;
        result.height = height;

        result.Construct();
        free(result.bytes);
        result.bytes = NULL;
        return result;
    }
    Texture2D CreateTextureFromHandle(void *handle, u32 width, u32 height, ImageFormat imageFormat, bool usedForRenderTarget)
    {
        Texture2D result = {};
        result.width = width;
        result.height = height;
        result.imageHandle = handle;
        result.ownsHandle = false;
        result.usedForRenderTarget = usedForRenderTarget;
        result.mipLevels = 1;
        result.imageFormat = imageFormat;
        result.bytes = NULL;
        result.constructed = false;
        result.isDisposed = false;

        result.Construct();
        return result;
    }
    Texture2D CreateTextureFromData(u8* data, u32 width, u32 height, ImageFormat imageFormat, bool usedForRenderTarget)
    {
        Texture2D result = {};
        result.width = width;
        result.height = height;
        result.ownsHandle = true;
        result.bytes = data;
        result.imageFormat = imageFormat;
        result.mipLevels = 1;
        result.usedForRenderTarget = usedForRenderTarget;
        result.constructed = false;
        result.isDisposed = false;

        result.Construct();
        return result;
    }
}
