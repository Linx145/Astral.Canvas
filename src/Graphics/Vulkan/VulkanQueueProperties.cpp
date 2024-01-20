#include "Graphics/Vulkan/VulkanQueueProperties.hpp"

i32 AstralCanvasVk_GetQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, AstralVulkanQueue_Type type)
{
    switch (type)
    {
        case AstralVulkanQueue_GeneralPurpose:
            return AstralCanvasVk_GetGeneralPurposeQueue(properties, physicalDevice, true);
        case AstralVulkanQueue_Graphics:
            return AstralCanvasVk_GetGraphicsQueue(properties, physicalDevice, false);
        case AstralVulkanQueue_Transfer:
            return AstralCanvasVk_GetTransferQueue(properties, physicalDevice);
        case AstralVulkanQueue_Compute:
            return AstralCanvasVk_GetComputeQueue(properties, physicalDevice);
        default:
            return -1;
    }
}

i32 AstralCanvasVk_GetGeneralPurposeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent)
{
    if (properties->generalPurposeQueueIndex != -1) return properties->generalPurposeQueueIndex;

    i32 maxScore = -100;
    for (usize i = 0; i < properties->all.length; i++)
    {
        VkQueueFamilyProperties* property = &properties->all.data[i];
        if ((property->queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            if (mustPresent)
            {
                VkBool32 isSupported = VkBool32(false);
                vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, properties->windowSurface, &isSupported);
                //VkEngine.KhrSurfaceAPI.GetPhysicalDeviceSurfaceSupport(device, i, VkEngine.WindowSurface, &isSupported);
                if (!isSupported)
                {
                    continue;
                }
            }
            i32 score = 0;
            if ((property->queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
            {
                score++;
            }
            if ((property->queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
            {
                score++;
            }
            if ((property->queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
            {
                score++;
            }
            if ((property->queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) != 0)
            {
                score++;
            }

            if (score > maxScore)
            {
                maxScore = score;
                properties->generalPurposeQueueIndex = i;
            }
        }
    }
    return properties->generalPurposeQueueIndex;
}

i32 AstralCanvasVk_GetComputeQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice)
{
    if (properties->dedicatedComputeQueueIndex != -1) return properties->dedicatedComputeQueueIndex;

    i32 maxScore = -100;
    for (usize i = 0; i < properties->all.length; i++)
    {
        VkQueueFamilyProperties* property = &properties->all.data[i];

        i32 score = 0;

        if ((property->queueFlags & VK_QUEUE_COMPUTE_BIT) == 0)
        {
            continue;
        }
        if ((property->queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            score--;
        }
        if ((property->queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR) != 0)
        {
            score--;
        }
        /*if ((property.QueueFlags & QueueFlags.TransferBit) == 0)
        {
            score--;
        }*/

        if (score > maxScore)
        {
            maxScore = score;
            properties->dedicatedTransferQueueIndex = i;
        }
    }

    return properties->dedicatedTransferQueueIndex;
}

i32 AstralCanvasVk_GetTransferQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice)
{
    if (properties->dedicatedTransferQueueIndex != -1) return properties->dedicatedTransferQueueIndex;

    i32 maxScore = -100;
    for (usize i = 0; i < properties->all.length; i++)
    {
        VkQueueFamilyProperties *property = &properties->all.data[i];
        i32 score = 0;
        //if we dont have a transfer bit, dont bother with this queue
        if ((property->queueFlags & VK_QUEUE_TRANSFER_BIT) == 0)
        {
            continue;
        }
        if ((property->queueFlags & VK_QUEUE_GRAPHICS_BIT) != 0)
        {
            score--;
        }
        if ((property->queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
        {
            score--;
        }

        if (score > maxScore)
        {
            maxScore = score;
            properties->dedicatedTransferQueueIndex = i;
        }
    }
    return properties->dedicatedTransferQueueIndex;
}

i32 AstralCanvasVk_GetGraphicsQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, bool mustPresent)
{
    if (properties->dedicatedGraphicsQueueIndex != -1) return properties->dedicatedGraphicsQueueIndex;

    i32 maxScore = -100;
    for (usize i = 0; i < properties->all.length; i++)
    {
        VkQueueFamilyProperties *property = &properties->all.data[i];
        if (mustPresent)
        {
            VkBool32 isSupported = VkBool32(false);
            vkGetPhysicalDeviceSurfaceSupportKHR(physicalDevice, i, properties->windowSurface, &isSupported);
            if (!isSupported)
            {
                continue;
            }
        }
        int score = 0;
        //if we dont have a transfer bit, dont bother with this queue
        if ((property->queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0)
        {
            continue;
        }
        if ((property->queueFlags & VK_QUEUE_TRANSFER_BIT) != 0)
        {
            score--;
        }
        if ((property->queueFlags & VK_QUEUE_COMPUTE_BIT) != 0)
        {
            score--;
        }
        //ignore sparse binding bit
        /*if ((property.QueueFlags & QueueFlags.SparseBindingBit) != 0)
        {
            score--;
        }*/

        if (score > maxScore)
        {
            maxScore = score;
            properties->dedicatedGraphicsQueueIndex = i;
        }
    }
    return properties->dedicatedGraphicsQueueIndex;
}