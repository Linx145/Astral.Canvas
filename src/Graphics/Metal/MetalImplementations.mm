#ifdef MACOS
#import "Graphics/Metal/MetalImplementations.h"
#import "Metal/Metal.h"
#import "Graphics/Metal/MetalInstanceData.h"
#import "Foundation/Foundation.h"
#include "Linxc.h"
#include "ErrorHandling.hpp"
#include "Graphics/Metal/MetalEnumConverters.h"

id<MTLBuffer> currentIndexBuffer = nil;

AstralCanvas::ImageFormat AstralCanvasMetal_GetSwapchainFormat()
{
    return AstralCanvasMetal_ToImageFormat(AstralCanvasMetal_GetSwapchain().pixelFormat);
}
bool AstralCanvasMetal_CreateRenderProgram(AstralCanvas::RenderProgram *program)
{
    usize size = sizeof(void*) * program->renderPasses.count;
    program->handle = program->allocator->Allocate(size);

    if (program->handle == NULL)
    {
        return false;
    }
    
    for (usize i = 0; i < program->renderPasses.count; i++)
    {
        AstralCanvas::RenderPass *rp = program->renderPasses.Get(i);
        MTLRenderPassDescriptor *descriptor = [MTLRenderPassDescriptor new];
        
        for (usize j = 0; j < rp->colorAttachmentIndices.length; j++)
        {
            AstralCanvas::RenderProgramImageAttachment *attachment = program->attachments.Get(rp->colorAttachmentIndices.data[j]);
            
            descriptor.colorAttachments[j].loadAction = attachment->clearColor ? MTLLoadActionClear : MTLLoadActionLoad;
            descriptor.colorAttachments[j].storeAction = MTLStoreActionStore;
            descriptor.colorAttachments[j].clearColor = MTLClearColorMake(0.0, 0.0, 0.0, 1.0);
            //descriptor.colorAttachments[0].texture = to resolve at runtime
        }
        if (rp->depthAttachmentIndex != -1)
        {
            descriptor.depthAttachment.clearDepth = program->attachments.Get(rp->depthAttachmentIndex)->clearDepth ? 0.0 : 1.0;
            descriptor.depthAttachment.loadAction = program->attachments.Get(rp->depthAttachmentIndex)->clearDepth ? MTLLoadActionClear : MTLLoadActionLoad;
            descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        }
        ((MTLRenderPassDescriptor**)program->handle)[i] = descriptor;
    }
    
    return true;
}
void* AstralCanvasMetal_StartRenderProgram(AstralCanvas::RenderProgram *program, AstralCanvas::RenderTarget *target, u32 currentRenderPass, AstralCanvas::Color color)
{
    if (program->handle != NULL)
    {
        id<CAMetalDrawable> surface = nil;
        if (target == NULL)
        {
            surface = AstralCanvasMetal_GetCurrentSwapchainTexture();
        }
        else
        {
            surface = (id<CAMetalDrawable>)target->renderTargetHandle;
        }
        MTLRenderPassDescriptor** array = (MTLRenderPassDescriptor**)program->handle;
        MTLRenderPassDescriptor* currentPass = array[currentRenderPass]; //todo
        currentPass.colorAttachments[0].texture = surface.texture;
        currentPass.colorAttachments[0].clearColor = MTLClearColorMake(color.R * ONE_OVER_255, color.G * ONE_OVER_255, color.B * ONE_OVER_255, color.A * ONE_OVER_255);
        
        id<MTLCommandBuffer> mainCmdBuffer = AstralCanvasMetal_GetMainCmdBuffer();
        id<MTLRenderCommandEncoder> encoder = [mainCmdBuffer renderCommandEncoderWithDescriptor:currentPass];
        
        return encoder;
    }
    return NULL;
}
void AstralCanvasMetal_EndRenderProgram(void* encoder)
{
    id<MTLCommandEncoder> encoderID = (id<MTLCommandEncoder>)encoder;
    [encoderID endEncoding];
}
void AstralCanvasMetal_DestroyRenderProgram(AstralCanvas::RenderProgram *program)
{
    if (program->handle != NULL)
    {
        usize count = program->renderPasses.count;
        for (usize i = 0; i < count; i++)
        {
            MTLRenderPassDescriptor *descriptor = (MTLRenderPassDescriptor *)((MTLRenderPassDescriptor**)program->handle)[i];
            [descriptor release];
        }
        program->allocator->Free((void**)&program->handle);
    }
}

void *AstralCanvasMetal_CreateRenderPipeline(AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPass)
{
    MTLRenderPipelineDescriptor *pipelineDescriptor = [MTLRenderPipelineDescriptor new];
    pipelineDescriptor.vertexFunction = (id<MTLFunction>)pipeline->shader->shaderModule1;
    pipelineDescriptor.fragmentFunction = (id<MTLFunction>)pipeline->shader->shaderModule2;

    AstralCanvas::RenderPass *pass = &renderProgram->renderPasses.ptr[renderPass];

    pipelineDescriptor.colorAttachments[0].pixelFormat = AstralCanvasMetal_FromImageFormat(renderProgram->attachments.ptr[pass->colorAttachmentIndices.data[0]].imageFormat);
    
    if (pass->depthAttachmentIndex != -1)
    {
        pipelineDescriptor.depthAttachmentPixelFormat = AstralCanvasMetal_FromImageFormat(renderProgram->attachments.ptr[pass->depthAttachmentIndex].imageFormat);
    }
    else pipelineDescriptor.depthAttachmentPixelFormat = MTLPixelFormatInvalid;
    
    usize attribCount = 0;
    for (usize i = 0; i < pipeline->vertexDeclarations.length; i++)
    {
        for (usize j = 0; j < pipeline->vertexDeclarations.data[i]->elements.count; j++)
        {
            pipelineDescriptor.vertexDescriptor.attributes[attribCount].offset = pipeline->vertexDeclarations.data[i]->elements.ptr[j].offset;
            pipelineDescriptor.vertexDescriptor.attributes[attribCount].format = AstralCanvasMetal_FromVertexElementFormat(pipeline->vertexDeclarations.data[i]->elements.ptr[j].format);
            pipelineDescriptor.vertexDescriptor.attributes[attribCount].bufferIndex = i;
            attribCount += 1;
        }
        
        pipelineDescriptor.vertexDescriptor.layouts[i].stride = pipeline->vertexDeclarations.data[i]->size;
        if (pipeline->vertexDeclarations.data[i]->inputRate == AstralCanvas::VertexInput_PerVertex)
        {
            pipelineDescriptor.vertexDescriptor.layouts[i].stepFunction = MTLVertexStepFunctionPerVertex;
        }
        else pipelineDescriptor.vertexDescriptor.layouts[i].stepFunction = MTLVertexStepFunctionPerInstance;
    }
    
    pipelineDescriptor.inputPrimitiveTopology = MTLPrimitiveTopologyClassTriangle;
    pipelineDescriptor.rasterizationEnabled = true;
    pipelineDescriptor.rasterSampleCount = 1;
    
    id<MTLDevice> device = AstralCanvasMetal_GetCurrentGPU();
    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:NULL];

    [pipelineDescriptor release];
    
    return pipelineState;
}
void AstralCanvasMetal_UseRenderPipeline(void *commandEncoder, AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPassToUse, Maths::Rectangle viewport, Maths::Rectangle clipArea)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    id<MTLRenderPipelineState> pipelineState = (id<MTLRenderPipelineState>)pipeline->GetOrCreateFor(renderProgram, renderPassToUse);

    [encoder setRenderPipelineState:pipelineState];

    /*MTLViewport metalViewport;
    metalViewport.width = viewport.Width;
    metalViewport.height = viewport.Height;
    metalViewport.originX = viewport.X;
    metalViewport.originY = viewport.Y;
    metalViewport.znear = 0.0;
    metalViewport.zfar = 1.0;
    [encoder setViewport:metalViewport];
    
    MTLScissorRect metalScissorRect;
    metalScissorRect.width = clipArea.Width;
    metalScissorRect.height = clipArea.Height;
    metalScissorRect.x = clipArea.X;
    metalScissorRect.y = clipArea.Y;
    
    [encoder setScissorRect:metalScissorRect];*/
}
void AstralCanvasMetal_DestroyRenderPipeline(void *pipeline)
{
    id<MTLRenderPipelineState> state = (id<MTLRenderPipelineState>)pipeline;
    [state release];
}

void AstralCanvasMetal_CreateVertexBuffer(AstralCanvas::VertexBuffer *vertexBuffer, void* verticesData, usize count)
{
    if (vertexBuffer->handle != NULL)
    {
        vertexBuffer->deinit();
    }
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    id<MTLBuffer> buffer = [gpu newBufferWithBytes:verticesData length:count options:MTLResourceOptionCPUCacheModeDefault];
    vertexBuffer->handle = buffer;
}
void AstralCanvasMetal_SetVertexBuffer(void *commandEncoder, const AstralCanvas::VertexBuffer *vertexBuffer, u32 bindingPoint)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    id<MTLBuffer> buffer = (id<MTLBuffer>)vertexBuffer->handle;
    
    [encoder setVertexBuffer:buffer offset:0 atIndex:bindingPoint];
}
void AstralCanvasMetal_DestroyVertexBuffer(AstralCanvas::VertexBuffer *vertexBuffer)
{
    if (vertexBuffer->handle != NULL)
    {
        id<MTLBuffer> buffer = (id<MTLBuffer>)vertexBuffer->handle;
        [buffer release];
    }
}

void AstralCanvasMetal_CreateIndexBuffer(AstralCanvas::IndexBuffer *indexBuffer, void* indicesData, usize count)
{
    if (indexBuffer->handle != NULL)
    {
        indexBuffer->deinit();
    }
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    id<MTLBuffer> buffer = [gpu newBufferWithBytes:indicesData length:count options:MTLResourceOptionCPUCacheModeDefault];
    indexBuffer->handle = buffer;
}
void AstralCanvasMetal_SetIndexBuffer(void *commandEncoder, const AstralCanvas::IndexBuffer *indexBuffer)
{
    id<MTLBuffer> buffer = (id<MTLBuffer>)indexBuffer->handle;
    
    currentIndexBuffer = buffer;
}
void AstralCanvasMetal_DestroyIndexBuffer(AstralCanvas::IndexBuffer *indexBuffer)
{
    if (indexBuffer->handle != NULL)
    {
        id<MTLBuffer> buffer = (id<MTLBuffer>)indexBuffer->handle;
        [buffer release];
    }
}

void AstralCanvasMetal_DestroyShaderProgram(void* vertex, void* fragment)
{
    if (vertex != NULL)
    {
        [(id<MTLFunction>)vertex release];
    }
    if (fragment != NULL)
    {
        [(id<MTLFunction>)fragment release];
    }
}
bool AstralCanvasMetal_CreateShaderProgram(string vertexSource, string fragmentSource, void** vertexOut, void** fragmentOut)
{
    IAllocator tempAllocator = GetCAllocator();
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    
    NSString *vertexNSString = [NSString stringWithUTF8String:vertexSource.buffer];
    NSString *fragmentNSString = [NSString stringWithUTF8String:fragmentSource.buffer];
    
    id<MTLFunction> vertexFunction = NULL;
    id<MTLFunction> fragmentFunction = NULL;
    
    id<MTLLibrary> vertexShader = [gpu newLibraryWithSource:vertexNSString options:NULL error:NULL];
    if (vertexShader != NULL)
    {
        vertexFunction = [vertexShader newFunctionWithName:@"main0"];
        [vertexShader release];
    }
    else 
    {
        [vertexNSString release];
        return false;
    }
    
    id<MTLLibrary> fragmentShader = [gpu newLibraryWithSource:fragmentNSString options:NULL error:NULL];
    if (fragmentShader != NULL)
    {
        fragmentFunction = [fragmentShader newFunctionWithName:@"main0"];
        [fragmentShader release];
    }
    else
    {
        [vertexNSString release];
        [fragmentNSString release];
        return false;
    }
    
    [vertexNSString release];
    [fragmentNSString release];
    
    *vertexOut = (void*)vertexFunction;
    *fragmentOut = (void*)fragmentFunction;
    return true;
}

void AstralCanvasMetal_DrawIndexedPrimitives(void *currentCommandEncoderInstance, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)currentCommandEncoderInstance;
    
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:indexCount indexType:MTLIndexTypeUInt16 indexBuffer:currentIndexBuffer indexBufferOffset:0 instanceCount:instanceCount baseVertex:vertexOffset baseInstance:firstInstance];
}
#endif
