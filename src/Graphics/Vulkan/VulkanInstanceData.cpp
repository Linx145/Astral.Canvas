#include "Graphics/Vulkan/VulkanInstanceData.hpp"

VkDebugUtilsMessengerCreateInfoEXT      AstralCanvasVk_debugCreateInfo = {};
VkDebugUtilsMessengerEXT                AstralCanvasVk_debugMessenger = {};
bool                                    AstralCanvasVk_validationLayers = false;
VkInstance                              AstralCanvasVk_instance = NULL;
AstralVulkanGPU                         AstralCanvasVk_GPU = {};
VmaAllocator                            AstralCanvasVk_vma = NULL;
AstralVulkanSwapchain                   AstralCanvasVk_swapchain = {};

VkDebugUtilsMessengerEXT AstralCanvasVk_GetDebugMessenger()
{
    return AstralCanvasVk_debugMessenger;
}
void AstralCanvasVk_SetDebugMessenger(VkDebugUtilsMessengerEXT messenger)
{
    AstralCanvasVk_debugMessenger = messenger;
}

VkDebugUtilsMessengerCreateInfoEXT *AstralCanvasVk_GetDebugCreateInfo()
{
    return &AstralCanvasVk_debugCreateInfo;
}
void AstralCanvasVk_SetDebugCreateInfo(VkDebugUtilsMessengerCreateInfoEXT info)
{
    AstralCanvasVk_debugCreateInfo = info;
}

bool AstralCanvasVk_ValidationLayersIsEnabled()
{
    return AstralCanvasVk_validationLayers;
}
void AstralCanvasVk_ValidationLayersGetEnabled(bool value)
{
    AstralCanvasVk_validationLayers = value;
}

VkInstance AstralCanvasVk_GetInstance()
{
    return AstralCanvasVk_instance;
}
void AstralCanvasVk_SetInstance(VkInstance instance)
{
    AstralCanvasVk_instance = instance;
}

AstralVulkanGPU *AstralCanvasVk_GetCurrentGPU()
{
    return &AstralCanvasVk_GPU;
}
void AstralCanvasVk_SetCurrentGPU(AstralVulkanGPU gpu)
{
    AstralCanvasVk_GPU = gpu;
}

VmaAllocator AstralCanvasVk_GetCurrentVulkanAllocator()
{
    return AstralCanvasVk_vma;
}
void AstralCanvasVk_SetCurrentVulkanAllocator(VmaAllocator allocator)
{
    AstralCanvasVk_vma = allocator;
}

AstralVulkanSwapchain *AstralCanvasVk_GetCurrentSwapchain()
{
    return &AstralCanvasVk_swapchain;
}
void AstralCanvasVk_SetCurrentSwapchain(AstralVulkanSwapchain swapchain)
{
    AstralCanvasVk_swapchain = swapchain;
}