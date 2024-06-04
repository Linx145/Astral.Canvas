#include "Graphics/RenderTarget.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#endif

#ifdef ASTRALCANVAS_OPENGL
#include "Graphics/Glad/glad.h"
#endif

namespace AstralCanvas
{
    void RenderTarget::deinit()
    {
        if (this->isDisposed || !this->constructed || this->textures.data == NULL)
        {
            return;
        }
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                if (this->renderTargetHandle != NULL)
                {
                    vkDestroyFramebuffer(AstralCanvasVk_GetCurrentGPU()->logicalDevice, (VkFramebuffer)this->renderTargetHandle, NULL);
                }
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                break;
            }
            #endif
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                u32 intHandle = (u32)this->renderTargetHandle;
                glDeleteFramebuffers(1, &intHandle);
            }
            break;
#endif
            default:
                THROW_ERR("Unimplemented backend: RenderTarget deinit");
                break;
        }
        for (usize i = 0; i < this->textures.length; i++)
        {
            this->textures.data[i].deinit();
        }
        this->textures.deinit();
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
                createInfo.attachmentCount = renderProgram->attachments.count;

                VkImageView *imageViews = (VkImageView*)malloc(createInfo.attachmentCount * sizeof(VkImageView));
                for (usize i = 0; i < createInfo.attachmentCount; i++)
                {
                    imageViews[i] = (VkImageView)this->textures.data[i].imageView;
                }
                createInfo.pAttachments = imageViews;

                //createInfo requires a renderpass to set what renderpass the framebuffer is
                //compatible with. Even if we pass in a different renderpass that is compatible
                //with the one it is created with, it will still be alright
                //https://registry.khronos.org/vulkan/specs/1.3-extensions/html/vkspec.html#renderpass-compatibility

                createInfo.renderPass = (VkRenderPass)renderProgram->handle;

                vkCreateFramebuffer(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &createInfo, NULL, (VkFramebuffer*)&this->renderTargetHandle);

                free(imageViews);
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                //metal doesnt require a dedicated framebuffer handle 
                //like Vulkan, so we don't have to do anything
                break;
            }
            #endif
            #ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                u32 intHandle;
                glGenFramebuffers(1, &intHandle);
                this->renderTargetHandle = (void*)intHandle;
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend: RenderTarget Construct");
                break;
        }
        this->constructed = true;
    }
    RenderTarget::RenderTarget(IAllocator allocator, u32 width, u32 height, collections::Array<Texture2D> texturesToUse)
    {
        this->allocator = allocator;
        this->textures = texturesToUse;
        this->width = width;
        this->height = height;
        this->constructed = false;
        this->isDisposed = false;
        this->hasBeenUsedBefore = false;
        this->isBackbuffer = false;
        this->renderTargetHandle = NULL;
    }
    RenderTarget::RenderTarget(IAllocator allocator, Texture2D thisBackendTexture, Texture2D thisDepthBuffer, bool isBackbuffer)
    {
        this->allocator = allocator;
        this->textures = collections::Array<Texture2D>(allocator, 2);
        this->textures.data[0] = thisBackendTexture;
        this->textures.data[1] = thisDepthBuffer;
        this->width = thisBackendTexture.width;
        this->height = thisBackendTexture.height;
        this->constructed = false;
        this->isDisposed = false;
        this->hasBeenUsedBefore = false;
        this->isBackbuffer = isBackbuffer;
        this->renderTargetHandle = NULL;
    }
    RenderTarget::RenderTarget(IAllocator allocator, u32 width, u32 height, ImageFormat imageFormat, ImageFormat depthFormat)
    {
        this->allocator = allocator;
        this->width = width;
        this->height = height;
        this->constructed = false;
        this->isDisposed = false;
        this->renderTargetHandle = NULL;
        u8 *bytes = (u8*)calloc(width * height, 4);
        usize textureLength = 1;
        Texture2D backendTexture = CreateTextureFromData(bytes, width, height, imageFormat, true, false);
        Texture2D depthBuffer{};

        if (depthFormat > ImageFormat_DepthNone)
        {
            if (depthFormat == ImageFormat_BackbufferFormat)
            {
                depthFormat = ImageFormat_Depth32;
            }
            textureLength++;
            depthBuffer = CreateTextureFromData(NULL, width, height, depthFormat, true, false);
        }
        free(bytes);

        this->textures = collections::Array<Texture2D>(allocator, textureLength);
        this->textures.data[0] = backendTexture;
        if (textureLength == 2)
            this->textures.data[1] = depthBuffer;
    }
}