#ifdef ASTRALCANVAS_METAL
#include "Linxc.h"
#include "Graphics/Metal/MetalInstanceData.h"
#include "Graphics/Texture2D.hpp"
#include "array.hpp"

id<MTLDevice> AstralCanvasMetal_CurrentGPU;
id<MTLCommandQueue> AstralCanvasMetal_MainCmdQueue;
id<MTLCommandBuffer> AstralCanvasMetal_MainCmdBuffer;
CAMetalLayer *AstralCanvasMetal_Swapchain;
id<CAMetalDrawable> AstralCanvasMetal_CurrentSwapchainTexture;
id<MTLDepthStencilState> AstralCanvasMetal_DepthTestDisabledWriteDisabled;
id<MTLDepthStencilState> AstralCanvasMetal_DepthTestEnabledWriteDisabled;
id<MTLDepthStencilState> AstralCanvasMetal_DepthTestEnabledWriteEnabled;
id<MTLDepthStencilState> AstralCanvasMetal_DepthTestDisabledWriteEnabled;
collections::Array<AstralCanvas::Texture2D> AstralCanvasMetal_DepthBuffers = collections::Array<AstralCanvas::Texture2D>();
i32 AstralCanvasMetal_SwapchainCurrentFrame = 0;

id<MTLDevice> AstralCanvasMetal_GetCurrentGPU()
{
    return AstralCanvasMetal_CurrentGPU;
}
void AstralCanvasMetal_SetCurrentGPU(id<MTLDevice> device)
{
    AstralCanvasMetal_CurrentGPU = device;
}

id<MTLCommandQueue> AstralCanvasMetal_GetMainCmdQueue()
{
    return AstralCanvasMetal_MainCmdQueue;
}
void AstralCanvasMetal_SetMainCmdQueue(id<MTLCommandQueue> queue)
{
    AstralCanvasMetal_MainCmdQueue = queue;
}

CAMetalLayer *AstralCanvasMetal_GetSwapchain()
{
    return AstralCanvasMetal_Swapchain;
}
void AstralCanvasMetal_SetSwapchain(CAMetalLayer *swapchain)
{
    AstralCanvasMetal_Swapchain = swapchain;
}

id<CAMetalDrawable> AstralCanvasMetal_GetCurrentSwapchainTexture()
{
    return AstralCanvasMetal_CurrentSwapchainTexture;
}
void AstralCanvasMetal_SetCurrentSwapchainTexture(id<CAMetalDrawable> texture)
{
    AstralCanvasMetal_CurrentSwapchainTexture = texture;
}

id<MTLCommandBuffer> AstralCanvasMetal_GetMainCmdBuffer()
{
    return AstralCanvasMetal_MainCmdBuffer;
}
void AstralCanvasMetal_SetMainCmdBuffer(id<MTLCommandBuffer> buffer)
{
    AstralCanvasMetal_MainCmdBuffer = buffer;
}

id<MTLDepthStencilState> AstralCanvasMetal_GetDepthTestDisabledWriteDisabled()
{
    return AstralCanvasMetal_DepthTestDisabledWriteDisabled;
}
id<MTLDepthStencilState> AstralCanvasMetal_GetDepthTestEnabledWriteDisabled()
{
    return AstralCanvasMetal_DepthTestEnabledWriteDisabled;
}
id<MTLDepthStencilState> AstralCanvasMetal_GetDepthTestEnabledWriteEnabled()
{
    return AstralCanvasMetal_DepthTestEnabledWriteEnabled;
}
id<MTLDepthStencilState> AstralCanvasMetal_GetDepthTestDisabledWriteEnabled()
{
    return AstralCanvasMetal_DepthTestDisabledWriteEnabled;
}
void AstralCanvasMetal_InitDepthStates()
{
    MTLDepthStencilDescriptor *descriptor = [MTLDepthStencilDescriptor new];
    
    //test disabled, write disabled
    descriptor.depthCompareFunction = MTLCompareFunctionAlways;
    descriptor.depthWriteEnabled = false;
    AstralCanvasMetal_DepthTestDisabledWriteDisabled = [AstralCanvasMetal_CurrentGPU newDepthStencilStateWithDescriptor:descriptor];
    
    //test enabled, write disabled
    descriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
    descriptor.depthWriteEnabled = false;
    AstralCanvasMetal_DepthTestEnabledWriteDisabled = [AstralCanvasMetal_CurrentGPU newDepthStencilStateWithDescriptor:descriptor];
    
    //test enabled, write enabled
    descriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
    descriptor.depthWriteEnabled = true;
    AstralCanvasMetal_DepthTestEnabledWriteEnabled = [AstralCanvasMetal_CurrentGPU newDepthStencilStateWithDescriptor:descriptor];
    
    //test disabled, write enabled
    descriptor.depthCompareFunction = MTLCompareFunctionAlways;
    descriptor.depthWriteEnabled = true;
    AstralCanvasMetal_DepthTestDisabledWriteEnabled = [AstralCanvasMetal_CurrentGPU newDepthStencilStateWithDescriptor:descriptor];
    
    [descriptor release];
}
void AstralCanvasMetal_RecreateDepthBuffers(usize frameWidth, usize frameHeight)
{
    u64 framesCount = AstralCanvasMetal_Swapchain.maximumDrawableCount;
    
    if (AstralCanvasMetal_DepthBuffers.data == NULL)
    {
        AstralCanvasMetal_DepthBuffers = collections::Array<AstralCanvas::Texture2D>(GetDefaultAllocator(), framesCount);
    }
    else
    {
        for (usize i = 0; i < AstralCanvasMetal_DepthBuffers.length; i++)
        {
            AstralCanvasMetal_DepthBuffers.data[i].deinit();
        }
    }
    for (usize i = 0; i < framesCount; i++)
    {
        AstralCanvasMetal_DepthBuffers.data[i] = AstralCanvas::CreateTextureFromData(NULL, frameWidth, frameHeight, AstralCanvas::ImageFormat_Depth32, AstralCanvas::SamplerGetPointClamp(), true);
    }
}
id<CAMetalDrawable> AstralCanvasMetal_NextDrawable()
{
    id<CAMetalDrawable> result = [AstralCanvasMetal_Swapchain nextDrawable];
    AstralCanvasMetal_SwapchainCurrentFrame += 1;
    if (AstralCanvasMetal_SwapchainCurrentFrame >= AstralCanvasMetal_Swapchain.maximumDrawableCount)
    {
        AstralCanvasMetal_SwapchainCurrentFrame = 0;
    }
    return result;
}
void *AstralCanvasMetal_GetDepthBackbuffer()
{
    return &AstralCanvasMetal_DepthBuffers.data[AstralCanvasMetal_SwapchainCurrentFrame];
}
#endif
