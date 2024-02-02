#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanGPU.hpp"
#include "Json.hpp"
#include "ArenaAllocator.hpp"

using namespace SomnialJson;
using namespace AstralCanvas;

VkBuffer AstralCanvasVk_CreateResourceBuffer(AstralVulkanGPU *gpu, usize size, VkBufferUsageFlags usageFlags)
{
    VkBufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    createInfo.size = size;
    createInfo.usage = usageFlags;
    createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

    VkBuffer result;
    if (vkCreateBuffer(gpu->logicalDevice, &createInfo, NULL, &result) == VK_SUCCESS)
    {
        return result;
    }
    return NULL;
}
VkCommandBuffer AstralCanvasVk_CreateTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, bool alsoBeginBuffer)
{
    VkCommandBufferAllocateInfo allocInfo = {};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = 1;
    allocInfo.commandPool = queueToUse->transientCommandPool;
    allocInfo.pNext = NULL;

    queueToUse->commandPoolMutex.EnterLock();

    VkCommandBuffer result;
    if (vkAllocateCommandBuffers(gpu->logicalDevice, &allocInfo, &result) != VK_SUCCESS)
    {
        result = NULL;
    }

    queueToUse->commandPoolMutex.ExitLock();

    if (alsoBeginBuffer && result != NULL)
    {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.pInheritanceInfo = NULL;
        beginInfo.pNext = NULL;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(result, &beginInfo);
    }
    return result;
}
void AstralCanvasVk_EndTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, VkCommandBuffer commandBuffer)
{
    vkEndCommandBuffer(commandBuffer);

    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    //if we are on the graphics queue, the queue may also be used for presenting
    //hence, we need to wait for the queue to finish presenting before we can transition the image
    if (gpu->DedicatedGraphicsQueue.queue == queueToUse->queue)
    {
        queueToUse->queueMutex.EnterLock();

        vkQueueWaitIdle(queueToUse->queue);
        //alternatively, wait for queue fence
        //vkWaitForFences(gpu->logicalDevice, 1, &queueToUse->queueFence, true, UINT64_MAX);
        //vkResetFences(gpu->logicalDevice, 1, &queueToUse->queueFence);

        queueToUse->queueMutex.ExitLock();
    }

    //submit the queue
    queueToUse->queueMutex.EnterLock();

    if (vkQueueSubmit(queueToUse->queue, 1, &submitInfo, NULL) != VK_SUCCESS)
    {
        queueToUse->queueMutex.ExitLock();
        THROW_ERR("Failed to submit queue");
    }
    //todo: see if can transfer this to a fence or something
    //vkWaitForFences(gpu->logicalDevice, 1, &queueToUse->queueFence, true, UINT64_MAX);
    //vkResetFences(gpu->logicalDevice, 1, &queueToUse->queueFence);
    vkQueueWaitIdle(queueToUse->queue);

    queueToUse->queueMutex.ExitLock();

    //finally, free the command buffer
    queueToUse->commandPoolMutex.EnterLock();

    vkFreeCommandBuffers(gpu->logicalDevice, queueToUse->transientCommandPool, 1, &commandBuffer);

    queueToUse->commandPoolMutex.ExitLock();
}

void AstralCanvasVk_TransitionTextureLayout(AstralVulkanGPU *gpu, Texture2D *texture, VkImageAspectFlags aspectFlags, VkImageLayout newLayout)
{
    if (texture->imageLayout == (u32)newLayout)
    {
        return;
    }
    AstralCanvasVk_TransitionImageLayout(gpu, (VkImage)texture->imageHandle, texture->mipLevels, aspectFlags, (VkImageLayout)texture->imageLayout, newLayout);
    texture->imageLayout = newLayout;
}
void AstralCanvasVk_TransitionImageLayout(AstralVulkanGPU *gpu, VkImage imageHandle, u32 mipLevels, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
{
    VkImageMemoryBarrier memBarrier = {};
    memBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    memBarrier.oldLayout = oldLayout;
    memBarrier.newLayout = newLayout;
    memBarrier.srcQueueFamilyIndex = 0;
    memBarrier.dstQueueFamilyIndex = 0;
    memBarrier.image = imageHandle;
    memBarrier.subresourceRange.aspectMask = aspectFlags;
    memBarrier.subresourceRange.baseMipLevel = 0;
    memBarrier.subresourceRange.levelCount = mipLevels;
    memBarrier.subresourceRange.baseArrayLayer = 0;
    memBarrier.subresourceRange.layerCount = 1;

    VkPipelineStageFlags sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
    VkPipelineStageFlags destinationStage = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

    AstralCanvasVkCommandQueue *cmdQueue = &gpu->DedicatedGraphicsQueue;
    VkCommandBuffer cmdBuffer = AstralCanvasVk_CreateTransientCommandBuffer(gpu, cmdQueue, true);

    if (cmdBuffer != NULL)
    {
        VkPipelineStageFlags depthStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
        VkPipelineStageFlags sampledStageMask = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT;

        switch (oldLayout)
        {
            case VK_IMAGE_LAYOUT_UNDEFINED:
                break;

            case VK_IMAGE_LAYOUT_GENERAL:
                sourceStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_MEMORY_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                sourceStage = depthStageMask;
                memBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                sourceStage = depthStageMask | sampledStageMask;
                break;

            case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                sourceStage = sampledStageMask;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                break;

            case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PREINITIALIZED:
                sourceStage = VK_PIPELINE_STAGE_HOST_BIT;
                memBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
                break;

            case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                break;

            default:
                THROW_ERR("Invalid source layout!");
                break;
        }

        switch (newLayout)
        {
            switch (newLayout)
            {
                case VK_IMAGE_LAYOUT_GENERAL:
                    destinationStage = VK_PIPELINE_STAGE_ALL_COMMANDS_BIT;
                    memBarrier.dstAccessMask = VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
                    break;

                case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
                    destinationStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
                    memBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
                    break;

                case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
                    destinationStage = depthStageMask;
                    memBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
                    break;

                case VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL:
                    destinationStage = depthStageMask | sampledStageMask;
                    memBarrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                    break;

                case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
                    destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
                    memBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
                    break;

                case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
                    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                    memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
                    break;

                case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
                    destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
                    memBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
                    break;

                case VK_IMAGE_LAYOUT_PRESENT_SRC_KHR:
                    // vkQueuePresentKHR performs automatic visibility operations
                    break;

                default:
                    THROW_ERR("Invalid destination layout!");
            }
        }

        cmdQueue->commandPoolMutex.EnterLock();
        vkCmdPipelineBarrier(cmdBuffer, sourceStage, destinationStage, 0, 0, NULL, 0, NULL, 1, &memBarrier);
        cmdQueue->commandPoolMutex.ExitLock();

        AstralCanvasVk_EndTransientCommandBuffer(gpu, cmdQueue, cmdBuffer);
    }
}
void AstralCanvasVk_CopyBufferToBuffer(AstralVulkanGPU *gpu, VkBuffer from, VkBuffer to, usize copySize)
{
    VkCommandBuffer transientCmdBuffer = AstralCanvasVk_CreateTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, true);

    VkBufferCopy bufferCopy{};
    bufferCopy.size = copySize;
    bufferCopy.dstOffset = 0;
    bufferCopy.srcOffset = 0;

    vkCmdCopyBuffer(transientCmdBuffer, from, to, 1, &bufferCopy);

    AstralCanvasVk_EndTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, transientCmdBuffer);
}
void AstralCanvasVk_CopyBufferToImage(AstralVulkanGPU *gpu, VkBuffer from, VkImage imageHandle, u32 width, u32 height)
{
    VkCommandBuffer transientCmdBuffer = AstralCanvasVk_CreateTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, true);

    VkBufferImageCopy bufferImageCopy = {};
    
    bufferImageCopy.bufferOffset = 0;
    //only set values other than 0 if the image buffer is not tightly packed
    bufferImageCopy.bufferRowLength = 0;
    bufferImageCopy.bufferImageHeight = 0;

    bufferImageCopy.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    bufferImageCopy.imageSubresource.mipLevel = 0;
    bufferImageCopy.imageSubresource.baseArrayLayer = 0;
    bufferImageCopy.imageSubresource.layerCount = 1;

    bufferImageCopy.imageOffset = {};
    bufferImageCopy.imageExtent.width = width;
    bufferImageCopy.imageExtent.height = height;
    bufferImageCopy.imageExtent.depth = 1;

    vkCmdCopyBufferToImage(
        transientCmdBuffer,
        from,
        (VkImage)imageHandle,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        1, //length of things to copy
        &bufferImageCopy
        );

    AstralCanvasVk_EndTransientCommandBuffer(gpu, &gpu->DedicatedTransferQueue, transientCmdBuffer);
}

/*VkFramebuffer AstralCanvasVk_GetOrCreateRenderTarget(AstralVulkanGPU *gpu, RenderTarget *renderTarget, RenderProgram *forProgram)
{
    VkFramebufferCreateInfo createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    createInfo.width = renderTarget->width;
    createInfo.height = renderTarget->height;
    createInfo.layers = 1;

    VkImageView imageViews[2];
    imageViews[0] = (VkImageView)renderTarget->backendTexture.imageView;

    createInfo.attachmentCount = 1;
    if (renderTarget->depthBuffer.imageHandle != NULL)
    {
        createInfo.attachmentCount = 2;
        imageViews[1] = (VkImageView)renderTarget->depthBuffer.imageView;
    }

    if (renderTarget->isBackbuffer)
    {

    }
    else
    {
        
    }
    
}*/
#endif
