#include "Graphics/Vulkan/VulkanHelpers.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanGPU.hpp"
#include "Graphics/Vulkan/VulkanVertex.hpp"
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

        //alternatively, wait for queue idle
        vkWaitForFences(gpu->logicalDevice, 1, &queueToUse->queueFence, true, UINT64_MAX);

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
    vkQueueWaitIdle(queueToUse->queue);

    queueToUse->queueMutex.ExitLock();

    //finally, free the command buffer
    queueToUse->commandPoolMutex.EnterLock();

    vkFreeCommandBuffers(gpu->logicalDevice, queueToUse->transientCommandPool, 1, &commandBuffer);

    queueToUse->commandPoolMutex.ExitLock();
}

void AstralCanvasVk_TransitionTextureLayout(AstralVulkanGPU *gpu, VkImage imageHandle, u32 mipLevels, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout)
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
void AstralCanvasVk_CopyBufferToImage(AstralVulkanGPU *gpu, VkBuffer from, VkImage imageHandle, u32 width, u32 height)
{
    VkCommandBuffer transientCmdBuffer = AstralCanvasVk_CreateTransientCommandBuffer(gpu, &gpu->DedicatedGraphicsQueue, true);

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

    AstralCanvasVk_EndTransientCommandBuffer(gpu, &gpu->DedicatedGraphicsQueue, transientCmdBuffer);
}

VkFramebuffer AstralCanvasVk_GetOrCreateRenderTarget(AstralVulkanGPU *gpu, RenderTarget *renderTarget, RenderProgram *forProgram)
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

    //createInfo requires a renderpass to set what renderpass the framebuffer is
    //compatible with. Even if we pass in a different renderpass that is compatible

    //with the one it is created with, it will still be alright
    //https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility

    if (renderTarget->isBackbuffer)
    {

    }
    else
    {
        
    }
    
}

void AstralCanvasVk_DestroyRenderPipeline(RenderPipeline *pipeline)
{
    for (usize i = 0; i < pipeline->zoneToPipelineInstance.bucketsCount; i++)
    {
        if (pipeline->zoneToPipelineInstance.buckets[i].initialized)
        {
            for (usize j = 0; j < pipeline->zoneToPipelineInstance.buckets[i].entries.count; j++)
            {
                VkPipeline vkPipeline = (VkPipeline)pipeline->zoneToPipelineInstance.buckets[i].entries.ptr[j].value;
                vkDestroyPipeline(AstralCanvasVk_GetCurrentGPU()->logicalDevice, vkPipeline, NULL);
            }
        }
    }
    pipeline->zoneToPipelineInstance.deinit();
    //do not dispose shader
}
VkPipeline AstralCanvasVk_CreateRenderPipeline(RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPassToUse)
{
    //dynamic states
    IAllocator cAllocator = GetCAllocator();
    ArenaAllocator arena = ArenaAllocator(&cAllocator);

    const i32 dynamicStateCount = 2;
    collections::Array<VkDynamicState> dynamicStates = collections::Array<VkDynamicState>(&arena.asAllocator, dynamicStateCount);
    dynamicStates.data[0] = VK_DYNAMIC_STATE_VIEWPORT;
    dynamicStates.data[1] = VK_DYNAMIC_STATE_SCISSOR;

    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = dynamicStateCount;
    dynamicState.pDynamicStates = dynamicStates.data;

    //vertex declarations

    usize vertexDeclCount = pipeline->vertexDeclarations.length;
    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

    if (vertexDeclCount > 0)
    {
        usize attribCount = 0;
        collections::Array<AstralCanvasVk_VertexDecl> vertexDecls = collections::Array<AstralCanvasVk_VertexDecl>(&arena.asAllocator, vertexDeclCount);
        for (usize i = 0; i < vertexDeclCount; i++)
        {
            vertexDecls.data[i] = AstralCanvasVk_CreateVertexDecl(&arena.asAllocator, &pipeline->vertexDeclarations.data[i]);
            attribCount += vertexDecls.data[i].attributeDescriptions.length;
        }

        collections::Array<VkVertexInputBindingDescription> bindingDescriptions = collections::Array<VkVertexInputBindingDescription>(&arena.asAllocator, vertexDeclCount);
        for (usize i = 0; i < vertexDeclCount; i++)
        {
            bindingDescriptions.data[i] = vertexDecls.data[i].bindingDescription;
        }

        collections::Array<VkVertexInputAttributeDescription> attribDescriptions = collections::Array<VkVertexInputAttributeDescription>(&arena.asAllocator, attribCount);
        usize attribIndex = 0;
        for (usize i = 0; i < vertexDeclCount; i++)
        {
            for (usize j = 0; j < vertexDecls.data[i].attributeDescriptions.length; j++)
            {
                attribDescriptions.data[attribIndex] = vertexDecls.data[i].attributeDescriptions.data[j];
                attribIndex++;
            }
        }

        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInputInfo.vertexBindingDescriptionCount = vertexDeclCount;
        vertexInputInfo.pVertexBindingDescriptions = bindingDescriptions.data;
        vertexInputInfo.vertexAttributeDescriptionCount = attribCount;
        vertexInputInfo.pVertexAttributeDescriptions = attribDescriptions.data;
    }

    //primitive type

    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo{};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.topology = AstralCanvasVk_FromPrimitiveType(pipeline->primitiveType);
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    //viewport data

    VkPipelineViewportStateCreateInfo viewportStateInfo{};
    viewportStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportStateInfo.viewportCount = 1;
    viewportStateInfo.scissorCount = 1;

    //rasterization behaviour

    VkPipelineRasterizationStateCreateInfo rasterizerInfo{};
    rasterizerInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizerInfo.depthClampEnable = false;
    rasterizerInfo.rasterizerDiscardEnable = false;
    rasterizerInfo.polygonMode = VK_POLYGON_MODE_FILL;
    if (inputAssemblyInfo.topology == VK_PRIMITIVE_TOPOLOGY_LINE_LIST || inputAssemblyInfo.topology == VK_PRIMITIVE_TOPOLOGY_LINE_STRIP)
    {
        rasterizerInfo.polygonMode = VK_POLYGON_MODE_LINE;
    }
    rasterizerInfo.lineWidth = 1.0f;
    rasterizerInfo.cullMode = AstralCanvasVk_FromCullMode(pipeline->cullMode);
    rasterizerInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;

    rasterizerInfo.depthBiasEnable = false;
    rasterizerInfo.depthBiasConstantFactor = 0.0f;
    rasterizerInfo.depthBiasClamp = 0.0f;
    rasterizerInfo.depthBiasSlopeFactor = 0.0f;

    //multisampling data
    //todo

    VkPipelineMultisampleStateCreateInfo multisamplingInfo{};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.sampleShadingEnable = false;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.minSampleShading = 1.0f;
    multisamplingInfo.pSampleMask = nullptr;
    multisamplingInfo.alphaToCoverageEnable = false;
    multisamplingInfo.alphaToOneEnable = false;

    //depth stencil data
    
    VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
    depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencilInfo.depthTestEnable = pipeline->depthTest;
    depthStencilInfo.depthWriteEnable = pipeline->depthWrite;
    depthStencilInfo.depthCompareOp = pipeline->depthTest ? VK_COMPARE_OP_LESS_OR_EQUAL : VK_COMPARE_OP_NEVER;
    depthStencilInfo.depthBoundsTestEnable = false;
    depthStencilInfo.minDepthBounds = 0.0f;
    depthStencilInfo.maxDepthBounds = 1.0f;
    depthStencilInfo.stencilTestEnable = false;

    VkPipelineColorBlendAttachmentState colorBlendState {};
    colorBlendState.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT; // ColorComponentFlags.RBit | ColorComponentFlags.GBit | ColorComponentFlags.BBit | ColorComponentFlags.ABit;
    if (pipeline->blendState != NULL)
    {
        colorBlendState.srcColorBlendFactor = AstralCanvasVk_FromBlend(pipeline->blendState->sourceColorBlend);
        colorBlendState.srcAlphaBlendFactor = AstralCanvasVk_FromBlend(pipeline->blendState->sourceAlphaBlend);
        colorBlendState.dstColorBlendFactor = AstralCanvasVk_FromBlend(pipeline->blendState->destinationColorBlend);
        colorBlendState.dstAlphaBlendFactor = AstralCanvasVk_FromBlend(pipeline->blendState->destinationAlphaBlend);
        colorBlendState.colorBlendOp = VK_BLEND_OP_ADD;
        colorBlendState.alphaBlendOp = VK_BLEND_OP_ADD;
        colorBlendState.blendEnable = true;
    }
    else
        colorBlendState.blendEnable = false;

    //color blend data

    VkPipelineColorBlendStateCreateInfo colorBlendInfo {};
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.blendConstants[0] = 1.0f;
    colorBlendInfo.blendConstants[1] = 1.0f;
    colorBlendInfo.blendConstants[2] = 1.0f;
    colorBlendInfo.blendConstants[3] = 1.0f;
    colorBlendInfo.logicOpEnable = false;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendState;

    //pipeline layout itself

    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
    pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
    pipelineLayoutCreateInfo.flags = 0;

    //u32 descriptorsCount = pipeline->shader->shaderVariables.samplers.length + pipeline->shader->shaderVariables.textures.length + pipeline->shader->shaderVariables.uniforms.length;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    if (pipeline->shader->shaderPipelineLayout != NULL)
    {
        pipelineLayoutCreateInfo.setLayoutCount = 1;
        pipelineLayoutCreateInfo.pSetLayouts = (VkDescriptorSetLayout*)&pipeline->shader->shaderPipelineLayout;
    }

    if (vkCreatePipelineLayout(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &pipelineLayoutCreateInfo, NULL, (VkPipelineLayout*)&pipeline->layout) != VK_SUCCESS)
    {
        arena.deinit();
        return NULL;
    }

    //pipeline itself

    VkPipelineShaderStageCreateInfo shaderStageInfos[2] = {{}, {}};
    shaderStageInfos[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfos[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStageInfos[0].module = (VkShaderModule)pipeline->shader->shaderModule1;
    shaderStageInfos[0].pName = "main"; //entry point

    shaderStageInfos[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStageInfos[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStageInfos[1].module = (VkShaderModule)pipeline->shader->shaderModule2;
    shaderStageInfos[1].pName = "main"; //entry point

    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.pStages = shaderStageInfos;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pVertexInputState = &vertexInputInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineCreateInfo.pViewportState = &viewportStateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizerInfo;
    pipelineCreateInfo.pMultisampleState = &multisamplingInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendInfo;
    pipelineCreateInfo.pDepthStencilState = &depthStencilInfo;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.layout = (VkPipelineLayout)pipeline->layout;
    pipelineCreateInfo.renderPass = (VkRenderPass)renderProgram->handle;
    pipelineCreateInfo.subpass = renderPassToUse;

    VkPipeline result;
    vkCreateGraphicsPipelines(AstralCanvasVk_GetCurrentGPU()->logicalDevice, NULL, 1, &pipelineCreateInfo, NULL, &result);

    arena.deinit();

    return result;
}