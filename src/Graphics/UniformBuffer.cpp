#include "Graphics/UniformBuffer.hpp"
#include "Graphics/CurrentBackend.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

namespace AstralCanvas
{
    UniformBuffer::UniformBuffer()
    {
        this->handle = NULL;
        this->size = 0;
        this->bindingPoint = 0;
    }
    UniformBuffer::UniformBuffer(usize size)
    {
        this->handle = NULL;
        this->size = size;
        this->bindingPoint = 0;

        this->Construct();
    }
    void UniformBuffer::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                
                VkBufferUsageFlags bufferUsage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
                this->handle = AstralCanvasVk_CreateResourceBuffer(AstralCanvasVk_GetCurrentGPU(), this->size, bufferUsage);
                this->memoryAllocation = AstralCanvasVk_AllocateMemoryForBuffer((VkBuffer)this->handle, VMA_MEMORY_USAGE_CPU_TO_GPU, (VkMemoryPropertyFlagBits)(VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT));
                break;
            }
            #endif
            default:
                break;
        }
    }
    void UniformBuffer::SetData(void *ptr, usize ptrSize)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                memcpy(this->memoryAllocation.vkAllocationInfo.pMappedData, ptr, ptrSize);
                break;
            }
            #endif
            default:
                break;
        }
    }
    void UniformBuffer::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                if (handle != NULL)
                    vkDestroyBuffer(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkBuffer)this->handle, NULL);
                if (this->memoryAllocation.vkAllocation != NULL)
                {
                    vmaFreeMemory(AstralCanvasVk_GetCurrentVulkanAllocator(), this->memoryAllocation.vkAllocation);
                    printf("Freed uniform buffer memory\n");
                }
                break;
            }
            #endif
            default:
                break;
        }
    }
}