#pragma once
#include "Graphics/SamplerState.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/Vulkan/VulkanGPU.hpp"

VkCommandBuffer AstralCanvasVk_CreateTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, bool alsoBeginBuffer);
void AstralCanvasVk_EndTransientCommandBuffer(AstralVulkanGPU *gpu, AstralCanvasVkCommandQueue *queueToUse, VkCommandBuffer commandBuffer);

void AstralCanvasVk_TransitionTextureLayout(AstralVulkanGPU *gpu, Texture2D *texture, VkImageAspectFlags aspectFlags, VkImageLayout oldLayout, VkImageLayout newLayout);

void AstralCanvasVk_CreateSamplerState(AstralVulkanGPU *gpu, SamplerState *samplerState);
void AstralCanvasVk_DestroySamplerState(AstralVulkanGPU *gpu, SamplerState *samplerState);

void AstralCanvasVk_CreateTexture2D(AstralVulkanGPU *gpu, Texture2D *texture);