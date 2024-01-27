#include "Graphics/RenderTarget.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#endif

namespace AstralCanvas
{
    void RenderTarget::deinit()
    {
        if (this->isDisposed || !this->constructed)
        {
            return;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                vkDestroyFramebuffer(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkFramebuffer)this->renderTargetHandle, NULL);
                this->backendTexture.deinit();
                this->depthBuffer.deinit();
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }
        this->isDisposed = true;
    }
    void RenderTarget::Construct(RenderProgram *renderProgram)
    {
        if (this->constructed)
        {
            return;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkFramebufferCreateInfo createInfo{};
                createInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
                createInfo.width = this->width;
                createInfo.height = this->height;
                createInfo.flags = 0;
                createInfo.layers = 1;
                createInfo.attachmentCount = 1;
                if (this->depthBuffer.constructed)
                {
                    createInfo.attachmentCount = 2;
                }
                VkImageView imageViews[2] = {};
                imageViews[0] = (VkImageView)this->backendTexture.imageView;
                imageViews[1] = (VkImageView)this->depthBuffer.imageView;
                createInfo.pAttachments = imageViews;

                //createInfo requires a renderpass to set what renderpass the framebuffer is
                //compatible with. Even if we pass in a different renderpass that is compatible
                //with the one it is created with, it will still be alright
                //https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility

                createInfo.renderPass = (VkRenderPass)renderProgram->handle;

                vkCreateFramebuffer(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &createInfo, NULL, (VkFramebuffer*)&this->renderTargetHandle);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }
        this->constructed = true;
    }
    RenderTarget::RenderTarget(Texture2D thisBackendTexture, Texture2D thisDepthBuffer, bool isBackbuffer)
    {
        this->backendTexture = thisBackendTexture;
        this->depthBuffer = thisDepthBuffer;
        this->width = thisBackendTexture.width;
        this->height = thisBackendTexture.height;
        this->constructed = false;
        this->hasBeenUsedBefore = false;
        this->isBackbuffer = isBackbuffer;
    }
    RenderTarget::RenderTarget(u32 width, u32 height, ImageFormat imageFormat, ImageFormat depthFormat)
    {
        this->width = width;
        this->height = height;
        u8 *bytes = (u8*)calloc(width * height, 4);
        this->backendTexture = CreateTextureFromData(bytes, width, height, imageFormat, SamplerGetPointClamp(), false);
        
        if (depthFormat != ImageFormat_DepthNone && depthFormat != ImageFormat_Undefined)
        {
            this->depthBuffer = CreateTextureFromData(bytes, width, height, depthFormat, SamplerGetPointClamp(), false);
        }
        else
        {
            this->depthBuffer = {};
        }
        free(bytes);
    }
}