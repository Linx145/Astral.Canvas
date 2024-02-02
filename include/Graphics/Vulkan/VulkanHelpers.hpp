#pragma once
#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/SamplerState.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/Vulkan/VulkanGPU.hpp"
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/MemoryAllocation.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/RenderProgram.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/RenderPipeline.hpp"
#include "ErrorHandling.hpp"

VkBuffer AstralCanvasVk_CreateResourceBuffer(AstralVulkanGPU *gpu, usize size, VkBufferUsageFlags usageFlags);

VkCommandBuffer AstralCanvasVk_CreateTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, bool alsoBeginBuffer);
void AstralCanvasVk_EndTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, VkCommandBuffer commandBuffer);

void AstralCanvasVk_CopyBufferToBuffer(AstralVulkanGPU *gpu, VkBuffer from, VkBuffer to, usize copySize);

void AstralCanvasVk_CopyBufferToImage(AstralVulkanGPU *gpu, VkBuffer from, VkImage imageHandle, u32 width, u32 height);
void AstralCanvasVk_TransitionTextureLayout(AstralVulkanGPU *gpu, AstralCanvas::Texture2D *texture, VkImageAspectFlags aspectFlags, VkImageLayout newLayout);
void AstralCanvasVk_TransitionImageLayout(AstralVulkanGPU *gpu, VkImage imageHandle, u32 mipLevels, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);

inline AstralCanvas::MemoryAllocation AstralCanvasVk_AllocateMemoryForImage(VkImage image, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlagBits memoryProperties)
{
    VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.requiredFlags = memoryProperties;

    AstralCanvas::MemoryAllocation memoryAllocated;

    if (vmaAllocateMemoryForImage(vma, image, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        THROW_ERR("Failed to create memory for image");
    }

    vmaBindImageMemory(vma, memoryAllocated.vkAllocation, image);

    return memoryAllocated;
}
inline AstralCanvas::MemoryAllocation AstralCanvasVk_AllocateMemoryForBuffer(VkBuffer buffer, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlagBits memoryProperties, bool createMapped = true)
{
    VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();
    
    VmaAllocationCreateInfo allocationCreateInfo{};
    allocationCreateInfo.usage = memoryUsage;
    if (createMapped)
    {
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    allocationCreateInfo.requiredFlags = memoryProperties;
    
    AstralCanvas::MemoryAllocation memoryAllocated;

    if (vmaAllocateMemoryForBuffer(vma, buffer, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        THROW_ERR("Failed to create memory for buffer");
    }

    vmaBindBufferMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), memoryAllocated.vkAllocation, buffer);

    return memoryAllocated;
}
#endif
