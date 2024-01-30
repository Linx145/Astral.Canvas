#pragma once
#include <Graphics/Vulkan/VulkanGPU.hpp>
#include <Graphics/Vulkan/VulkanSwapchain.hpp>
#include "Graphics/Vulkan/vk_mem_alloc.h"

VkDebugUtilsMessengerCreateInfoEXT *AstralCanvasVk_GetDebugCreateInfo();
void AstralCanvasVk_SetDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT info);

VkDebugUtilsMessengerEXT AstralCanvasVk_GetDebugMessenger();
void AstralCanvasVk_SetDebugMessenger(VkDebugUtilsMessengerEXT messenger);

bool AstralCanvasVk_ValidationLayersIsEnabled();
void AstralCanvasVk_ValidationLayersGetEnabled(bool value);

VkInstance AstralCanvasVk_GetInstance();
void AstralCanvasVk_SetInstance(VkInstance instance);

AstralVulkanGPU *AstralCanvasVk_GetCurrentGPU();
void AstralCanvasVk_SetCurrentGPU(AstralVulkanGPU gpu);

VmaAllocator AstralCanvasVk_GetCurrentVulkanAllocator();
void AstralCanvasVk_SetCurrentVulkanAllocator(VmaAllocator allocator);

AstralVulkanSwapchain *AstralCanvasVk_GetCurrentSwapchain();
void AstralCanvasVk_SetCurrentSwapchain(AstralVulkanSwapchain swapchain);

VkSemaphore AstralCanvasVk_GetAwaitPresentCompleteSemaphore();
void AstralCanvasVk_SetAwaitPresentCompleteSemaphore(VkSemaphore semaphore);

VkSemaphore AstralCanvasVk_GetAwaitRenderCompleteSemaphore();
void AstralCanvasVk_SetAwaitRenderCompleteSemaphore(VkSemaphore semaphore);

VkCommandPool AstralCanvasVk_GetMainCmdPool();
void AstralCanvasVk_SetMainCmdPool(VkCommandPool commandBuffer);

VkCommandBuffer AstralCanvasVk_GetMainCmdBuffer();
void AstralCanvasVk_SetMainCmdBuffer(VkCommandBuffer commandBuffer);

VkDescriptorPool AstralCanvasVk_GetDescriptorPool();
void AstralCanvasVk_SetDescriptorPool(VkDescriptorPool pool);