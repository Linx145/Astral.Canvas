#pragma once

#ifndef ASTRAL_VK_H
#define ASTRAL_VK_H
#include <Linxc.h>
#include <vulkan/vulkan.h>
#include <Windowing/Window.hpp>
#include <GLFW/glfw3.h>
#include "vector.hpp"
#include "array.hpp"
#include "string.hpp"
#include "ErrorHandling.hpp"

extern VkDebugUtilsMessengerCreateInfoEXT AstralCanvasVk_debugCreateInfo;
extern VkDebugUtilsMessengerEXT AstralCanvasVk_debugMessenger;
extern bool AstralCanvasVk_validationLayers;
extern VkInstance AstralCanvasVk_Instance;

VkBool32 AstralCanvasVk_ErrorCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageTypes, const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* pUserData);

bool AstralCanvasVk_Initialize(IAllocator* allocator, collections::Array<const char*> validationLayersToUse, collections::Array<const char*> requiredExtensions, AstralCanvasWindow* window);

collections::vector<const char*> AstralCanvasVk_GetDefaultInstanceExtensions(IAllocator *allocator);

bool AstralCanvasVk_CreateInstance(IAllocator* allocator, collections::Array<const char*> validationLayersToUse, const char* appName, const char* engineName, u32 applicationVersion, u32 engineVersion, u32 vulkanVersion = VK_API_VERSION_1_3);

bool AstralCanvasVk_CreateDebugMessenger();

void AstralCanvasVk_Deinitialize(IAllocator *allocator, AstralCanvasWindow *window);
#endif