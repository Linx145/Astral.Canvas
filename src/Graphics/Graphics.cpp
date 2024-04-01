#include "Graphics/Graphics.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "hash.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalImplementations.h"
#include "ErrorHandling.hpp"
#endif

namespace AstralCanvas
{
    usize bindBufferNoOffsets = 0;

    Graphics::Graphics()
    {
        this->currentCommandEncoderInstance = NULL;
        this->currentRenderPass = 0;
        this->currentRenderPipeline = NULL;
        this->currentRenderProgram = NULL;
        this->currentRenderTarget = NULL;
    }
    void Graphics::AwaitGraphicsIdle()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralCanvasVkCommandQueue *queue = &AstralCanvasVk_GetCurrentGPU()->DedicatedGraphicsQueue;
                queue->queueMutex.EnterLock();
                vkQueueWaitIdle(queue->queue);
                queue->queueMutex.ExitLock();
                break;
            }
            #endif
        }
    }
    void Graphics::SetVertexBuffer(const VertexBuffer *vb, u32 bindingPoint)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                vkCmdBindVertexBuffers(cmdBuffer, bindingPoint, 1, (VkBuffer*)&vb->handle, &bindBufferNoOffsets);
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SetVertexBuffer(this->currentCommandEncoderInstance, vb, bindingPoint);
                break;
            }
#endif
            default:
                break;
        }
    }
    void Graphics::SetInstanceBuffer(const InstanceBuffer *instanceBuffer, u32 bindingPoint)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                vkCmdBindVertexBuffers(cmdBuffer, bindingPoint, 1, (VkBuffer*)&instanceBuffer->handle, &bindBufferNoOffsets);
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SetInstanceBuffer(this->currentCommandEncoderInstance, instanceBuffer, bindingPoint);
                break;
            }
#endif
            default:
                break;
        }
    }
    void Graphics::SetIndexBuffer(const IndexBuffer *indexBuffer)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                vkCmdBindIndexBuffer(cmdBuffer, (VkBuffer)indexBuffer->handle, 0, indexBuffer->indexElementSize == IndexBufferSize_U16 ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32);
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                //this is set at draw command time
                //AstralCanvasMetal_SetIndexBuffer(this->currentCommandEncoderInstance, indexBuffer);
                break;
            }
#endif
            default:
                break;
        }
        this->currentIndexBuffer = indexBuffer;
    }
    void Graphics::SetRenderTarget(RenderTarget *target)
    {
        if (currentRenderProgram == NULL)
        {
            currentRenderTarget = target;
        }
    }
    void Graphics::SetClipArea(Maths::Rectangle newClipArea)
    {
        this->ClipArea = newClipArea;
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                VkRect2D clip;
                clip.extent.width = this->ClipArea.Width;
                clip.extent.height = this->ClipArea.Height;
                clip.offset.x = this->ClipArea.X;
                clip.offset.y = this->ClipArea.Y;
                vkCmdSetScissor(cmdBuffer, 0, 1, &clip);
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SetClipArea(currentCommandEncoderInstance, newClipArea);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: Graphics SetClipArea");
                break;
        }
    }
    void Graphics::StartRenderProgram(RenderProgram *program, const Color clearColor)
    {
        this->currentRenderProgram = program;
        this->clearColor = clearColor;
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                RenderTarget *renderTarget = currentRenderTarget;
                if (renderTarget == NULL)
                {
                    AstralVulkanSwapchain *swapchain = (AstralVulkanSwapchain *)this->currentWindow->swapchain;
                    renderTarget = &swapchain->renderTargets.data[swapchain->currentImageIndex];
                }

                AstralCanvasVkTextureToTransition *textures = (AstralCanvasVkTextureToTransition*)malloc(sizeof(AstralCanvasVkTextureToTransition) * renderTarget->textures.length);
                for (usize i = 0; i < renderTarget->textures.length; i++)
                {
                    textures[i].texture = &renderTarget->textures.data[i];
                    if (renderTarget->textures.data[i].imageFormat > ImageFormat_DepthNone)
                    {
                        if (renderTarget->textures.data[i].imageFormat == ImageFormat_Depth24Stencil8
                        || renderTarget->textures.data[i].imageFormat == ImageFormat_Depth16Stencil8)
                        {
                            textures[i].aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
                        }
                        else
                        {
                            textures[i].aspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT;
                        }
                        textures[i].newLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
                    }
                    else
                    {
                        textures[i].aspectFlags = VK_IMAGE_ASPECT_COLOR_BIT;
                        textures[i].newLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                    }
                }
                AstralCanvasVk_TransitionTextureLayouts(AstralCanvasVk_GetCurrentGPU(), textures, renderTarget->textures.length);
                free(textures);

                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                renderTarget->Construct(this->currentRenderProgram);

                VkRenderPassBeginInfo info{};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.framebuffer = (VkFramebuffer)renderTarget->renderTargetHandle;
                info.renderPass = (VkRenderPass)program->handle;

                //clear values
                //arbitrarily large number
                VkClearValue clearValues[32];

                info.clearValueCount = this->currentRenderProgram->attachments.count;
                info.pClearValues = clearValues;
                for (usize i = 0; i < this->currentRenderProgram->attachments.count; i++)
                {
                    clearValues[i] = {};
                    RenderProgramImageAttachment attachment = this->currentRenderProgram->attachments.ptr[i];

                    if (attachment.clearColor && attachment.imageFormat < ImageFormat_DepthNone)
                    {
                        clearValues[i].color.float32[0] = clearColor.R * ONE_OVER_255;
                        clearValues[i].color.float32[1] = clearColor.G * ONE_OVER_255;
                        clearValues[i].color.float32[2] = clearColor.B * ONE_OVER_255;
                        clearValues[i].color.float32[3] = clearColor.A * ONE_OVER_255;
                    }
                    else if (attachment.clearDepth && attachment.imageFormat > ImageFormat_DepthNone)
                    {
                        clearValues[i].depthStencil.depth = 1.0f;
                        clearValues[i].depthStencil.stencil = 255;
                    }
                }

                info.renderArea.offset.x = 0;
                info.renderArea.offset.y = 0;
                info.renderArea.extent.width = renderTarget->width;
                info.renderArea.extent.height = renderTarget->height;

                vkCmdBeginRenderPass(cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                void* cmdBufferEncoder = AstralCanvasMetal_StartRenderProgram(currentRenderProgram, currentRenderTarget, currentRenderPass, clearColor);
                this->currentCommandEncoderInstance = cmdBufferEncoder;
                break;
            }
#endif
            default:
                break;
        }
    }
    void Graphics::NextRenderPass()
    {
        currentRenderPass += 1;
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();
                
                vkCmdNextSubpass(cmdBuffer, VK_SUBPASS_CONTENTS_INLINE);
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                if (currentCommandEncoderInstance != NULL)
                {
                    //apparently this is how it works
                    AstralCanvasMetal_EndRenderProgram(this->currentCommandEncoderInstance);
                    StartRenderProgram(this->currentRenderProgram, clearColor);
                }
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: RenderProgram NextRenderPass");
                break;
        }
    }
    void Graphics::EndRenderProgram()
    {
        if (this->currentRenderProgram != NULL)
        {
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    vkCmdEndRenderPass(cmdBuffer);

                    RenderTarget *renderTarget = currentRenderTarget;
                    if (renderTarget == NULL)
                    {
                        AstralVulkanSwapchain *swapchain = (AstralVulkanSwapchain *)this->currentWindow->swapchain;
                        renderTarget = &swapchain->renderTargets.data[swapchain->currentImageIndex];
                    }
                    if (renderTarget != NULL)
                    {
                        for (usize i = 0; i < currentRenderProgram->attachments.count; i++)
                        {
                            if (renderTarget->textures.data[i].imageFormat < ImageFormat_DepthNone)
                            {
                                u64 finalImageLayout;
                                RenderProgramImageAttachment attachmentData = currentRenderProgram->attachments.ptr[i];
                                if (attachmentData.outputType == RenderPassOutput_ToNextPass)// || i < program->attachments.count - 1)
                                {
                                    finalImageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
                                }
                                else if (attachmentData.outputType == RenderPassOutput_ToRenderTarget)
                                {
                                    finalImageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                                }
                                else
                                {
                                    finalImageLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
                                }
                                renderTarget->textures.data[i].imageLayout = finalImageLayout;
                            }  
                        }
                    }
                    break;
                }
                #endif
#ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    if (currentCommandEncoderInstance != NULL)
                        AstralCanvasMetal_EndRenderProgram(this->currentCommandEncoderInstance);
                    break;
                }
#endif
                default:
                    break;
            }
        }
        this->currentRenderPass = 0;
        this->currentRenderProgram = NULL;
    }
    void Graphics::UseRenderPipeline(RenderPipeline *pipeline)
    {
        if (this->currentRenderProgram != NULL)
        {
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    void *handle = pipeline->GetOrCreateFor(this->currentRenderProgram, currentRenderPass);
                    
                    vkCmdBindPipeline(cmdBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, (VkPipeline)handle);

                    VkViewport viewport{};
                    viewport.minDepth = 0.0f;
                    viewport.maxDepth = 1.0f;
                    viewport.x = (float)this->Viewport.X;
                    viewport.y = (float)this->Viewport.Y;
                    viewport.width = (float)this->Viewport.Width;
                    viewport.height = (float)this->Viewport.Height;

                    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

                    VkRect2D clip;
                    clip.extent.width = this->ClipArea.Width;
                    clip.extent.height = this->ClipArea.Height;
                    clip.offset.x = this->ClipArea.X;
                    clip.offset.y = this->ClipArea.Y;
                    vkCmdSetScissor(cmdBuffer, 0, 1, &clip);
                    break;
                }
                #endif
#ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    AstralCanvasMetal_UseRenderPipeline(this->currentCommandEncoderInstance, pipeline, this->currentRenderProgram, this->currentRenderPass, this->Viewport, this->ClipArea);
                    break;
                }
#endif
                default:
                    break;
            }
            this->currentRenderPipeline = pipeline;
            this->usedShaders.Add(pipeline->shader);
        }
    }
    void Graphics::SetShaderVariable(const char* variableName, void* ptr, usize size)
    {
        if (currentRenderPipeline != NULL)
        {
            currentRenderPipeline->shader->SetShaderVariable(variableName, ptr, size);
        }
    }
    void Graphics::SetShaderVariableTextures(const char* variableName, Texture2D **textures, usize count)
    {
        if (currentRenderPipeline != NULL)
        {
            currentRenderPipeline->shader->SetShaderVariableTextures(variableName, textures, count);
        }
    }
    void Graphics::SetShaderVariableTexture(const char* variableName, Texture2D *texture)
    {
        if (currentRenderPipeline != NULL)
        {
            currentRenderPipeline->shader->SetShaderVariableTexture(variableName, texture);
        }
    }
    void Graphics::SetShaderVariableSamplers(const char* variableName, SamplerState **samplers, usize count)
    {
        if (currentRenderPipeline != NULL)
        {
            currentRenderPipeline->shader->SetShaderVariableSamplers(variableName, samplers, count);
        }
    }
    void Graphics::SetShaderVariableSampler(const char* variableName, SamplerState *sampler)
    {
        if (currentRenderPipeline != NULL)
        {
            currentRenderPipeline->shader->SetShaderVariableSampler(variableName, sampler);
        }
    }
    
    void Graphics::SendUpdatedUniforms()
    {
        if (currentRenderPipeline->shader->uniformsHasBeenSet)
        {
            currentRenderPipeline->shader->uniformsHasBeenSet = false;

            currentRenderPipeline->shader->SyncUniformsWithGPU(this->currentCommandEncoderInstance);
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    vkCmdBindDescriptorSets(
                        AstralCanvasVk_GetMainCmdBuffer(), 
                        VK_PIPELINE_BIND_POINT_GRAPHICS, 
                        (VkPipelineLayout)currentRenderPipeline->layout, 
                        0, 1, //descriptor set count
                        (VkDescriptorSet*)&currentRenderPipeline->shader->descriptorSets.ptr[currentRenderPipeline->shader->descriptorForThisDrawCall],
                        0, NULL); //dynamic offsets count
                    break;
                }
                #endif
                default:
                    break;
            }
            currentRenderPipeline->shader->descriptorForThisDrawCall += 1;
        }

    }

    void Graphics::DrawIndexedPrimitivesIndirect(ComputeBuffer* drawDataBuffer, usize drawDataBufferOffset, u32 drawCount)
    {
        if (this->currentRenderPipeline != NULL && this->currentRenderProgram != NULL)
        {
            SendUpdatedUniforms();
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    vkCmdDrawIndexedIndirect(cmdBuffer, (VkBuffer)drawDataBuffer->handle, drawDataBufferOffset, drawCount, sizeof(DrawIndexedIndirectCommand));
                    //vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                    break;
                }
                #endif
#ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    AstralCanvasMetal_DrawIndexedPrimitives(this->currentCommandEncoderInstance, this->currentIndexBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                    break;
                }
#endif
                default:
                    break;
            }
        }
    }
    void Graphics::DrawIndexedPrimitivesIndirectCount(ComputeBuffer *drawDataBuffer, usize drawDataBufferOffset, ComputeBuffer *drawCountBuffer, usize drawCountBufferOffset, u32 maxDrawCount)
    {
        if (this->currentRenderPipeline != NULL && this->currentRenderProgram != NULL)
        {
            SendUpdatedUniforms();
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    vkCmdDrawIndexedIndirectCount(cmdBuffer, (VkBuffer)drawDataBuffer->handle, drawDataBufferOffset, (VkBuffer)drawCountBuffer->handle, drawCountBufferOffset, maxDrawCount, sizeof(DrawIndexedIndirectCommand));
                    //vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                    break;
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
    }
    void Graphics::DrawIndexedPrimitives(u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
    {
        if (this->currentRenderPipeline != NULL && this->currentRenderProgram != NULL)
        {
            SendUpdatedUniforms();
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                    break;
                }
                #endif
#ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                {
                    AstralCanvasMetal_DrawIndexedPrimitives(this->currentCommandEncoderInstance, this->currentIndexBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                    break;
                }
#endif
                default:
                    break;
            }
            //this->currentRenderPipeline = NULL;
        }
    }
}
