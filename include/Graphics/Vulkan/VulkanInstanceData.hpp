#pragma once
#include <Graphics/Vulkan/VulkanGPU.hpp>

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