#include "Graphics/Vulkan/VulkanQueueProperties.hpp"

i32 AstralCanvasVk_GetQueue(AstralVulkanQueueProperties *properties, VkPhysicalDevice physicalDevice, AstralVulkanQueue_Type type)
{
    switch (type)
    {
        case AstralVulkanQueue_Graphics:
            return AstralCanvasVk_GetGraphicsQueue(properties, physicalDevice, true);
        case AstralVulkanQueue_Transfer:
            return AstralCanvasVk_GetTransferQueue(properties, physicalDevice);
        case AstralVulkanQueue_Compute:
            return AstralCanvasVk_GetComputeQueue(properties, physicalDevice);
        default:
            return -1;
    }
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
            properties->dedicatedComputeQueueIndex = i;
        }
    }

    AstralVulkanQueueResult queueResult;
    queueResult.type = AstralVulkanQueue_Compute;
    queueResult.queueIndex = 0;
    queueResult.queueID = properties->dedicatedComputeQueueIndex;
    for (i32 i = 0; i < properties->queueCreationResults.count; i++)
    {
        if (properties->queueCreationResults.Get(i)->queueID == properties->dedicatedComputeQueueIndex)
        {
            queueResult.queueIndex += 1;
        }
    }
    properties->queueCreationResults.Add(queueResult);

    return properties->dedicatedComputeQueueIndex;
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

    AstralVulkanQueueResult queueResult;
    queueResult.type = AstralVulkanQueue_Transfer;
    queueResult.queueIndex = 0;
    queueResult.queueID = properties->dedicatedTransferQueueIndex;
    for (i32 i = 0; i < properties->queueCreationResults.count; i++)
    {
        if (properties->queueCreationResults.Get(i)->queueID == properties->dedicatedTransferQueueIndex)
        {
            queueResult.queueIndex += 1;
        }
    }
    properties->queueCreationResults.Add(queueResult);

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

    AstralVulkanQueueResult queueResult;
    queueResult.type = AstralVulkanQueue_Graphics;
    queueResult.queueIndex = 0;
    queueResult.queueID = properties->dedicatedGraphicsQueueIndex;
    for (i32 i = 0; i < properties->queueCreationResults.count; i++)
    {
        if (properties->queueCreationResults.Get(i)->queueID == properties->dedicatedGraphicsQueueIndex)
        {
            queueResult.queueIndex += 1;
        }
    }
    properties->queueCreationResults.Add(queueResult);

    return properties->dedicatedGraphicsQueueIndex;
}