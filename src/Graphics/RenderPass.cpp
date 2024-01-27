#include "Graphics/RenderPass.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#endif

namespace AstralCanvas
{
    RenderProgram::RenderProgram(IAllocator *allocator)
    {
        this->attachments = collections::vector<RenderProgramImageAttachment>(allocator);
        this->renderPasses = collections::vector<RenderPass>(allocator);
    }
    i32 RenderProgram::AddAttachment(ImageFormat imageFormat, Color clearColor, bool clearDepth)
    {
        i32 result = (i32)this->attachments.count;

        this->attachments.Add(
            {
                imageFormat,
                clearColor,
                clearDepth,
                this
            }
        );

        return result;
    }
    void RenderProgram::AddRenderPass(collections::Array<i32> colorAttachmentIDs, i32 depthAttachmentID)
    {
        this->renderPasses.Add(
            {
                colorAttachmentIDs,
                depthAttachmentID,
                this
            }
        );
    }
    void RenderProgram::AddRenderPass(IAllocator *allocator, i32 colorAttachmentID, i32 depthAttachmentID)
    {
        collections::Array<i32> colorAttachmentIDs = collections::Array<i32>(allocator, 1);
        colorAttachmentIDs.data[0] = colorAttachmentID;

        this->renderPasses.Add(
            {
                colorAttachmentIDs,
                depthAttachmentID,
                this
            }
        );
    }
    void RenderProgram::Construct()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralCanvasVk_CreateRenderProgram(AstralCanvasVk_GetCurrentGPU(), this);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }
    }
    void RenderProgram::deinit()
    {

    }
}