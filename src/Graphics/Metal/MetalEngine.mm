#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalEngine.h"
#include "Graphics/Metal/MetalInstanceData.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

bool AstralCanvasMetal_Initialize(IAllocator *allocator, AstralCanvas::Window* window)
{
    id<MTLDevice> gpu = MTLCreateSystemDefaultDevice();
    AstralCanvasMetal_SetCurrentGPU(gpu);
    
    id<MTLCommandQueue> commandQueue = [gpu newCommandQueue];
    AstralCanvasMetal_SetMainCmdQueue(commandQueue);
    
    CAMetalLayer *swapchain = [CAMetalLayer layer];
    swapchain.device = AstralCanvasMetal_GetCurrentGPU();
    swapchain.opaque = YES;
    AstralCanvasMetal_SetSwapchain(swapchain);

    NSWindow *nswindow = glfwGetCocoaWindow(window->handle);
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;
    
    return true;
}
void AstralCanvasMetal_BeginDraw()
{
    CAMetalLayer *layer = AstralCanvasMetal_GetSwapchain();
    
    id<CAMetalDrawable> currentSwapchainSurface = [layer nextDrawable];
    //id<MTLTexture> currentSwapchainTexture = currentSwapchainSurface.texture;
    
    AstralCanvasMetal_SetCurrentSwapchainTexture(currentSwapchainSurface);

    id<MTLCommandQueue> mainCmdQueue = AstralCanvasMetal_GetMainCmdQueue();
    id<MTLCommandBuffer> mainCmdBuffer = [mainCmdQueue commandBuffer];

    AstralCanvasMetal_SetMainCmdBuffer(mainCmdBuffer);
}
void AstralCanvasMetal_EndDraw()
{
    id<MTLCommandBuffer> mainCmdBuffer = AstralCanvasMetal_GetMainCmdBuffer();
    id<CAMetalDrawable> surface = AstralCanvasMetal_GetCurrentSwapchainTexture();
    
    [mainCmdBuffer presentDrawable:surface];
    [mainCmdBuffer commit];
    [mainCmdBuffer release];
    [surface release];
}
#endif
