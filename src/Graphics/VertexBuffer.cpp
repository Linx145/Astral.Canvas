#include "Graphics/VertexBuffer.hpp"
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
    VertexBuffer::VertexBuffer()
    {
        this->handle = NULL;
        this->memoryAllocation.unused = 0;
        this->vertexCount = 0;
        this->vertexType = NULL;
        this->isDynamic = false;
    }
    VertexBuffer::VertexBuffer(VertexDeclaration *thisVertexType, usize vertexCount, bool isDynamic, bool canRead)
    {
        this->canRead = canRead;
        this->vertexType = thisVertexType;
        this->vertexCount = vertexCount;
        this->handle = NULL;
        this->memoryAllocation.unused = 0;
        this->isDynamic = isDynamic;

        this->Construct();
    }
    void VertexBuffer::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
                if (this->canRead)
                {
                    bufferUsage |= VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
                }
                this->handle = AstralCanvasVk_CreateResourceBuffer(AstralCanvasVk_GetCurrentGPU(), this->vertexCount * this->vertexType->size, bufferUsage);
                if (this->isDynamic)
                {
                    this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                }
                else
                {
                    this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_GPU_ONLY, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                }
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_CreateVertexBuffer(this);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void VertexBuffer::SetData(void* verticesData, usize count)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                if (this->isDynamic)
                {
                    memcpy(this->memoryAllocation.vkAllocationInfo.pMappedData, verticesData, count * this->vertexType->size);
                }
                else
                {
                    usize lengthOfBytes = count * this->vertexType->size;
                    AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                    VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, lengthOfBytes, VK_BUFFER_USAGE_TRANSFER_SRC_BIT);
                    MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT);

                    memcpy(stagingMemory.vkAllocationInfo.pMappedData, verticesData, lengthOfBytes);

                    AstralCanvasVk_CopyBufferToBuffer(gpu, stagingBuffer, (VkBuffer)this->handle, lengthOfBytes);

                    vkDestroyBuffer(gpu->logicalDevice, stagingBuffer, NULL);
                    vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), stagingMemory.vkAllocation);
                }
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SetVertexData(this, verticesData, count);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void *VertexBuffer::GetData(IAllocator *allocator, usize* dataLength)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                usize lengthOfBytes = this->vertexCount * this->vertexType->size;
                AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
                VkBuffer stagingBuffer = AstralCanvasVk_CreateResourceBuffer(gpu, lengthOfBytes, VK_BUFFER_USAGE_TRANSFER_DST_BIT);
                MemoryAllocation stagingMemory = AstralCanvasVk_AllocateMemoryForBuffer(stagingBuffer, VMA_MEMORY_USAGE_GPU_TO_CPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT));

                AstralCanvasVk_CopyBufferToBuffer(gpu, (VkBuffer)this->handle, stagingBuffer, lengthOfBytes);

                void *result = allocator->Allocate(lengthOfBytes);
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
        return NULL;
    }
    void VertexBuffer::deinit()
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
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_DestroyVertexBuffer(this);
                break;
            }
            #endif
            default:
                break;
        }
    }
}
