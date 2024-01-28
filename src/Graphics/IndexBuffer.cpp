#include "Graphics/IndexBuffer.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#include "Graphics/Vulkan/vk_mem_alloc.h"
#endif

namespace AstralCanvas
{
    IndexBuffer::IndexBuffer()
    {
        this->handle = NULL;
        this->memoryAllocation.unused = 0;
        this->indexCount = 0;
        this->indexElementSize = IndexBufferSize_U16;
    }
    IndexBuffer::IndexBuffer(IndexBufferSize thisIndexElementSize, usize indexCount)
    {
        this->indexElementSize = thisIndexElementSize;
        this->indexCount = indexCount;
        this->handle = NULL;
        this->memoryAllocation.unused = 0;

        this->Construct();
    }
    void IndexBuffer::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                usize size = this->indexCount;
                if (this->indexElementSize == IndexBufferSize_U16)
                {
                    size *= 2;
                }
                else
                    size *= 4;

                this->handle = AstralCanvasVk_CreateResourceBuffer(AstralCanvasVk_GetCurrentGPU(), size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT);
                this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void IndexBuffer::SetData(u8* bytes, usize lengthOfBytes)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, lengthOfBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

                memcpy(stagingMemory.vkAllocationInfo.pMappedData, bytes, lengthOfBytes);

                AstralCanvasVk_CopyBufferToBuffer(gpu, stagingBuffer, (VkBuffer)this->handle, lengthOfBytes);

                vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);
                vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);

                break;
            }
            #endif
            default:
                break;
        }
    }
    void IndexBuffer::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();

                vkDestroyBuffer(gpu->logicalDevice, (VkBuffer)this->handle, NULL);

                vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), this->memoryAllocation.vkAllocation);
                break;
            }
            #endif
            default:
                break;
        }
    }
}