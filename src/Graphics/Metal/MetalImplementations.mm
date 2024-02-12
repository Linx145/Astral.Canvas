#ifdef ASTRALCANVAS_METAL
#import "Graphics/Metal/MetalImplementations.h"
#import "Metal/Metal.h"
#import "Graphics/Metal/MetalInstanceData.h"
#import "Foundation/Foundation.h"
#include "Linxc.h"
#include "ErrorHandling.hpp"
#include "Graphics/Metal/MetalEnumConverters.h"

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
        MTLRenderPassDescriptor** array = (MTLRenderPassDescriptor**)program->handle;
        MTLRenderPassDescriptor* currentPass = array[currentRenderPass];

        id<MTLTexture> depthBuffer = nil;
        if (target == NULL)
        {
            currentPass.colorAttachments[0].texture = (id<MTLTexture>)AstralCanvasMetal_GetCurrentSwapchainTexture().texture;
            currentPass.colorAttachments[0].clearColor = MTLClearColorMake(color.R * ONE_OVER_255, color.G * ONE_OVER_255, color.B * ONE_OVER_255, color.A * ONE_OVER_255);
            depthBuffer = (id<MTLTexture>)((AstralCanvas::Texture2D*)AstralCanvasMetal_GetDepthBackbuffer())->imageHandle;
        }
        else
        {
            for (usize i = 0; i < program->renderPasses.ptr[currentRenderPass].colorAttachmentIndices.length; i++)
            {
                currentPass.colorAttachments[i].texture = (id<MTLTexture>)target->textures.data[program->renderPasses.ptr[currentRenderPass].colorAttachmentIndices.data[i]].imageHandle;
                currentPass.colorAttachments[i].clearColor = MTLClearColorMake(color.R * ONE_OVER_255, color.G * ONE_OVER_255, color.B * ONE_OVER_255, color.A * ONE_OVER_255);
            }
            depthBuffer = (id<MTLTexture>)target->textures.data[program->renderPasses.ptr[currentRenderPass].depthAttachmentIndex].imageHandle;//(id<MTLTexture>)target->depthBuffer.imageHandle;
        }
        
        currentPass.depthAttachment.texture = depthBuffer;
        currentPass.depthAttachment.clearDepth = 1.0f;
        
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
    [encoderID release];
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
        program->allocator->Free(program->handle);
    }
}

void *AstralCanvasMetal_CreateRenderPipeline(AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPass)
{
    if (pipeline->shader == NULL || pipeline->shader->shaderModule1 == NULL || pipeline->shader->shaderModule2 == NULL)
    {
        if (pipeline->shader != NULL)
        {
            if (pipeline->shader->shaderModule1 == NULL)
            {
                printf("Pipeline missing first shader module\n");
            }
            if (pipeline->shader->shaderModule2 == NULL)
            {
                printf("Pipeline missing second shader module\n");
            }
        }
        THROW_ERR("Pipeline does not have a valid shader");
    }
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
    usize verticesLength = pipeline->vertexDeclarations.length;
    for (usize i = 0; i < verticesLength; i++)
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
    
    NSError *errors;
    id<MTLRenderPipelineState> pipelineState = [device newRenderPipelineStateWithDescriptor:pipelineDescriptor error:&errors];


    [pipelineDescriptor release];

    if (pipelineState == nil)
    {
        printf("Error: %s\n", [errors.localizedDescription cStringUsingEncoding:NSUTF8StringEncoding]);
        THROW_ERR("Failed to create pipeline state");
    }
    
    return pipelineState;
}
void AstralCanvasMetal_UseRenderPipeline(void *commandEncoder, AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPassToUse, Maths::Rectangle viewport, Maths::Rectangle clipArea)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    id<MTLRenderPipelineState> pipelineState = (id<MTLRenderPipelineState>)pipeline->GetOrCreateFor(renderProgram, renderPassToUse);

    [encoder setRenderPipelineState:pipelineState];
    if (pipeline->depthTest)
    {
        if (pipeline->depthWrite)
        {
            //test enabled, write enabled
            [encoder setDepthStencilState:AstralCanvasMetal_GetDepthTestEnabledWriteEnabled()];
        }
        else
        {
            //test enabled, write disabled
            [encoder setDepthStencilState:AstralCanvasMetal_GetDepthTestEnabledWriteDisabled()];
        }
    }
    else
    {
        if (pipeline->depthWrite)
        {
            //test disabled, write enabled
            [encoder setDepthStencilState:AstralCanvasMetal_GetDepthTestDisabledWriteEnabled()];
        }
        else
        {
            //test disabled, write disabled
            [encoder setDepthStencilState:AstralCanvasMetal_GetDepthTestDisabledWriteDisabled()];
        }
    }

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
    
    [encoder setFrontFacingWinding:MTLWindingClockwise];
    [encoder setCullMode:MTLCullModeNone];//AstralCanvasMetal_FromCullMode(pipeline->cullMode)];
}
void AstralCanvasMetal_DestroyRenderPipeline(void *pipeline)
{
    id<MTLRenderPipelineState> state = (id<MTLRenderPipelineState>)pipeline;
    [state release];
}

void AstralCanvasMetal_CreateVertexBuffer(AstralCanvas::VertexBuffer *vertexBuffer)
{
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    id<MTLBuffer> buffer = [gpu newBufferWithLength:(vertexBuffer->vertexCount * vertexBuffer->vertexType->size) options:MTLCPUCacheModeDefaultCache];
    vertexBuffer->handle = buffer;
}
void AstralCanvasMetal_CreateInstanceBuffer(AstralCanvas::InstanceBuffer *instanceBuffer)
{
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    id<MTLBuffer> buffer = [gpu newBufferWithLength:(instanceBuffer->instanceCount * instanceBuffer->instanceSize) options:MTLCPUCacheModeDefaultCache];
    instanceBuffer->handle = buffer;
}
void AstralCanvasMetal_SetVertexData(AstralCanvas::VertexBuffer *vertexBuffer, void *data, usize count)
{
    id<MTLBuffer> buffer = (id<MTLBuffer>)vertexBuffer->handle;
    memcpy(buffer.contents, data, vertexBuffer->vertexType->size * count);
}
void AstralCanvasMetal_SetInstanceData(AstralCanvas::InstanceBuffer *instanceBuffer, void *data, usize count)
{
    id<MTLBuffer> buffer = (id<MTLBuffer>)instanceBuffer->handle;
    memcpy(buffer.contents, data, instanceBuffer->instanceSize * count);
}
void AstralCanvasMetal_SetVertexBuffer(void *commandEncoder, const AstralCanvas::VertexBuffer *vertexBuffer, u32 bindingPoint)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    id<MTLBuffer> buffer = (id<MTLBuffer>)vertexBuffer->handle;
    
    [encoder setVertexBuffer:buffer offset:0 atIndex:bindingPoint];
}
void AstralCanvasMetal_SetInstanceBuffer(void *commandEncoder, const AstralCanvas::InstanceBuffer *instanceBuffer, u32 bindingPoint)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    id<MTLBuffer> buffer = (id<MTLBuffer>)instanceBuffer->handle;
    
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
void AstralCanvasMetal_DestroyInstanceBuffer(AstralCanvas::InstanceBuffer *instanceBuffer)
{
    if (instanceBuffer->handle != NULL)
    {
        id<MTLBuffer> buffer = (id<MTLBuffer>)instanceBuffer->handle;
        [buffer release];
    }
}

void AstralCanvasMetal_CreateUniformBuffer(AstralCanvas::UniformBuffer *uniformBuffer, void* uniformData, usize count)
{
    if (uniformBuffer->handle != NULL)
    {
        uniformBuffer->deinit();
    }
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    id<MTLBuffer> buffer = [gpu newBufferWithBytes:uniformData length:(count) options:MTLCPUCacheModeDefaultCache];
    uniformBuffer->handle = buffer;
}
void AstralCanvasMetal_DestroyUniformBuffer(AstralCanvas::UniformBuffer *uniformBuffer)
{
    if (uniformBuffer->handle != NULL)
    {
        id<MTLBuffer> buffer = (id<MTLBuffer>)uniformBuffer->handle;
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
    id<MTLBuffer> buffer = [gpu newBufferWithBytes:indicesData length:count options:MTLCPUCacheModeDefaultCache];
    indexBuffer->handle = buffer;
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
    id<MTLDevice> gpu = AstralCanvasMetal_GetCurrentGPU();
    
    NSString *vertexNSString = [NSString stringWithUTF8String:vertexSource.buffer];
    NSString *fragmentNSString = [NSString stringWithUTF8String:fragmentSource.buffer];
    
    id<MTLFunction> vertexFunction = NULL;
    id<MTLFunction> fragmentFunction = NULL;
    
    NSError *createVertexError;
    id<MTLLibrary> vertexShader = [gpu newLibraryWithSource:vertexNSString options:NULL error:&createVertexError];
    if (vertexShader != NULL)
    {
        vertexFunction = [vertexShader newFunctionWithName:@"main0"];
        [vertexShader release];
    }
    else 
    {
        printf("Error creating vertex shader: %s\n", [createVertexError.description UTF8String]);
        
        [vertexNSString release];
        return false;
    }
    
    NSError *createFragmentError;
    id<MTLLibrary> fragmentShader = [gpu newLibraryWithSource:fragmentNSString options:NULL error:&createFragmentError];
    if (fragmentShader != NULL)
    {
        fragmentFunction = [fragmentShader newFunctionWithName:@"main0"];
        [fragmentShader release];
    }
    else
    {
        printf("Error creating fragment shader: %s\n", [createFragmentError.description UTF8String]);
        
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

void AstralCanvasMetal_DrawIndexedPrimitives(void *currentCommandEncoderInstance, const AstralCanvas::IndexBuffer *indexBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)currentCommandEncoderInstance;
    
    MTLIndexType indexType = MTLIndexTypeUInt16;
    if (indexBuffer->indexElementSize == AstralCanvas::IndexBufferSize_U32)
    {
        indexType = MTLIndexTypeUInt32;
    }
    
    [encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle indexCount:indexCount indexType:indexType indexBuffer:((id<MTLBuffer>)indexBuffer->handle) indexBufferOffset:0 instanceCount:instanceCount baseVertex:vertexOffset baseInstance:firstInstance];
}

void AstralCanvasMetal_CreateTexture(AstralCanvas::Texture2D *texture)
{
    MTLTextureDescriptor *descriptor = [[MTLTextureDescriptor alloc] init];
    descriptor.pixelFormat = AstralCanvasMetal_FromImageFormat(texture->imageFormat);
    descriptor.width = texture->width;
    descriptor.height = texture->height;
    descriptor.depth = 1;
    if (texture->usedForRenderTarget)
    {
        descriptor.storageMode = MTLStorageModePrivate;
        descriptor.usage = MTLTextureUsageRenderTarget | MTLTextureUsageShaderRead | MTLTextureUsageShaderWrite;
    }
    else 
    {
        //descriptor.storageMode = MTLStorageMode;
        descriptor.usage = MTLTextureUsageShaderRead;
    }
    
    id<MTLTexture> handle = [AstralCanvasMetal_GetCurrentGPU() newTextureWithDescriptor:descriptor];
    
    printf("Created metal texture handle\n");

    [descriptor release];
    
    if (texture->bytes != NULL)
    {
        printf("texture copy attempt: %llu\n", (usize)texture->bytes);
        MTLRegion region = {
            {0, 0, 0},
            {
                texture->width,
                texture->height,
                1
            }
        };
        NSUInteger bytesPerRow = 4 * texture->width;
        [handle replaceRegion:region mipmapLevel:0 withBytes:texture->bytes bytesPerRow:bytesPerRow];
        printf("texture copy complete\n");
    }
    
    texture->imageHandle = handle;
}
void AstralCanvasMetal_DestroyTexture(AstralCanvas::Texture2D *texture)
{
    id<MTLTexture> handle = (id<MTLTexture>)texture->imageHandle;
    [handle release];
}
void AstralCanvasMetal_CreateSampler(AstralCanvas::SamplerState *sampler)
{
    MTLSamplerDescriptor *descriptor = [[MTLSamplerDescriptor alloc] init];
    descriptor.borderColor = MTLSamplerBorderColorTransparentBlack;
    descriptor.sAddressMode = AstralCanvasMetal_FromRepeatMode(sampler->repeatMode);
    descriptor.rAddressMode = AstralCanvasMetal_FromRepeatMode(sampler->repeatMode);
    descriptor.tAddressMode = AstralCanvasMetal_FromRepeatMode(sampler->repeatMode);
    descriptor.minFilter = AstralCanvasMetal_FromSampleMode(sampler->sampleMode);
    descriptor.magFilter = AstralCanvasMetal_FromSampleMode(sampler->sampleMode);
    descriptor.mipFilter = MTLSamplerMipFilterNotMipmapped;//AstralCanvasMetal_FromSampleMode(sampler->sampleMode);
    descriptor.maxAnisotropy = sampler->anisotropyLevel;
    
    id<MTLSamplerState> samplerState = [AstralCanvasMetal_GetCurrentGPU() newSamplerStateWithDescriptor:descriptor];
    
    [descriptor release];
    
    sampler->handle = samplerState;
}
void AstralCanvasMetal_DestroySampler(AstralCanvas::SamplerState *sampler)
{
    id<MTLSamplerState> handle = (id<MTLSamplerState>)sampler->handle;
    [handle release];
}

void AstralCanvasMetal_AddUniformDescriptorSets(AstralCanvas::Shader *shader)
{
    
    AstralCanvas::ShaderVariables shaderVariables = shader->shaderVariables;
    //Metal doesnt have descriptor set objects like Vulkan, so just add NULL to fill the buffer up
    shader->descriptorSets.Add(NULL);
    for (usize i = 0; i < shaderVariables.uniforms.capacity; i++)
    {
        AstralCanvas::ShaderResource *resource = &shaderVariables.uniforms.ptr[i];
        if (resource->variableName.buffer != NULL)
        {
            AstralCanvas::ShaderStagingMutableState newMutableState{};
            switch (resource->type)
            {
                case AstralCanvas::ShaderResourceType_Uniform:
                {
                    newMutableState.ub = AstralCanvas::UniformBuffer(resource->size);
                    newMutableState.ownsUniformBuffer = true;
                    break;
                }
                case AstralCanvas::ShaderResourceType_Texture:
                {
                    newMutableState.textures = collections::Array<AstralCanvas::Texture2D*>(shader->allocator, fmax(resource->arrayLength, 1));
                    newMutableState.imageInfos = shader->allocator->Allocate(sizeof(void*) * newMutableState.textures.length);
                    //newMutableState.imageInfos = this->allocator->Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.textures.length);
                    break;
                }
                case AstralCanvas::ShaderResourceType_Sampler:
                {
                    newMutableState.samplers = collections::Array<AstralCanvas::SamplerState*>(shader->allocator, fmax(resource->arrayLength, 1));
                    newMutableState.samplerInfos = shader->allocator->Allocate(sizeof(void*) * newMutableState.samplers.length);
                    //newMutableState.samplerInfos = this->allocator->Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.samplers.length);
                    break;
                }
                default:
                    break;
            }
            shaderVariables.uniforms.ptr[i].stagingData.Add(newMutableState);
        }
    }
}
void AstralCanvasMetal_SyncUniformsWithGPU(void *commandEncoder, AstralCanvas::Shader *shader)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;
    for (usize i = 0; i < shader->shaderVariables.uniforms.capacity; i++)
    {
        if (shader->shaderVariables.uniforms.ptr[i].variableName.buffer != NULL)
        {
            //should never throw out of range error since CheckDescriptorSetAvailability() is always called prior to this
            AstralCanvas::ShaderStagingMutableState *toMutate = &shader->shaderVariables.uniforms.ptr[i].stagingData.ptr[shader->descriptorForThisDrawCall];
            if (!toMutate->mutated)
            {
                continue;
            }
            //set mutated to false in anticipation of reuse for next frame
            toMutate->mutated = false;
            
            switch (shader->shaderVariables.uniforms.ptr[i].type)
            {
                case AstralCanvas::ShaderResourceType_Uniform:
                {
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Vertex) != 0)
                    {
                        //printf("Setting vertex uniform buffer at %u\n", shader->shaderVariables.uniforms.ptr[i].binding);
                        [encoder setVertexBuffer:(id<MTLBuffer>)toMutate->ub.handle offset:0 atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];
                    }
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Fragment) != 0)
                    {
                        [encoder setFragmentBuffer:(id<MTLBuffer>)toMutate->ub.handle offset:0 atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];
                    }
                    
                    break;
                }
                case AstralCanvas::ShaderResourceType_Texture:
                {
                    for (usize i = 0; i < toMutate->textures.length; i++)
                    {
                        ((void**)toMutate->imageInfos)[i] = toMutate->textures.data[i]->imageHandle;
                    }
                    
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Vertex) != 0)
                    {
                        if (toMutate->textures.length > 1)
                        {
                            NSRange range;
                            range.location = shader->shaderVariables.uniforms.ptr[i].binding + 8;
                            range.length = toMutate->textures.length;
                            [encoder setVertexTextures:(id<MTLTexture>*)toMutate->imageInfos withRange:range];
                        }
                        else [encoder setVertexTexture:(id<MTLTexture>)((id<MTLTexture>*)toMutate->imageInfos)[0] atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];//shader->shaderVariables.uniforms.ptr[i].binding];
                    }
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Fragment) != 0)
                    {
                        if (toMutate->textures.length > 1)
                        {
                            NSRange range;
                            range.location = shader->shaderVariables.uniforms.ptr[i].binding + 8;
                            range.length = toMutate->textures.length;
                            [encoder setFragmentTextures:(id<MTLTexture>*)toMutate->imageInfos withRange:range];
                        }
                        else [encoder setFragmentTexture:(id<MTLTexture>)((id<MTLTexture>*)toMutate->imageInfos)[0] atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];//shader->shaderVariables.uniforms.ptr[i].binding];
                    }
                    break;
                }
                case AstralCanvas::ShaderResourceType_Sampler:
                {
                    for (usize i = 0; i < toMutate->samplers.length; i++)
                    {
                        ((void**)toMutate->samplerInfos)[i] = toMutate->samplers.data[i]->handle;
                    }
                    
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Vertex) != 0)
                    {
                        if (toMutate->samplers.length > 1)
                        {
                            NSRange range;
                            range.location = shader->shaderVariables.uniforms.ptr[i].binding + 8;
                            range.length = toMutate->samplers.length;
                            [encoder setVertexSamplerStates:(id<MTLSamplerState>*)toMutate->samplerInfos withRange:range];
                        }
                        else [encoder setVertexSamplerState:(id<MTLSamplerState>)((id<MTLSamplerState>*)toMutate->samplerInfos)[0] atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];//shader->shaderVariables.uniforms.ptr[i].binding];
                    }
                    if ((shader->shaderVariables.uniforms.ptr[i].accessedBy & AstralCanvas::InputAccessedBy_Fragment) != 0)
                    {
                        if (toMutate->samplers.length > 1)
                        {
                            NSRange range;
                            range.location = shader->shaderVariables.uniforms.ptr[i].binding + 8;
                            range.length = toMutate->samplers.length;
                            [encoder setFragmentSamplerStates:(id<MTLSamplerState>*)toMutate->samplerInfos withRange:range];
                        }
                        else [encoder setFragmentSamplerState:(id<MTLSamplerState>)((id<MTLSamplerState>*)toMutate->samplerInfos)[0] atIndex:shader->shaderVariables.uniforms.ptr[i].binding + 8];//shader->shaderVariables.uniforms.ptr[i].binding];
                    }
                    //printf("Sent sampler at position %i\n", shader->shaderVariables.uniforms.ptr[i].binding);
                    break;
                }
                case AstralCanvas::ShaderResourceType_StructuredBuffer:
                {
                    break;
                }
            }
        }
    }
}
void AstralCanvasMetal_SetClipArea(void *commandEncoder, Maths::Rectangle clipArea)
{
    id<MTLRenderCommandEncoder> encoder = (id<MTLRenderCommandEncoder>)commandEncoder;

    MTLScissorRect metalScissorRect;
    metalScissorRect.width = clipArea.Width;
    metalScissorRect.height = clipArea.Height;
    metalScissorRect.x = clipArea.X;
    metalScissorRect.y = clipArea.Y;
    
    [encoder setScissorRect:metalScissorRect];
}
#endif
