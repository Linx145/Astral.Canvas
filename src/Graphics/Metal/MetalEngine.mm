#ifdef ASTRALCANVAS_METAL
#include "Graphics/Metal/MetalEngine.h"
#include "Graphics/Metal/MetalInstanceData.h"
#include "GLFW/glfw3.h"

#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

dispatch_semaphore_t semaphore;

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

    NSWindow *nswindow = glfwGetCocoaWindow((GLFWwindow*)window->handle);
    nswindow.contentView.layer = swapchain;
    nswindow.contentView.wantsLayer = YES;
    
    semaphore = dispatch_semaphore_create(1);

    AstralCanvasMetal_InitDepthStates();
    AstralCanvasMetal_RecreateDepthBuffers(window->resolution.X, window->resolution.Y);
    return true;
}
void AstralCanvasMetal_BeginDraw()
{
    dispatch_semaphore_wait(semaphore, DISPATCH_TIME_FOREVER);
    
    id<CAMetalDrawable> currentSwapchainSurface = AstralCanvasMetal_NextDrawable();
    
    AstralCanvasMetal_SetCurrentSwapchainTexture(currentSwapchainSurface);

    id<MTLCommandQueue> mainCmdQueue = AstralCanvasMetal_GetMainCmdQueue();
    id<MTLCommandBuffer> mainCmdBuffer = [mainCmdQueue commandBuffer];

    AstralCanvasMetal_SetMainCmdBuffer(mainCmdBuffer);
}
void AstralCanvasMetal_EndDraw()
{
    id<MTLCommandBuffer> mainCmdBuffer = AstralCanvasMetal_GetMainCmdBuffer();
    id<CAMetalDrawable> surface = AstralCanvasMetal_GetCurrentSwapchainTexture();
    
    __block dispatch_semaphore_t block_semaphore = semaphore;
    [mainCmdBuffer addCompletedHandler:^(id<MTLCommandBuffer> buffer)
     {
         dispatch_semaphore_signal(block_semaphore);
     }];
    
    [mainCmdBuffer presentDrawable:surface];
    [mainCmdBuffer commit];
    [mainCmdBuffer release];
    [surface release];
}
#endif
