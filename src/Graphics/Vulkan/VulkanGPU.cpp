#include "Graphics/Vulkan/VulkanGPU.hpp"
#include <Graphics/Vulkan/VulkanSwapchainSupportDetails.hpp>

using namespace collections;

option<AstralVulkanGPU> AstralCanvasVk_SelectGPU(IAllocator *allocator, VkInstance instance, VkSurfaceKHR windowSurface, Array<const char*> requiredExtensions)
{
	u32 deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, NULL) != VK_SUCCESS || deviceCount == 0)
	{
		//No GPUs with Vulkan support
		return option<AstralVulkanGPU>();
	}

	IAllocator cAllocator = GetCAllocator();

	Array<AstralVulkanGPU> gpus = Array<AstralVulkanGPU>();
	Array<VkPhysicalDevice> physicalDevices = Array<VkPhysicalDevice>(&cAllocator, deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data);

	for (usize i = 0; i < physicalDevices.length; i++)
	{
		u32 flagsCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices.data[i], &flagsCount, NULL);

		//need to allocate with regular allocator just in case we are going to be persistent
		Array<VkQueueFamilyProperties> allFamilyProperties = Array<VkQueueFamilyProperties>(allocator);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices.data[i], &flagsCount, allFamilyProperties.data);

		gpus.data[i] = AstralVulkanGPU(requiredExtensions, physicalDevices.data[i], AstralVulkanQueueProperties(allocator, allFamilyProperties, windowSurface));
	}
	physicalDevices.deinit();

	u32 maxScore = 0;
	u32 bestGPU = -1;
	for (u32 i = 0; i < gpus.length; i++)
	{
		u32 newScore = AstralCanvasVk_GetGPUScore(&gpus.data[i], windowSurface);
		if (newScore > maxScore)
		{
			bestGPU = i;
		}
	}

	if (bestGPU == -1)
	{
		return option<AstralVulkanGPU>();
	}
	AstralVulkanGPU result = gpus.data[bestGPU];
	for (usize i = 0; i < gpus.length; i++)
	{
		if (i != bestGPU)
		{
			//dont call deinit here because it'll attempt to destroy the logical device
			//which doesnt exist
			//just deinit the queueInfo, which contains the array we created for it earlier (allFamilyProperties)
			gpus.data[i].queueInfo.deinit();
		}
	}
	gpus.deinit();

	return result;
}
u32 AstralCanvasVk_GetGPUScore(AstralVulkanGPU* gpu, VkSurfaceKHR windowSurface)
{
	u32 score = 0;

	if (AstralCanvasVk_GetGraphicsQueue(&gpu->queueInfo, gpu->physicalDevice, true) == -1)
	{
		return 0;
	}

	if (!AstralCanvasVk_GPUExtensionsSupported(gpu))
	{
		return 0;
	}

	IAllocator cAllocator = GetCAllocator();
	AstralVulkanSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(gpu->physicalDevice, windowSurface, &cAllocator);
	bool swapchainSupports = details.presentModes.length > 0 && details.supportedSurfaceFormats.length > 0;
	details.deinit();
	if (!swapchainSupports) //the gpu doesn't support the swapchain
	{
		return 0;
	}

	if (gpu->properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 1;
	}
	return score;
}
bool AstralCanvasVk_GPUExtensionsSupported(AstralVulkanGPU *gpu)
{
	IAllocator cAllocator = GetCAllocator();

	u32 supportedExtensionsCount = 0;
	vkEnumerateDeviceExtensionProperties(gpu->physicalDevice, NULL, &supportedExtensionsCount, NULL);

	Array<VkExtensionProperties> properties = Array<VkExtensionProperties>(&cAllocator);

	vkEnumerateDeviceExtensionProperties(gpu->physicalDevice, NULL, &supportedExtensionsCount, properties.data);

	i32 totalSupported = 0;
	for (usize i = 0; i < properties.length; i++)
	{
		for (usize j = 0; j < gpu->requiredExtensions.length; j++)
		{
			if (gpu->requiredExtensions.data[j] == properties.data[i].extensionName)
			{
				totalSupported++;
				break;
			}
		}
	}

	properties.deinit();
	return totalSupported == gpu->requiredExtensions.length;
}

bool AstralCanvasVk_CreateLogicalDevice(AstralVulkanGPU* gpu, IAllocator* allocator)
{
	collections::vector<VkDeviceQueueCreateInfo> createInfos = collections::vector<VkDeviceQueueCreateInfo>(allocator);
	float queuePriorities = 1.0f;

	VkDeviceQueueCreateInfo createInfo;
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.queueCount = 1;
	createInfo.pQueuePriorities = &queuePriorities;

	i32 graphicsQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Graphics);
	createInfo.queueFamilyIndex = graphicsQueueIndex;
	createInfos.Add(createInfo);
	
	i32 transferQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Transfer);
	createInfo.queueFamilyIndex = transferQueueIndex;
	createInfos.Add(createInfo);
	
	i32 computeQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Compute);
	createInfo.queueFamilyIndex = computeQueueIndex;
	createInfos.Add(createInfo);

	VkDeviceCreateInfo deviceCreateInfo;
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = (u32)createInfos.count;
	deviceCreateInfo.pQueueCreateInfos = createInfos.ptr;
	deviceCreateInfo.enabledExtensionCount = (u32)gpu->requiredExtensions.length;
	deviceCreateInfo.ppEnabledExtensionNames = gpu->requiredExtensions.data;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;

	if (vkCreateDevice(gpu->physicalDevice, &deviceCreateInfo, NULL, &gpu->logicalDevice) != VK_SUCCESS)
	{
		return false;
	}

	for (usize i = 0; i < gpu->queueInfo.queueCreationResults.count; i++)
	{
		AstralVulkanQueueResult *queueData = gpu->queueInfo.queueCreationResults.Get(i);
		VkQueue queue;
		vkGetDeviceQueue(gpu->logicalDevice, queueData->queueID, queueData->queueIndex, &queue);
		switch (queueData->type)
		{
			case AstralVulkanQueue_Compute:
			{
				gpu->DedicatedComputeQueue = AstralVulkanCommandQueue(queue);
				break;
			}
			case AstralVulkanQueue_Transfer:
			{
				gpu->DedicatedTransferQueue = AstralVulkanCommandQueue(queue);
				break;
			}
			default:
			{
				gpu->DedicatedGraphicsQueue = AstralVulkanCommandQueue(queue);
				break;
			}
		}
	}

	createInfos.deinit();
}