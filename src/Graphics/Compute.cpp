#include "Graphics/Compute.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"
#include "ArenaAllocator.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

namespace AstralCanvas
{
    ComputePipeline::ComputePipeline()
    {
        this->handle = NULL;
        this->shader = NULL;
        this->layout = NULL;
    }
    ComputePipeline::ComputePipeline(Shader *computeShader)
    {
        this->handle = NULL;
        this->shader = computeShader;
        this->layout = NULL;
        this->Construct();
    }
    void ComputePipeline::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                if (layout == NULL)
                {
                    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
                    pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
                    pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
                    pipelineLayoutCreateInfo.pPushConstantRanges = NULL;
                    pipelineLayoutCreateInfo.flags = 0;

                    pipelineLayoutCreateInfo.setLayoutCount = 0;
                    if (shader->shaderPipelineLayout != NULL)
                    {
                        pipelineLayoutCreateInfo.setLayoutCount = 1;
                        pipelineLayoutCreateInfo.pSetLayouts = (VkDescriptorSetLayout*)&shader->shaderPipelineLayout;
                    }

                    if (vkCreatePipelineLayout(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &pipelineLayoutCreateInfo, NULL, (VkPipelineLayout*)&this->layout) != VK_SUCCESS)
                    {
                        break;
                    }
                }

                VkPipelineShaderStageCreateInfo computeStageInfo{};
                computeStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
                computeStageInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
                computeStageInfo.module = (VkShaderModule)this->shader->shaderModule1;
                computeStageInfo.pName = "main";

                VkComputePipelineCreateInfo pipelineCreateInfo{};
                pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
                pipelineCreateInfo.stage = computeStageInfo;
                pipelineCreateInfo.layout = layout;

                VkPipeline pipeline;
                if (vkCreateComputePipelines(AstralCanvasVk_GetCurrentGPU()->logicalDevice, NULL, 1, &pipelineCreateInfo, NULL, &pipeline) != VK_SUCCESS)
                {
                    string errMsg = string(GetCAllocator(), "Failed to create compute pipeline for shader");
                    THROW_ERR(errMsg.buffer);
                    errMsg.deinit();
                }

                this->handle = pipeline;
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: ComputePipeline Construct");
                break;
        }
    }
    void ComputePipeline::DispatchNow(i32 threadsX, i32 threadsY, i32 threadsZ)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                shader->SyncUniformsWithGPU(NULL);

                AstralCanvasVkCommandQueue* queueToUse = &AstralCanvasVk_GetCurrentGPU()->DedicatedComputeQueue;
                VkCommandBuffer commandBuffer = AstralCanvasVk_CreateTransientCommandBuffer(AstralCanvasVk_GetCurrentGPU(), queueToUse, true);

                vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_COMPUTE, (VkPipeline)this->handle);
                vkCmdBindDescriptorSets(
                    commandBuffer, 
                    VK_PIPELINE_BIND_POINT_COMPUTE, 
                    (VkPipelineLayout)layout, 
                    0, 1, //descriptor set count
                    (VkDescriptorSet*)&shader->descriptorSets.ptr[shader->descriptorForThisDrawCall],
                    0, NULL); //dynamic offsets count
                shader->descriptorForThisDrawCall += 1;

                vkCmdDispatch(commandBuffer, threadsX, threadsY, threadsZ);

                AstralCanvasVk_EndTransientCommandBuffer(AstralCanvasVk_GetCurrentGPU(), queueToUse, commandBuffer);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: ComputePipeline DispatchNow");
                break;
        }
    }
    void ComputePipeline::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                vkDestroyPipelineLayout(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkPipelineLayout)layout, NULL);
                vkDestroyPipeline(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkPipeline)handle, NULL);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: ComputePipeline deinit");
                break;
        }
    }
}