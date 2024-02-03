#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalInstanceData.h"

id<MTLDevice> AstralCanvasMetal_CurrentGPU;
id<MTLCommandQueue> AstralCanvasMetal_MainCmdQueue;
id<MTLCommandBuffer> AstralCanvasMetal_MainCmdBuffer;
CAMetalLayer *AstralCanvasMetal_Swapchain;
id<CAMetalDrawable> AstralCanvasMetal_CurrentSwapchainTexture;

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
#endif
