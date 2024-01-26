#include "Graphics/RenderTarget.hpp"
#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#endif

void AstralCanvas_DestroyRenderTarget(RenderTarget *renderTarget)
{

}
RenderTarget AstralCanvas_CreateRenderTargetFrom(Texture2D thisBackendTexture, Texture2D thisDepthBuffer)
{

}
RenderTarget AstralCanvas_CreateRenderTargetFromWH(u32 width, u32 height, ImageFormat imageFormat, ImageFormat depthFormat)
{

}