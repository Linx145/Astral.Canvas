#include "Graphics/Compute.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"
#include "ArenaAllocator.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

#ifdef ASTRALCANVAS_OPENGL
#include "Graphics/Glad/glad.h"
#endif

namespace AstralCanvas
{
    ComputePipeline::ComputePipeline()
    {
        this->handle = NULL;
        this->shader = NULL;
#ifdef ASTRALCANVAS_VULKAN
        this->layout = NULL;
#endif
    }
    ComputePipeline::ComputePipeline(Shader *computeShader)
    {
        this->handle = NULL;
        this->shader = computeShader;
#ifdef ASTRALCANVAS_VULKAN
        this->layout = NULL;
#endif
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
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                u32 programHandle = glCreateProgram();

                glAttachShader(programHandle, (u32)shader->shaderModule1);
                glLinkProgram(programHandle);
                glDetachShader(programHandle, (u32)shader->shaderModule1);

                GLint isLinked = 0;
                glGetProgramiv(programHandle, GL_LINK_STATUS, (int*)&isLinked);
                if (isLinked == GL_FALSE)
                {
                    glDeleteProgram(programHandle);
                    THROW_ERR("Failed to link OpenGL compute pipeline!");
                }

                this->handle = (void*)programHandle;

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
                shader->uniformsHasBeenSet = false;
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

                vkCmdDispatch(commandBuffer, threadsX, threadsY, threadsZ);

                AstralCanvasVk_EndTransientCommandBuffer(AstralCanvasVk_GetCurrentGPU(), queueToUse, commandBuffer);
                
                shader->descriptorForThisDrawCall = 0;
                break;
            }
            #endif
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                glUseProgram((GLuint)this->handle);

                glDispatchCompute(threadsX, threadsY, threadsZ);

                glUseProgram(0);
                //glMemoryBarrier(GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
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
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                glDeleteProgram((GLuint)this->handle);
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: ComputePipeline deinit");
                break;
        }
    }
}