#include "Graphics/InstanceBuffer.hpp"
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
    InstanceBuffer::InstanceBuffer()
    {
        this->handle = NULL;
        this->memoryAllocation.unused = 0;
        this->instanceCount = 0;
        this->instanceSize = 0;
    }
    InstanceBuffer::InstanceBuffer(usize instanceSize, usize instanceCount, bool canRead)
    {
        this->canRead = canRead;
        this->instanceSize = instanceSize;
        this->instanceCount = instanceCount;
        this->handle = NULL;
        this->memoryAllocation.unused = 0;

        this->Construct();
    }
    void InstanceBuffer::Construct()
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
                this->handle = AstralCanvasVk_CreateResourceBuffer(AstralCanvasVk_GetCurrentGPU(), this->instanceCount * this->instanceSize, bufferUsage);

                this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_CPU_TO_GPU, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_CreateInstanceBuffer(this);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void InstanceBuffer::SetData(void* instancesData, usize count)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                memcpy(this->memoryAllocation.vkAllocationInfo.pMappedData, instancesData, count * this->instanceSize);

                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SetInstanceData(this, instancesData, count);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void InstanceBuffer::deinit()
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
                AstralCanvasMetal_DestroyInstanceBuffer(this);
                break;
            }
            #endif
            default:
                break;
        }
    }
}
