#include "Graphics/ComputeBuffer.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#include "Graphics/Vulkan/vk_mem_alloc.h"
#endif

#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalImplementations.h"
#endif

namespace AstralCanvas
{
    collections::vector<ComputeBuffer*> computeBuffersToClear;

    ComputeBuffer::ComputeBuffer()
    {
        handle = NULL;
        elementCount = 0;
        elementSize = 0;

        memoryAllocation.unused = 0;
    }
    ComputeBuffer::ComputeBuffer(usize elementSize, usize elementCount, bool accessedAsVertexBuffer, bool accessedAsIndirectDrawData, bool CPUCanRead)
    {
        handle = NULL;
        this->elementCount = elementCount;
        this->elementSize = elementSize;
        this->accessedAsVertexBuffer = accessedAsVertexBuffer;
        this->accessedAsIndirectDrawData = accessedAsIndirectDrawData;
        this->CPUCanRead = CPUCanRead;
        this->memoryAllocation.unused = 0;

        this->Construct();
    }
    void ComputeBuffer::SetData(u8* bytes, usize elementsToSet)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                memcpy(this->memoryAllocation.vkAllocationInfo.pMappedData, bytes, elementsToSet * elementSize);
                /*usize lengthOfBytes = elementsToSet * elementSize;
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, lengthOfBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

                memcpy(stagingMemory.vkAllocationInfo.pMappedData, bytes, lengthOfBytes);

                AstralCanvasVk_CopyBufferToBuffer(gpu, stagingBuffer, (VkBuffer)this->handle, lengthOfBytes);

                vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);
                vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);*/

                break;
            }
            #endif
            default:
                break;
        }
    }
    void *ComputeBuffer::GetData(IAllocator allocator, usize* dataLength)
    {
        if (CPUCanRead)
        {
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    usize lengthOfBytes = elementCount * elementSize;
                    AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                    VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, lengthOfBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
                    MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_GPU_TO_CPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

                    AstralCanvasVk_CopyBufferToBuffer(gpu, (VkBuffer)this->handle, stagingBuffer, lengthOfBytes);

                    void *result = allocator.Allocate(lengthOfBytes);
                    memcpy(result, stagingMemory.vkAllocationInfo.pMappedData, lengthOfBytes);

                    vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);
                    vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);

                    if (dataLength != NULL)
                    {
                        *dataLength = lengthOfBytes;
                    }
                    return result;
                }
                #endif
                #ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    break;
                }
                #endif
                default:
                    break;
            }
        }
        return NULL;
    }
    void ComputeBuffer::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                usize size = this->elementCount * this->elementSize;

                VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                if (this->accessedAsVertexBuffer)
                {
                    usageFlags |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
                }
                if (this->accessedAsIndirectDrawData)
                {
                    usageFlags |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
                }
                if (this->CPUCanRead)
                {
                    usageFlags |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                }
                this->handle = AstralCanvasVk_CreateResourceBuffer(AstralCanvasVk_GetCurrentGPU(), size, usageFlags);

                this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void ComputeBuffer::FlagToClear()
    {
        computeBuffersToClear.Add(this);
    }
    void ComputeBuffer::ClearAllFlagged()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkCommandBuffer transientCmdBuffer = AstralCanvasVk_CreateTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, true);
                for (usize i = 0; i < computeBuffersToClear.count; i++)
                {
                    vkCmdFillBuffer(transientCmdBuffer, (VkBuffer)computeBuffersToClear.ptr[i]->handle, 0, computeBuffersToClear.ptr[i]->elementCount * computeBuffersToClear.ptr[i]->elementSize, 0);
                }
                AstralCanvasVk_EndTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, transientCmdBuffer);
                break;
            }
            #endif
            default:
                break;
        }
        computeBuffersToClear.Clear();
    }
    void ComputeBuffer::deinit()
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