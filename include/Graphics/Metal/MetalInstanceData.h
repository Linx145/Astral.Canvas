#pragma once                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
#ifdef ASTRALCANVAS_METAL
#import <Metal/Metal.h>
#import <QuartzCore/CAMetalLayer.h>

id<MTLDevice> AstralCanvasMetal_GetCurrentGPU();
void AstralCanvasMetal_SetCurrentGPU(id<MTLDevice> device);

id<MTLCommandQueue> AstralCanvasMetal_GetMainCmdQueue();
void AstralCanvasMetal_SetMainCmdQueue(id<MTLCommandQueue> queue);

CAMetalLayer *AstralCanvasMetal_GetSwapchain();
void AstralCanvasMetal_SetSwapchain(CAMetalLayer *swapchain);

id<CAMetalDrawable> AstralCanvasMetal_GetCurrentSwapchainTexture();
void AstralCanvasMetal_SetCurrentSwapchainTexture(id<CAMetalDrawable> texture);

id<MTLCommandBuffer> AstralCanvasMetal_GetMainCmdBuffer();
void AstralCanvasMetal_SetMainCmdBuffer(id<MTLCommandBuffer> buffer);
#endif
