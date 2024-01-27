#pragma once
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

void AstralCanvasVk_CopyBufferToImage(AstralVulkanGPU *gpu, VkBuffer from, VkImage imageHandle, u32 width, u32 height);
void AstralCanvasVk_TransitionTextureLayout(AstralVulkanGPU *gpu, VkImage imageHandle, u32 mipLevels, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);

inline AstralCanvasMemoryAllocation AstralCanvasVk_AllocateMemoryForImage(VkImage image, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlagBits memoryProperties)
{
    VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();

    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = memoryUsage;
    allocationCreateInfo.requiredFlags = memoryProperties;

    AstralCanvasMemoryAllocation memoryAllocated;

    if (vmaAllocateMemoryForImage(vma, image, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        THROW_ERR("Failed to create memory for image");
    }

    return memoryAllocated;
}
inline AstralCanvasMemoryAllocation AstralCanvasVk_AllocateMemoryForBuffer(VkBuffer buffer, VmaMemoryUsage memoryUsage, VkMemoryPropertyFlagBits memoryProperties, bool createMapped = true)
{
    VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();
    
    VmaAllocationCreateInfo allocationCreateInfo = {};
    allocationCreateInfo.usage = memoryUsage;
    if (createMapped)
    {
        allocationCreateInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT;
    }
    allocationCreateInfo.requiredFlags = memoryProperties;
    
    AstralCanvasMemoryAllocation memoryAllocated;

    if (vmaAllocateMemoryForBuffer(vma, buffer, &allocationCreateInfo, &memoryAllocated.vkAllocation, &memoryAllocated.vkAllocationInfo) != VK_SUCCESS)
    {
        THROW_ERR("Failed to create memory for buffer");
    }

    return memoryAllocated;
}