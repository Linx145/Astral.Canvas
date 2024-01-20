#include "Graphics/Vulkan/VulkanGPU.hpp"
#include "array.hpp"
using namespace collections;

bool AstralCanvasVk_SelectGPU(IAllocator *allocator, VkInstance instance)
{
	VkPhysicalDevice device = NULL;
	u32 deviceCount = 0;
	if (vkEnumeratePhysicalDevices(instance, &deviceCount, NULL) != VK_SUCCESS || deviceCount == 0)
	{
		//No GPUs with Vulkan support
		return false;
	}

	Array<VkPhysicalDevice> gpus = Array<VkPhysicalDevice>(allocator, deviceCount);
	vkEnumeratePhysicalDevices(instance, &deviceCount, gpus.data);

	u32 maxScore = 0;
	u32 bestGPU = -1;
	for (usize i = 0; i < gpus.length; i++)
	{
		u32 score = AstralCanvasVk_GetGPUScore(gpus.data[i]);
		if (score >= maxScore)
		{
			bestGPU = i;
		}
	}
	if (bestGPU == -1)
	{
		return false;
	}
	device = gpus.data[bestGPU];

	gpus.deinit();
}

u32 AstralCanvasVk_GetGPUScore(VkPhysicalDevice gpu)
{
	u32 score = 0;
	VkPhysicalDeviceProperties properties;
	VkPhysicalDeviceFeatures features;

	vkGetPhysicalDeviceProperties(gpu, &properties);
	vkGetPhysicalDeviceFeatures(gpu, &features);

	if (properties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)
	{
		score += 5;
	}
}