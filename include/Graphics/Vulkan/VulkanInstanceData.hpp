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