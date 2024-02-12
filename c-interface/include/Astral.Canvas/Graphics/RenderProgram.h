#pragma once
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef void *AstralCanvasRenderProgram;
    typedef void *AstralCanvasRenderPass;

    DynamicFunction AstralCanvasRenderProgram AstralCanvasRenderProgram_Init();
    DynamicFunction i32 AstralCanvasRenderProgram_AddAttachment(AstralCanvasRenderProgram ptr, AstralCanvas_ImageFormat imageFormat, bool clearColor, bool clearDepth, AstralCanvas_RenderPassOutputType outputType);
    DynamicFunction AstralCanvasRenderPass AstralCanvasRenderProgram_AddRenderPass(AstralCanvasRenderProgram ptr, i32 colorAttachmentID, i32 depthAttachmentID);
    DynamicFunction AstralCanvasRenderPass AstralCanvasRenderProgram_AddRenderPasses(AstralCanvasRenderProgram ptr, i32 *colorAttachmentIDs, usize colorAttachmentIDsCount, i32 depthAttachmentID);
    DynamicFunction AstralCanvasRenderPass AstralCanvasRenderProgram_GetRenderPass(AstralCanvasRenderProgram ptr, usize index);
    DynamicFunction void AstralCanvasRenderProgram_Construct(AstralCanvasRenderProgram ptr);
    DynamicFunction void AstralCanvasRenderProgram_Deinit(AstralCanvasRenderProgram ptr);
    DynamicFunction void AstralCanvasRenderPass_AddInput(AstralCanvasRenderPass ptr, i32 inputIndex);
    DynamicFunction void AstralCanvasRenderPass_GetColorAttachments(AstralCanvasRenderPass ptr, i32 *attachmentIndices, usize* numAttachments);
    DynamicFunction i32 AstralCanvasRenderPass_GetDepthAttachments(AstralCanvasRenderPass ptr);

#ifdef __cplusplus
}
#endif