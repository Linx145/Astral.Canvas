#pragma once
#include "Linxc.h"
#include "vector.hpp"
#include "Graphics/Enums.hpp"
#include "Color.hpp"

namespace AstralCanvas
{
    struct RenderProgramImageAttachment
    {
        /// The format of the image that the render stage(s) with this associated attachment should take
        ImageFormat imageFormat;
        /// Whether the attached image should be cleared when beginning the pass. If transparent, no clearing is performed
        Color clearColor;
        /// Whether the attached depth buffer should be cleared when beginning the pass. Not applicable if depthAttachmentIndex is -1
        bool clearDepth;
        /// Which renderprogram this pass belongs to
        void *belongsToProgram;
    };
    struct RenderPass
    {
        /// The index in the array of the pass' color attachment
        collections::Array<i32> colorAttachmentIndices;
        /// The index in the array of the pass' depth attachment. -1 if not applicable
        i32 depthAttachmentIndex;
        /// Which renderprogram this pass belongs to
        void *belongsToProgram;
    };
    struct RenderProgram
    { //note: corresponds to a VkRenderpass in Vulkan
        IAllocator *allocator;
        void *handle;
        collections::vector<RenderProgramImageAttachment> attachments;
        collections::vector<RenderPass> renderPasses;

        RenderProgram(IAllocator *allocator);

        /// Adds an attachment to the render program. An attachment refers to the state of the textures passing in and out of each renderpass
        i32 AddAttachment(ImageFormat imageFormat, Color clearColor, bool clearDepth);
        /// Adds a render pass to the render program. A render pass is a stage within the 
        /// program, defining how it can read the previous stage (if any)'s texture and how 
        /// it will output to the next stage, or to the result rendertarget.
        void AddRenderPass(IAllocator *allocator, i32 colorAttachmentID, i32 depthAttachmentID);
        void AddRenderPass(collections::Array<i32> colorAttachmentIDs, i32 depthAttachmentID);
        void Construct();
        void deinit();
        /// Begins the render program execution. Remember to call End()!
        void Begin();
        /// Advances to the next render pass within the program. Therefore, all previously 
        /// recorded commands would have been in the previous renderpass, and all thereafter 
        /// will be in the aforementioned next pass.
        void NextRenderPass();
    };
}