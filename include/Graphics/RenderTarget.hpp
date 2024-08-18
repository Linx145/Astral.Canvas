#pragma once
#include "Graphics/Texture2D.hpp"
#include "Graphics/RenderProgram.hpp"
#include "array.hpp"

namespace AstralCanvas
{
    struct RenderTarget
    {
        IAllocator allocator;
        /// All textures that should be rendered to when using this render target. Normally
        /// consists of a color texture and depth buffer, but you may need to attach
        /// more in the case of more complex render programs.
        collections::Array<Texture2D *> textures;
        /// The width of the target. Equivalent to backendTexture.width
        u32 width;
        /// The height of the target. Equivalent to backendTexture.height
        u32 height;
        /// Whether the Render Target is the image that the window renders with
        bool isBackbuffer;
        /// A pointer to the handle of the rendertarget's backend-specific implementation
        void *renderTargetHandle;
        /// Used by Astral.Canvas to track whether the render target has been used before. 
        /// If set as a texture in a uniform but has not been used prior, an error will be raised.
        bool hasBeenUsedBefore;

        bool isDisposed;
        bool constructed;

        RenderTarget(IAllocator allocator, u32 width, u32 height, collections::Array<Texture2D *> texturesToUse);
        RenderTarget(IAllocator allocator, Texture2D *thisBackendTexture, Texture2D *thisDepthBuffer, bool isBackbuffer);
        RenderTarget(IAllocator allocator, u32 width, u32 height, ImageFormat imageFormat, ImageFormat depthFormat);
        void deinit();
        void Construct(AstralCanvas::RenderProgram *renderProgram);
    };
}