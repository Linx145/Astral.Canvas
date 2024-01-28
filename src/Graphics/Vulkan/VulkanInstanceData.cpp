#include "Graphics/Vulkan/VulkanInstanceData.hpp"

VkDebugUtilsMessengerCreateInfoEXT      AstralCanvasVk_debugCreateInfo = {};
VkDebugUtilsMessengerEXT                AstralCanvasVk_debugMessenger = {};
bool                                    AstralCanvasVk_validationLayers = true;
VkInstance                              AstralCanvasVk_instance = NULL;
AstralVulkanGPU                         AstralCanvasVk_GPU = {};
VmaAllocator                            AstralCanvasVk_vma = NULL;
AstralVulkanSwapchain                   AstralCanvasVk_swapchain = {};
VkSemaphore                             AstralCanvasVk_AwaitPresentCompleteSemaphore = NULL;
VkSemaphore                             AstralCanvasVk_AwaitRenderCompleteSemaphore = NULL;
VkCommandPool                           AstralCanvasVk_MainCommandPool = NULL;
VkCommandBuffer                         AstralCanvasVk_MainCommandBuffer = NULL;

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

VkSemaphore AstralCanvasVk_GetAwaitPresentCompleteSemaphore()
{
    return AstralCanvasVk_AwaitPresentCompleteSemaphore;
}
void AstralCanvasVk_SetAwaitPresentCompleteSemaphore(VkSemaphore semaphore)
{
    AstralCanvasVk_AwaitPresentCompleteSemaphore = semaphore;
}

VkSemaphore AstralCanvasVk_GetAwaitRenderCompleteSemaphore()
{
    return AstralCanvasVk_AwaitRenderCompleteSemaphore;
}
void AstralCanvasVk_SetAwaitRenderCompleteSemaphore(VkSemaphore semaphore)
{
    AstralCanvasVk_AwaitRenderCompleteSemaphore = semaphore;
}

VkCommandPool AstralCanvasVk_GetMainCmdPool()
{
    return AstralCanvasVk_MainCommandPool;
}
void AstralCanvasVk_SetMainCmdPool(VkCommandPool commandPool)
{
    AstralCanvasVk_MainCommandPool = commandPool;
}

VkCommandBuffer AstralCanvasVk_GetMainCmdBuffer()
{
    return AstralCanvasVk_MainCommandBuffer;
}
void AstralCanvasVk_SetMainCmdBuffer(VkCommandBuffer commandBuffer)
{
    AstralCanvasVk_MainCommandBuffer = commandBuffer;
}