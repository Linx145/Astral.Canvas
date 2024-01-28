#include "Graphics/Graphics.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "Graphics/Vulkan/VulkanHelpers.hpp"
#endif

namespace AstralCanvas
{
    usize bindBufferNoOffsets = 0;

    Graphics::Graphics()
    {
        this->currentRenderPass = 0;
        this->currentRenderPipeline = NULL;
        this->currentRenderProgram = NULL;
        this->currentRenderTarget = NULL;
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
            default:
                break;
        }
    }
    void Graphics::SetRenderTarget(RenderTarget *target)
    {
        if (currentRenderPipeline == NULL)
        {
            currentRenderTarget = target;
        }
    }
    void Graphics::StartRenderProgram(RenderProgram *program, const Color clearColor)
    {
        this->currentRenderProgram = program;
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                RenderTarget *renderTarget = currentRenderTarget;
                if (renderTarget == NULL)
                {
                    AstralVulkanSwapchain *swapchain = AstralCanvasVk_GetCurrentSwapchain();
                    renderTarget = &swapchain->renderTargets.data[swapchain->currentImageIndex];
                }

                AstralCanvasVk_TransitionTextureLayout(AstralCanvasVk_GetCurrentGPU(), &renderTarget->backendTexture, VK_IMAGE_ASPECT_COLOR_BIT, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL);

                VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                renderTarget->Construct(this->currentRenderProgram);

                VkRenderPassBeginInfo info{};
                info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
                info.framebuffer = (VkFramebuffer)renderTarget->renderTargetHandle;
                info.renderPass = (VkRenderPass)program->handle;
                VkClearValue clearValues[2];
                
                clearValues[0].color.float32[0] = clearColor.R * ONE_OVER_255;
                clearValues[0].color.float32[1] = clearColor.G * ONE_OVER_255;
                clearValues[0].color.float32[2] = clearColor.B * ONE_OVER_255;
                clearValues[0].color.float32[3] = clearColor.A * ONE_OVER_255;

                clearValues[1].depthStencil.depth = 1.0f;
                clearValues[1].depthStencil.stencil = 255;

                RenderProgramImageAttachment attachment = this->currentRenderProgram->attachments.ptr[this->currentRenderProgram->renderPasses.ptr[0].colorAttachmentIndices.data[0]];
                if (attachment.clearColor)
                {
                    info.pClearValues = clearValues;
                    info.clearValueCount = 1;
                }
                if (this->currentRenderProgram->renderPasses.ptr[0].depthAttachmentIndex != -1)
                {
                    info.clearValueCount++;
                }
                info.renderArea.offset.x = 0;
                info.renderArea.offset.y = 0;
                info.renderArea.extent.width = renderTarget->width;
                info.renderArea.extent.height = renderTarget->height;

                vkCmdBeginRenderPass(cmdBuffer, &info, VK_SUBPASS_CONTENTS_INLINE);
            }
            #endif
            default:
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
                }
                #endif
                default:
                    break;
            }
        }
        this->currentRenderProgram = NULL;
    }
    void Graphics::UseRenderPipeline(RenderPipeline *pipeline)
    {
        if (this->currentRenderPipeline == NULL && this->currentRenderProgram != NULL)
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
                    viewport.x = this->Viewport.X;
                    viewport.y = this->Viewport.Y;
                    viewport.width = this->Viewport.Width;
                    viewport.height = this->Viewport.Height;

                    vkCmdSetViewport(cmdBuffer, 0, 1, &viewport);

                    VkRect2D clip;
                    clip.extent.width = this->ClipArea.Width;
                    clip.extent.height = this->ClipArea.Height;
                    clip.offset.x = this->ClipArea.X;
                    clip.offset.y = this->ClipArea.Y;
                    vkCmdSetScissor(cmdBuffer, 0, 1, &clip);
                }
                #endif
                default:
                    break;
            }
            this->currentRenderPipeline = pipeline;
        }
    }

    void Graphics::DrawIndexedPrimitives(u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
    {
        if (this->currentRenderPipeline != NULL && this->currentRenderProgram != NULL)
        {
            switch (GetActiveBackend())
            {
                #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkCommandBuffer cmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

                    vkCmdDrawIndexed(cmdBuffer, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
                }
                #endif
                default:
                    break;
            }
            this->currentRenderPipeline = NULL;
        }
    }
}