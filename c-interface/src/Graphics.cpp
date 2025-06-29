#include "Astral.Canvas/Graphics/Graphics.h"
#include "Graphics/Graphics.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanSwapchain.hpp"
#endif

exportC AstralCanvasRenderProgram AstralCanvasGraphics_GetCurrentRenderProgram(AstralCanvasGraphics ptr)
{
    return (AstralCanvasRenderProgram)((AstralCanvas::Graphics *)ptr)->currentRenderProgram;
}
exportC AstralCanvasRenderTarget AstralCanvasGraphics_GetCurrentRenderTarget(AstralCanvasGraphics ptr)
{
    return (AstralCanvasRenderTarget)((AstralCanvas::Graphics *)ptr)->currentRenderTarget;
}
exportC u32 AstralCanvasGraphics_SwapchainFrameCount(AstralCanvasGraphics ptr)
{
    AstralCanvas::Window *win = ((AstralCanvas::Graphics *)ptr)->currentWindow;
    assert(win != NULL);
    void *swapchainPtr = win->swapchain;
    switch (AstralCanvas::GetActiveBackend())
    {
        #ifdef ASTRALCANVAS_VULKAN
        case AstralCanvas::Backend_Vulkan:
        {
            AstralVulkanSwapchain *swapchain = (AstralVulkanSwapchain *)swapchainPtr;
            return swapchain->imageCount; //->renderTargets.data[swapchain->currentImageIndex];
        }
        #endif
        default:
            THROW_ERR("Unimplemented backend: AstralCanvasGraphics_SwapchainFrameCount");
            break;
    }
}
exportC u32 AstralCanvasGraphics_GetCurrentSwapchainFrame(AstralCanvasGraphics ptr)
{
    void *swapchainPtr = ((AstralCanvas::Graphics *)ptr)->currentWindow->swapchain;
    switch (AstralCanvas::GetActiveBackend())
    {
        #ifdef ASTRALCANVAS_VULKAN
        case AstralCanvas::Backend_Vulkan:
        {
            AstralVulkanSwapchain *swapchain = (AstralVulkanSwapchain *)swapchainPtr;
            return swapchain->currentImageIndex; //->renderTargets.data[swapchain->currentImageIndex];
        }
        #endif
        default:
            THROW_ERR("Unimplemented backend: AstralCanvasGraphics_GetCurrentSwapchainFrame");
            break;
    }
}
exportC AstralCanvasRenderTarget AstralCanvasGraphics_GetSwapchainFrameRenderTarget(AstralCanvasGraphics ptr, u32 index)
{
    void *swapchainPtr = (AstralCanvasRenderTarget)((AstralCanvas::Graphics *)ptr)->currentWindow->swapchain;
    switch (AstralCanvas::GetActiveBackend())
    {
        #ifdef ASTRALCANVAS_VULKAN
        case AstralCanvas::Backend_Vulkan:
        {
            AstralVulkanSwapchain *swapchain = (AstralVulkanSwapchain *)swapchainPtr;
            return &swapchain->renderTargets.data[index]; //->renderTargets.data[swapchain->currentImageIndex];
        }
        #endif
        default:
            THROW_ERR("Unimplemented backend: AstralCanvasGraphics_GetSwapchainFrameRenderTarget");
            break;
    }
}
exportC AstralCanvasRenderTarget AstralCanvasGraphics_GetCurrentWindowRendertarget(AstralCanvasGraphics ptr)
{
    return (AstralCanvasRenderTarget)((AstralCanvas::Graphics *)ptr)->GetCurrentWindowRendertarget();
}
exportC u32 AstralCanvasGraphics_GetCurrentRenderProgramPass(AstralCanvasGraphics ptr)
{
    return ((AstralCanvas::Graphics *)ptr)->currentRenderPass;
}
exportC void AstralCanvasGraphics_SetVertexBuffer(AstralCanvasGraphics ptr, const AstralCanvasVertexBuffer vb, u32 bindingPoint)
{
    ((AstralCanvas::Graphics *)ptr)->SetVertexBuffer((const AstralCanvas::VertexBuffer*)vb, bindingPoint);
}
exportC void AstralCanvasGraphics_SetComputeBufferAsVertexBuffer(AstralCanvasGraphics ptr, const AstralCanvasComputeBuffer computeBuffer, u32 bindingPoint)
{
    ((AstralCanvas::Graphics *)ptr)->SetComputeBufferAsVertexBuffer((const AstralCanvas::ComputeBuffer*)computeBuffer, bindingPoint);
}
exportC void AstralCanvasGraphics_SetInstanceBuffer(AstralCanvasGraphics ptr, const AstralCanvasInstanceBuffer instanceBuffer, u32 bindingPoint)
{
    ((AstralCanvas::Graphics *)ptr)->SetInstanceBuffer((const AstralCanvas::InstanceBuffer*)instanceBuffer, bindingPoint);
}
exportC void AstralCanvasGraphics_SetIndexBuffer(AstralCanvasGraphics ptr, const AstralCanvasIndexBuffer indexBuffer)
{
    ((AstralCanvas::Graphics *)ptr)->SetIndexBuffer((const AstralCanvas::IndexBuffer*)indexBuffer);
}
exportC void AstralCanvasGraphics_SetRenderTarget(AstralCanvasGraphics ptr, AstralCanvasRenderTarget target)
{
    ((AstralCanvas::Graphics *)ptr)->SetRenderTarget((AstralCanvas::RenderTarget*)target);
}
exportC void AstralCanvasGraphics_StartRenderProgram(AstralCanvasGraphics ptr, AstralCanvasRenderProgram program, AstralCanvasColor clearColor)
{
    ((AstralCanvas::Graphics *)ptr)->StartRenderProgram((AstralCanvas::RenderProgram*)program, AstralCanvas::Color(clearColor.R, clearColor.G, clearColor.B, clearColor.A));
}
exportC void AstralCanvasGraphics_EndRenderProgram(AstralCanvasGraphics ptr)
{
    ((AstralCanvas::Graphics *)ptr)->EndRenderProgram();
}
exportC void AstralCanvasGraphics_UseRenderPipeline(AstralCanvasGraphics ptr, AstralCanvasRenderPipeline pipeline)
{
    ((AstralCanvas::Graphics *)ptr)->UseRenderPipeline((AstralCanvas::RenderPipeline*)pipeline);
}
exportC void AstralCanvasGraphics_AwaitGraphicsIdle(AstralCanvasGraphics ptr)
{
    ((AstralCanvas::Graphics *)ptr)->AwaitGraphicsIdle();
}
exportC void AstralCanvasGraphics_SetShaderVariable(AstralCanvasGraphics ptr, const char* variableName, void* data, usize size)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariable(variableName, data, size);
}
exportC void AstralCanvasGraphics_SetShaderVariableTexture(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D texture)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariableTexture(variableName, (AstralCanvas::Texture2D*)texture);
}
exportC void AstralCanvasGraphics_SetShaderVariableTextures(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasTexture2D *textures, usize count)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariableTextures(variableName, (AstralCanvas::Texture2D**)textures, count);
}
exportC void AstralCanvasGraphics_SetShaderVariableSampler(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState sampler)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariableSampler(variableName, (AstralCanvas::SamplerState*)sampler);
}
exportC void AstralCanvasGraphics_SetShaderVariableSamplers(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasSamplerState *samplers, usize count)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariableSamplers(variableName, (AstralCanvas::SamplerState**)samplers, count);
}
exportC void AstralCanvasGraphics_SetShaderVariableComputeBuffer(AstralCanvasGraphics ptr, const char* variableName, AstralCanvasComputeBuffer computeBuffer)
{
    ((AstralCanvas::Graphics *)ptr)->SetShaderVariableComputeBuffer(variableName, (AstralCanvas::ComputeBuffer*)computeBuffer);
}
exportC void AstralCanvasGraphics_SendUpdatedUniforms(AstralCanvasGraphics ptr)
{
    ((AstralCanvas::Graphics *)ptr)->SendUpdatedUniforms();
}
exportC void AstralCanvasGraphics_DrawIndexedPrimitives(AstralCanvasGraphics ptr, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance)
{
    ((AstralCanvas::Graphics *)ptr)->DrawIndexedPrimitives(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
}
exportC void AstralCanvasGraphics_DrawIndexedPrimitivesIndirectCount(AstralCanvasGraphics ptr, AstralCanvasComputeBuffer drawDataBuffer, usize drawDataBufferOffset, AstralCanvasComputeBuffer drawCountBuffer, usize drawCountBufferOffset, u32 maxDrawCount)
{
    ((AstralCanvas::Graphics *)ptr)->DrawIndexedPrimitivesIndirectCount((AstralCanvas::ComputeBuffer*)drawDataBuffer, drawDataBufferOffset, (AstralCanvas::ComputeBuffer*)drawCountBuffer, drawCountBufferOffset, maxDrawCount);
}
exportC void AstralCanvasGraphics_DrawIndexedPrimitivesIndirect(AstralCanvasGraphics ptr, AstralCanvasComputeBuffer drawDataBuffer, usize drawDataBufferOffset, u32 drawCount)
{
    ((AstralCanvas::Graphics *)ptr)->DrawIndexedPrimitivesIndirect((AstralCanvas::ComputeBuffer*)drawDataBuffer, drawDataBufferOffset, drawCount);
}
exportC void AstralCanvasGraphics_NextRenderPass(AstralCanvasGraphics ptr)
{
    ((AstralCanvas::Graphics *)ptr)->NextRenderPass();
}
exportC AstralCanvasClipArea AstralCanvasGraphics_GetClipArea(AstralCanvasGraphics ptr)
{
    Maths::Rectangle rect = ((AstralCanvas::Graphics *)ptr)->ClipArea;
    return {rect.X, rect.Y, rect.Width, rect.Height};
}
exportC void AstralCanvasGraphics_SetClipArea(AstralCanvasGraphics ptr, i32 x, i32 y, i32 w, i32 h)
{
    ((AstralCanvas::Graphics *)ptr)->SetClipArea({x, y, w, h});
}
exportC AstralCanvasClipArea AstralCanvasGraphics_GetViewport(AstralCanvasGraphics ptr)
{
    Maths::Rectangle rect = ((AstralCanvas::Graphics *)ptr)->Viewport;
    return {rect.X, rect.Y, rect.Width, rect.Height};
}
exportC void AstralCanvasGraphics_SetViewport(AstralCanvasGraphics ptr, i32 x, i32 y, i32 w, i32 h)
{
    ((AstralCanvas::Graphics *)ptr)->Viewport = {x, y, w, h};
}