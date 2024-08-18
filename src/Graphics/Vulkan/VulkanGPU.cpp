#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanGPU.hpp"
#include <Graphics/Vulkan/VulkanSwapchainSupportDetails.hpp>

using namespace collections;

bool AstralCanvasVk_SelectGPU(IAllocator allocator, VkInstance instance, VkSurfaceKHR windowSurface, Array<const char*> requiredExtensions, AstralVulkanGPU* output)
{
	u32 deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, NULL) != VK_SUCCESS || deviceCount == 0)
	{
		//No GPUs with Vulkan support
		return false;
	}

	IAllocator cAllocator = GetCAllocator();

	Array<AstralVulkanGPU> gpus = Array<AstralVulkanGPU>(cAllocator, deviceCount);
	Array<VkPhysicalDevice> physicalDevices = Array<VkPhysicalDevice>(cAllocator, deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data);

	for (usize i = 0; i < physicalDevices.length; i++)
	{
		u32 propertiesCount = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices.data[i], &propertiesCount, NULL);

		//need to allocate with regular allocator just in case we are going to be persistent
		Array<VkQueueFamilyProperties> allFamilyProperties = Array<VkQueueFamilyProperties>(allocator, propertiesCount);
		vkGetPhysicalDeviceQueueFamilyProperties(physicalDevices.data[i], &propertiesCount, allFamilyProperties.data);

		gpus.data[i] = AstralVulkanGPU(requiredExtensions, physicalDevices.data[i], AstralVulkanQueueProperties(allocator, allFamilyProperties, windowSurface));
	}
	physicalDevices.deinit();

	i32 maxScore = -1;
	u32 bestGPU = -1;
	for (u32 i = 0; i < gpus.length; i++)
	{
		i32 newScore = AstralCanvasVk_GetGPUScore(&gpus.data[i], windowSurface);
		if (newScore > maxScore)
		{
			bestGPU = i;
		}
	}

	if (bestGPU == -1)
	{
		for (usize i = 0; i < gpus.length; i++)
		{
			gpus.data[i].queueInfo.deinit();
		}
		gpus.deinit();
		return false;
	}
	AstralVulkanGPU result = gpus.data[bestGPU];

	AstralCanvasVk_CreateLogicalDevice(&result, cAllocator);

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

	*output = result;
	return true;
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
	AstralVkSwapchainSupportDetails details = AstralCanvasVk_QuerySwapchainSupport(gpu->physicalDevice, windowSurface, cAllocator);
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

	Array<VkExtensionProperties> properties = Array<VkExtensionProperties>(cAllocator);

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

bool AstralCanvasVk_CreateLogicalDevice(AstralVulkanGPU* gpu, IAllocator allocator)
{
	collections::vector<VkDeviceQueueCreateInfo> createInfos = collections::vector<VkDeviceQueueCreateInfo>(allocator);
	float queuePriorities = 1.0f;

	VkDeviceQueueCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
	createInfo.queueCount = 1;
	createInfo.pQueuePriorities = &queuePriorities;
	createInfo.pNext = NULL;
	createInfo.flags = NULL;

	i32 graphicsQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Graphics);
	printf("Creating graphics queue, index is %i\n", graphicsQueueIndex);
	createInfo.queueFamilyIndex = graphicsQueueIndex;
	createInfos.Add(createInfo);
	
	i32 transferQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Transfer);
	if (transferQueueIndex != graphicsQueueIndex)
	{
		printf("Creating transfer queue, index is %i\n", transferQueueIndex);
		createInfo.queueFamilyIndex = transferQueueIndex;
		createInfos.Add(createInfo);
	}
	
	i32 computeQueueIndex = AstralCanvasVk_GetQueue(&gpu->queueInfo, gpu->physicalDevice, AstralVulkanQueue_Compute);
	if (computeQueueIndex != graphicsQueueIndex)
	{
		printf("Creating compute queue, index is %i\n", computeQueueIndex);
		createInfo.queueFamilyIndex = computeQueueIndex;
		createInfos.Add(createInfo);
	}

	VkPhysicalDeviceFeatures physicalDeviceFeatures = VkPhysicalDeviceFeatures{};
	physicalDeviceFeatures.fillModeNonSolid = true;

	VkDeviceCreateInfo deviceCreateInfo = {};
	deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
	deviceCreateInfo.queueCreateInfoCount = (u32)createInfos.count;
	deviceCreateInfo.pQueueCreateInfos = createInfos.ptr;
	deviceCreateInfo.enabledExtensionCount = (u32)gpu->requiredExtensions.length;
	deviceCreateInfo.ppEnabledExtensionNames = gpu->requiredExtensions.data;
	deviceCreateInfo.enabledLayerCount = 0;
	deviceCreateInfo.ppEnabledLayerNames = NULL;
	deviceCreateInfo.pNext = NULL;
	deviceCreateInfo.pEnabledFeatures = &physicalDeviceFeatures;

	if (vkCreateDevice(gpu->physicalDevice, &deviceCreateInfo, NULL, &gpu->logicalDevice) != VK_SUCCESS)
	{
		return false;
	}
	bool encounteredErrorCreatingCommandPools = false;

	for (usize i = 0; i < gpu->queueInfo.queueCreationResults.count; i++)
	{
		AstralVulkanQueueResult *queueData = gpu->queueInfo.queueCreationResults.Get(i);
		VkQueue queue;
		vkGetDeviceQueue(gpu->logicalDevice, queueData->queueID, queueData->queueIndex, &queue);

		VkCommandPool commandPool;
		VkCommandPoolCreateInfo commandPoolCreateInfo = {};
		commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
		commandPoolCreateInfo.queueFamilyIndex = queueData->queueID;
		commandPoolCreateInfo.pNext = NULL;

		if (vkCreateCommandPool(gpu->logicalDevice, &commandPoolCreateInfo, NULL, &commandPool) != VK_SUCCESS)
		{
			encounteredErrorCreatingCommandPools = true;
		}

		VkFence queueFence;
		VkFenceCreateInfo fenceCreateInfo = {};
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		/*if (queueData->type == AstralVulkanQueue_Graphics)
		{
			fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
		}*/
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		if (vkCreateFence(gpu->logicalDevice, &fenceCreateInfo, NULL, &queueFence))
		{
			encounteredErrorCreatingCommandPools = true;
		}
		switch (queueData->type)
		{
			case AstralVulkanQueue_Compute:
			{
				gpu->DedicatedComputeQueue = AstralCanvasVkCommandQueue(gpu->logicalDevice, queue, queueFence, commandPool);
				break;
			}
			case AstralVulkanQueue_Transfer:
			{
				gpu->DedicatedTransferQueue = AstralCanvasVkCommandQueue(gpu->logicalDevice, queue, queueFence, commandPool);
				break;
			}
			default:
			{
				gpu->DedicatedGraphicsQueue = AstralCanvasVkCommandQueue(gpu->logicalDevice, queue, queueFence, commandPool);
				break;
			}
		}
	}
	if (gpu->DedicatedTransferQueue.queue == NULL)
	{
		gpu->DedicatedTransferQueue = gpu->DedicatedGraphicsQueue;
	}
	if (gpu->DedicatedComputeQueue.queue == NULL)
	{
		gpu->DedicatedComputeQueue = gpu->DedicatedGraphicsQueue;
	}

	createInfos.deinit();

	return !encounteredErrorCreatingCommandPools;
}

void AstralCanvasVk_ReleaseGPU(AstralVulkanGPU *gpu)
{
	if (gpu->DedicatedComputeQueue.queue != gpu->DedicatedGraphicsQueue.queue)
	{
		gpu->DedicatedComputeQueue.deinit();
	}
	if (gpu->DedicatedTransferQueue.queue != gpu->DedicatedGraphicsQueue.queue)
	{
		gpu->DedicatedTransferQueue.deinit();
	}
	gpu->DedicatedGraphicsQueue.deinit();
	if (gpu->logicalDevice != NULL)
	{
		vkDestroyDevice(gpu->logicalDevice, NULL);
	}
	gpu->requiredExtensions.deinit();
}
#endif
