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

#endif
