#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalInstanceData.h"

id<MTLDevice> AstralCanvasMetal_CurrentGPU;
id<MTLCommandQueue> AstralCanvasMetal_MainCmdQueue;
CAMetalLayer *AstralCanvasMetal_Swapchain;

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
#endif
