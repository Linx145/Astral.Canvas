#pragma once
#include "Linxc.h"
#include "Graphics/SamplerState.hpp"
#include "Graphics/MemoryAllocation.hpp"

namespace AstralCanvas
{
    struct Texture2D
    {
        /// The raw pixel data of the image as loaded from the file. NULL if does not own image, or store data is false
        u8 *bytes;
        /// The width of the texture in pixels
        u32 width;
        /// The height of the texture in pixels
        u32 height;
        /// Whether the texture has been deinit'd
        bool isDisposed;
        /// Whether the texture has been constructed from it's byte data
        bool constructed;
        /// Whether the texture should keep it's raw pixel data as read from the file even after it has been copied over to VRAM
        bool storeData;
        /// How many copies of the image in decreasing resolution should be created
        u32 mipLevels;
        /// How much data comprises a pixel in the image, and what the data stands for
        ImageFormat imageFormat;
        /// The behaviour of shaders when sampling this texture. Included here as a reference for convenience
        SamplerState *samplerState;
        /// Whether the texture owns the handle. 
        /// This is false for when creating aliases to other textures, 
        /// or to a texture owned by a swapchain. When false, deinit() will not dispose the image handle
        bool ownsHandle;
        /// Whether the image is the backbuffer for a render target object. Will dictate what the image GPU layout will be
        bool usedForRenderTarget;
        /// The handle of the texture in the graphics API. Corresponds to a texture pointer type in the current graphics backend
        void *imageHandle;
        /// The view into the image in the graphics API.
        void *imageView;
        /// The allocated VRAM for the image
        AstralCanvas::MemoryAllocation allocatedMemory;

        void deinit();
        void Construct();
    };
    Texture2D CreateTextureFromHandle(void *handle, u32 width, u32 height, ImageFormat imageFormat, bool usedForRenderTarget);
    Texture2D CreateTextureFromData(u8* data, u32 width, u32 height, ImageFormat imageFormat, SamplerState *samplerState, bool usedForRenderTarget);
}