#pragma once
#include "Texture2D.hpp"

struct RenderTarget
{
    /// The backend texture of the render target. All render targets have a texture that represents the rendered results itself
    Texture2D backendTexture;
    /// The depth buffer that stores draw depth (Z axis position) information when rendering to this render target
    Texture2D depthBuffer;
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
};

void AstralCanvas_DestroyRenderTarget(RenderTarget *renderTarget);
RenderTarget AstralCanvas_CreateRenderTargetFrom(Texture2D thisBackendTexture, Texture2D thisDepthBuffer);
RenderTarget AstralCanvas_CreateRenderTargetFromWH(u32 width, u32 height, ImageFormat imageFormat, ImageFormat depthFormat);