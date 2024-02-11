#include "Astral.Canvas/Graphics/RenderProgram.h"
#include "Graphics/RenderProgram.hpp"
#include "string.h"

exportC AstralCanvasRenderProgram AstralCanvasRenderProgram_Init()
{
    AstralCanvas::RenderProgram *result = (AstralCanvas::RenderProgram *)malloc(sizeof(AstralCanvas::RenderProgram));
    *result = AstralCanvas::RenderProgram(GetDefaultAllocator());
    return result;
}
exportC i32 AstralCanvasRenderProgram_AddAttachment(AstralCanvasRenderProgram ptr, AstralCanvas_ImageFormat imageFormat, bool clearColor, bool clearDepth, AstralCanvas_RenderPassOutputType outputType)
{
    return ((AstralCanvas::RenderProgram *)ptr)->AddAttachment((AstralCanvas::ImageFormat)imageFormat, clearColor, clearDepth, (AstralCanvas::RenderPassOutputType)outputType);
}
exportC AstralCanvasRenderPass AstralCanvasRenderProgram_AddRenderPass(AstralCanvasRenderProgram ptr, i32 colorAttachmentID, i32 depthAttachmentID)
{
    return ((AstralCanvas::RenderProgram *)ptr)->AddRenderPass(colorAttachmentID, depthAttachmentID);
}
exportC AstralCanvasRenderPass AstralCanvasRenderProgram_AddRenderPasses(AstralCanvasRenderProgram ptr, i32 *colorAttachmentIDs, usize colorAttachmentIDsCount, i32 depthAttachmentID)
{
    collections::Array<i32> array = collections::Array<i32>(((AstralCanvas::RenderProgram *)ptr)->allocator, colorAttachmentIDsCount);
    return ((AstralCanvas::RenderProgram *)ptr)->AddRenderPass(array, depthAttachmentID);
}
exportC AstralCanvasRenderPass AstralCanvasRenderProgram_GetRenderPass(AstralCanvasRenderProgram ptr, usize index)
{
    return ((AstralCanvas::RenderProgram *)ptr)->renderPasses.Get(index);
}
exportC void AstralCanvasRenderProgram_Construct(AstralCanvasRenderProgram ptr)
{
    ((AstralCanvas::RenderProgram *)ptr)->Construct();
}
exportC void AstralCanvasRenderProgram_Deinit(AstralCanvasRenderProgram ptr)
{
    ((AstralCanvas::RenderProgram *)ptr)->deinit();
}
exportC void AstralCanvasRenderPass_AddInput(AstralCanvasRenderPass ptr, i32 inputIndex)
{
    ((AstralCanvas::RenderPass *)ptr)->AddInput(inputIndex);
}
exportC void AstralCanvasRenderPass_GetColorAttachments(AstralCanvasRenderPass ptr, i32 *attachmentIndices, usize* numAttachments)
{
    if (numAttachments != NULL)
    {
        *numAttachments = ((AstralCanvas::RenderPass *)ptr)->colorAttachmentIndices.length;
    }
    if (attachmentIndices != NULL)
    {
        memcpy(attachmentIndices, ((AstralCanvas::RenderPass *)ptr)->colorAttachmentIndices.data, sizeof(i32) * ((AstralCanvas::RenderPass *)ptr)->colorAttachmentIndices.length);
    }
}
exportC i32 AstralCanvasRenderPass_GetDepthAttachments(AstralCanvasRenderPass ptr)
{
    return ((AstralCanvas::RenderPass *)ptr)->depthAttachmentIndex;
}