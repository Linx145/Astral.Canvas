#include "Astral.Canvas/Graphics/RenderProgram.h"
#include "Graphics/RenderProgram.hpp"

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
exportC void AstralCanvasRenderProgram_Construct(AstralCanvasRenderProgram ptr)
{
    ((AstralCanvas::RenderProgram *)ptr)->Construct();
}
exportC void AstralCanvasRenderProgram_Deinit(AstralCanvasRenderProgram ptr)
{
    ((AstralCanvas::RenderProgram *)ptr)->deinit();
}
exportC void AstralCanvasRenderProgramAttachment_AddInput(AstralCanvasRenderPass ptr, i32 inputIndex)
{
    ((AstralCanvas::RenderPass *)ptr)->AddInput(inputIndex);
}