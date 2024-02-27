#ifdef ASTRALCANVAS_VULKAN
#include <Graphics/Vulkan/VulkanEngine.hpp>
#include <Graphics/Vulkan/VulkanInstanceData.hpp>
#include "Graphics/Vulkan/vk_mem_alloc.h"
#include "Graphics/SamplerState.hpp"

using namespace collections;

bool onResized;

VkBool32 AstralCanvasVk_ErrorCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	LOG_WARNING(pCallbackData->pMessage);
	// if (messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	// {
	// 	THROW_ERR(pCallbackData->pMessage);
	// }
	// else
	// {
	// 	LOG_WARNING(pCallbackData->pMessage);
	// }
	return VK_FALSE;
}

collections::vector<const char*> AstralCanvasVk_GetDefaultInstanceExtensions(IAllocator* allocator)
{
	u32 count = 0;
	const char** ptrs = glfwGetRequiredInstanceExtensions(&count);

	collections::vector<const char*> result = collections::vector<const char*>(allocator);

	for (u32 i = 0; i < count; i++)
	{
		result.Add(ptrs[i]);
	}

	return result;
}

bool AstralCanvasVk_Initialize(IAllocator* allocator, Array<const char*> validationLayersToUse, Array<const char*> requiredExtensions, AstralCanvas::Window* window)
{
	if (!AstralCanvasVk_CreateInstance(allocator, validationLayersToUse, "", "", 0, 0))
	{
		return false;
	}
	printf("Created vulkan instance\n");

	if (glfwCreateWindowSurface(AstralCanvasVk_GetInstance(), (GLFWwindow*)window->handle, NULL, (VkSurfaceKHR*)&window->windowSurfaceHandle) != VK_SUCCESS)
	{
		printf("Failed to create window surface!\n");
		return false;
	}
	printf("Created window surface\n");
	AstralVulkanGPU gpu;
	if (!AstralCanvasVk_SelectGPU(allocator, AstralCanvasVk_GetInstance(), (VkSurfaceKHR)window->windowSurfaceHandle, requiredExtensions, &gpu))
	{
		return false;
	}
	AstralCanvasVk_SetCurrentGPU(gpu);
	printf("Created GPU interface\n");

	VmaVulkanFunctions vulkanAllocatorFunctions = {};
	vulkanAllocatorFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanAllocatorFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocator vulkanAllocator;
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.device = gpu.logicalDevice;
	allocatorCreateInfo.instance = AstralCanvasVk_GetInstance();
	allocatorCreateInfo.physicalDevice = gpu.physicalDevice;
	allocatorCreateInfo.pAllocationCallbacks = NULL;
	allocatorCreateInfo.pDeviceMemoryCallbacks = NULL;
	allocatorCreateInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorCreateInfo.pVulkanFunctions = &vulkanAllocatorFunctions;

	if (vmaCreateAllocator(&allocatorCreateInfo, &vulkanAllocator) != VK_SUCCESS)
	{
		return false;
	}
	AstralCanvasVk_SetCurrentVulkanAllocator(vulkanAllocator);
	printf("Created memory allocator\n");

	AstralVulkanSwapchain swapchain;
	if (!AstralCanvasVk_CreateSwapchain(allocator, AstralCanvasVk_GetCurrentGPU(), window, &swapchain))
	{
		return false;
	}
	AstralCanvasVk_SetCurrentSwapchain(swapchain);
	printf("Created swapchain\n");

	VkSemaphoreCreateInfo semaphoreCreateInfo;
	semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
	semaphoreCreateInfo.flags = 0;
	semaphoreCreateInfo.pNext = NULL;

	VkSemaphore awaitPresentCompleteSemaphore;
	vkCreateSemaphore(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &semaphoreCreateInfo, NULL, &awaitPresentCompleteSemaphore);

	AstralCanvasVk_SetAwaitPresentCompleteSemaphore(awaitPresentCompleteSemaphore);

	VkSemaphore awaitRenderCompleteSemaphore;
	vkCreateSemaphore(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &semaphoreCreateInfo, NULL, &awaitRenderCompleteSemaphore);

	AstralCanvasVk_SetAwaitRenderCompleteSemaphore(awaitRenderCompleteSemaphore);

	//main rendering command pool (Non transient)
	VkCommandPool mainCmdPool;
	VkCommandPoolCreateInfo cmdPoolCreateInfo{};
	cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
	cmdPoolCreateInfo.queueFamilyIndex = AstralCanvasVk_GetCurrentGPU()->queueInfo.dedicatedGraphicsQueueIndex;
	vkCreateCommandPool(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &cmdPoolCreateInfo, NULL, &mainCmdPool);

	AstralCanvasVk_SetMainCmdPool(mainCmdPool);

	//main command buffer (Non transient)
	VkCommandBuffer mainCmdBuffer;
	VkCommandBufferAllocateInfo cmdBufferInfo{};
	cmdBufferInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	cmdBufferInfo.commandPool = mainCmdPool;
	cmdBufferInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	cmdBufferInfo.commandBufferCount = 1;
	vkAllocateCommandBuffers(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &cmdBufferInfo, &mainCmdBuffer);

	AstralCanvasVk_SetMainCmdBuffer(mainCmdBuffer);

	u32 maxUniformDescriptors = ASTRALVULKAN_MAX_DESCRIPTOR_SETS;
	VkDescriptorPoolSize poolSizes[4];
	poolSizes[0].type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
	poolSizes[0].descriptorCount = maxUniformDescriptors;
	poolSizes[1].type = VK_DESCRIPTOR_TYPE_SAMPLER;
	poolSizes[1].descriptorCount = maxUniformDescriptors;
	poolSizes[2].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	poolSizes[2].descriptorCount = maxUniformDescriptors;
	poolSizes[3].type = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
	poolSizes[3].descriptorCount = maxUniformDescriptors;

	VkDescriptorPoolCreateInfo poolCreateInfo{};
	poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	poolCreateInfo.pPoolSizes = poolSizes;
	poolCreateInfo.poolSizeCount = 4;
	poolCreateInfo.maxSets = maxUniformDescriptors;

	VkDescriptorPool mainPool;
	vkCreateDescriptorPool(gpu.logicalDevice, &poolCreateInfo, NULL, &mainPool);

	AstralCanvasVk_SetDescriptorPool(mainPool);
}

bool AstralCanvasVk_CreateInstance(IAllocator* allocator, Array<const char*> validationLayersToUse, const char* appName, const char* engineName, u32 applicationVersion, u32 engineVersion, u32 vulkanVersion)
{
	VkApplicationInfo appInfo;
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = appName;
	appInfo.pEngineName = engineName;
	appInfo.apiVersion = vulkanVersion;
	appInfo.engineVersion = engineVersion;
	appInfo.applicationVersion = applicationVersion;
	appInfo.pNext = NULL;

	collections::vector<const char*> extensions = AstralCanvasVk_GetDefaultInstanceExtensions(allocator);
	if (AstralCanvasVk_ValidationLayersIsEnabled())
	{
		extensions.Add(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
	}

	VkInstanceCreateInfo instanceInfo = {};
	instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	instanceInfo.pApplicationInfo = &appInfo;
	instanceInfo.enabledExtensionCount = extensions.count;
	instanceInfo.ppEnabledExtensionNames = extensions.ptr;
	instanceInfo.flags = 0;
	if (AstralCanvasVk_ValidationLayersIsEnabled())
	{
		//check validation layer support
		u32 layerCount = 0;
		vkEnumerateInstanceLayerProperties(&layerCount, NULL);

		//LayerProperties[] layerProperties = new LayerProperties[layerCount];
		Array<VkLayerProperties> layerProperties = Array<VkLayerProperties>(allocator, layerCount);
		vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data);

		u32 totalSupported = 0;
		for (usize i = 0; i < layerProperties.length; i++)
		{
			for (usize j = 0; j < validationLayersToUse.length; j++)
			{
				if (strcmp(validationLayersToUse.data[j], layerProperties.data[i].layerName) == 0)
				{
					totalSupported++;
				}
			}
		}
		if (totalSupported != validationLayersToUse.length)
		{
			LOG_WARNING("Not all requested Vulkan validation layers are supported");
			return false;
		}

		instanceInfo.enabledLayerCount = validationLayersToUse.length;
		instanceInfo.ppEnabledLayerNames = validationLayersToUse.data;

		VkDebugUtilsMessengerCreateInfoEXT AstralCanvasVk_debugCreateInfo = {};

		AstralCanvasVk_debugCreateInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;//StructureType.DebugUtilsMessengerCreateInfoExt;
		AstralCanvasVk_debugCreateInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT;
		AstralCanvasVk_debugCreateInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT;
		AstralCanvasVk_debugCreateInfo.pNext = NULL;
		AstralCanvasVk_debugCreateInfo.pUserData = NULL;
		AstralCanvasVk_debugCreateInfo.pfnUserCallback = &AstralCanvasVk_ErrorCallback;
		AstralCanvasVk_SetDebugCreateInfo(AstralCanvasVk_debugCreateInfo);
		instanceInfo.pNext = AstralCanvasVk_GetDebugCreateInfo();

		layerProperties.deinit();
	}
	else
	{
		instanceInfo.enabledLayerCount = 0;
		instanceInfo.ppEnabledLayerNames = NULL;
		instanceInfo.pNext = NULL;
	}

	VkInstance instance;
	bool result = vkCreateInstance(&instanceInfo, NULL, &instance) == VK_SUCCESS;
	if (result)
	{
		AstralCanvasVk_SetInstance(instance);
	}
	return result;
}

bool AstralCanvasVk_CreateDebugMessenger()
{
	PFN_vkCreateDebugUtilsMessengerEXT func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(AstralCanvasVk_GetInstance(), "vkCreateDebugUtilsMessengerEXT");
	if (func != NULL) 
	{
		VkDebugUtilsMessengerEXT messenger;

		bool result = func(AstralCanvasVk_GetInstance(), AstralCanvasVk_GetDebugCreateInfo(), NULL, &messenger) == VK_SUCCESS;

		AstralCanvasVk_SetDebugMessenger(messenger);
		return result;
	}
	else 
	{
		return false;
	}
}

void AstralCanvasVk_AwaitShutdown()
{
	vkQueueWaitIdle(AstralCanvasVk_GetCurrentGPU()->DedicatedGraphicsQueue.queue);
}
void AstralCanvasVk_Deinitialize(IAllocator* allocator, AstralCanvas::Window* window)
{
	AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
	AstralCanvas::DestroyDefaultSamplerStates();
	//vkWaitForFences(gpu->logicalDevice, 1, &gpu->DedicatedGraphicsQueue.queueFence, true, UINT64_MAX);

	VkDescriptorPool mainDescriptorPool = AstralCanvasVk_GetDescriptorPool();
	if (mainDescriptorPool != NULL)
	{
		vkDestroyDescriptorPool(gpu->logicalDevice, mainDescriptorPool, NULL);
	}

	VkCommandPool mainCommandPool = AstralCanvasVk_GetMainCmdPool();
	if (mainCommandPool != NULL)
	{
		vkDestroyCommandPool(gpu->logicalDevice, mainCommandPool, NULL);
	}

	VkSemaphore semaphore = AstralCanvasVk_GetAwaitRenderCompleteSemaphore();
	if (semaphore != NULL)
	{
		vkDestroySemaphore(gpu->logicalDevice, semaphore, NULL);
	}
	semaphore = AstralCanvasVk_GetAwaitPresentCompleteSemaphore();
	if (semaphore != NULL)
	{
		vkDestroySemaphore(gpu->logicalDevice, semaphore, NULL);
	}

	AstralVulkanSwapchain *swapchain = AstralCanvasVk_GetCurrentSwapchain();
	if (swapchain != NULL)
	{
		AstralCanvasVk_DestroySwapchain(swapchain);
	}

	VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();
	if (vma != NULL)
	{
		vmaDestroyAllocator(vma);
	}

	if (gpu != NULL)
	{
		AstralCanvasVk_ReleaseGPU(gpu);
	}

	VkInstance instance = AstralCanvasVk_GetInstance();
	vkDestroySurfaceKHR(instance, (VkSurfaceKHR)window->windowSurfaceHandle, NULL);

	if (AstralCanvasVk_ValidationLayersIsEnabled())
	{
		auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
		if (func != nullptr) {
			func(instance, AstralCanvasVk_GetDebugMessenger(), NULL);
		}
	}

	if (instance != NULL)
	{
		vkDestroyInstance(instance, NULL);
	}
}

void AstralCanvasVk_BeginDraw()
{
	AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
	AstralVulkanSwapchain *swapchain = AstralCanvasVk_GetCurrentSwapchain();

	VkFence toWaitFor = gpu->DedicatedGraphicsQueue.queueFence;
	vkWaitForFences(gpu->logicalDevice, 1, &toWaitFor, true, UINT64_MAX);
	vkResetFences(gpu->logicalDevice, 1, &toWaitFor);

	swapchain->recreatedThisFrame = false;

	if (AstralCanvasVk_SwapchainSwapBuffers(gpu, swapchain, AstralCanvasVk_GetAwaitPresentCompleteSemaphore(), NULL))
	{
		onResized = true;
		swapchain->recreatedThisFrame = true;
		return;
	}

	VkCommandBuffer mainCmdBuffer = AstralCanvasVk_GetMainCmdBuffer();
	vkResetCommandBuffer(mainCmdBuffer, 0);

	VkCommandBufferBeginInfo beginInfo{};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.pInheritanceInfo = NULL;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	if (vkBeginCommandBuffer(mainCmdBuffer, &beginInfo) != VK_SUCCESS)
	{
		THROW_ERR("Failed to begin command buffer");
	}
}
void AstralCanvasVk_EndDraw()
{
	//submit to GPU
	vkEndCommandBuffer(AstralCanvasVk_GetMainCmdBuffer());

	VkSemaphore awaitPresentComplete = AstralCanvasVk_GetAwaitPresentCompleteSemaphore();
	VkSemaphore awaitRenderComplete = AstralCanvasVk_GetAwaitRenderCompleteSemaphore();
	VkPipelineStageFlags waitFlags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	VkCommandBuffer mainCmdBuffer = AstralCanvasVk_GetMainCmdBuffer();

	VkSubmitInfo submitInfo{};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pWaitDstStageMask = &waitFlags;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &awaitPresentComplete;
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &awaitRenderComplete;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &mainCmdBuffer;

	AstralVulkanGPU *gpu = AstralCanvasVk_GetCurrentGPU();
	gpu->DedicatedGraphicsQueue.queueMutex.EnterLock();
	if (vkQueueSubmit(gpu->DedicatedGraphicsQueue.queue, 1, &submitInfo, gpu->DedicatedGraphicsQueue.queueFence) != VK_SUCCESS)
	{
		THROW_ERR("Error submitting vulkan queue");
	}
	gpu->DedicatedGraphicsQueue.queueMutex.ExitLock();

	AstralVulkanSwapchain* swapchain = AstralCanvasVk_GetCurrentSwapchain();

	VkPresentInfoKHR presentInfo{};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &swapchain->handle;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = &awaitRenderComplete;
	presentInfo.pImageIndices = &swapchain->currentImageIndex;

	gpu->DedicatedGraphicsQueue.queueMutex.EnterLock();
	VkResult presentResults = vkQueuePresentKHR(gpu->DedicatedGraphicsQueue.queue, &presentInfo);
	swapchain->renderTargets.data[swapchain->currentImageIndex].textures.data[0].imageLayout = (u32)VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	gpu->DedicatedGraphicsQueue.queueMutex.ExitLock();

	if (presentResults == VK_ERROR_OUT_OF_DATE_KHR || onResized)
	{
		if (!swapchain->recreatedThisFrame)
		{
            AstralCanvasVk_DestroySwapchain(swapchain);
            AstralCanvasVk_SwapchainRecreate(swapchain, gpu);
		}
		if (onResized)
		{
			onResized = false;
		}
		if (presentResults == VK_ERROR_OUT_OF_DATE_KHR)
		{
			presentResults = VK_SUCCESS;
		}
		//swapChain.Recreate(SwapChain.QuerySwapChainSupport(CurrentGPU.Device));
	}

	if (presentResults != VK_SUCCESS)
	{
		THROW_ERR("Error presenting queue");
	}
}
#endif
