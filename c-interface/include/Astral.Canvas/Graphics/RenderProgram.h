#pragma once
#include "Astral.Canvas/Graphics/Enums.h"

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct 
    {
        /// The format of the image that the render stage(s) with this associated attachment should take
        AstralCanvas_ImageFormat imageFormat;
        /// Whether the attached image should be cleared when beginning the pass. If transparent, no clearing is performed
        bool clearColor;
        /// Whether the attached depth buffer should be cleared when beginning the pass. Not applicable if depthAttachmentIndex is -1
        bool clearDepth;
        /// Which renderprogram this pass belongs to
        void *belongsToProgram;

        AstralCanvas_RenderPassOutputType outputType;
    } AstralCanvasRenderProgramImageAttachment;

    typedef void *AstralCanvasRenderProgram;

    DynamicFunction AstralCanvasRenderProgram AstralCanvasRenderProgram_Init();
    DynamicFunction i32 AstralCanvasRenderProgram_AddAttachment(AstralCanvasRenderProgram ptr, AstralCanvas_ImageFormat imageFormat, bool clearColor, bool clearDepth, AstralCanvas_RenderPassOutputType outputType);
    DynamicFunction void AstralCanvasRenderProgram_AddRenderPass(AstralCanvasRenderProgram ptr, i32 colorAttachmentID, i32 depthAttachmentID);
    DynamicFunction void AstralCanvasRenderProgram_AddRenderPasses(AstralCanvasRenderProgram ptr, i32 *colorAttachmentIDs, usize colorAttachmentIDsCount, i32 depthAttachmentID);
    DynamicFunction void AstralCanvasRenderProgram_Construct(AstralCanvasRenderProgram ptr);
    DynamicFunction void AstralCanvasRenderProgram_Deinit(AstralCanvasRenderProgram ptr);
    //call begin and nextpass through graphics

#ifdef __cplusplus
}
#endif