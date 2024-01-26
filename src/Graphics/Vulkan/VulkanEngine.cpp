#include <Graphics/Vulkan/VulkanEngine.hpp>
#include <Graphics/Vulkan/VulkanInstanceData.hpp>

using namespace collections;


VkBool32 AstralCanvasVk_ErrorCallback(
	VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
	VkDebugUtilsMessageTypeFlagsEXT messageTypes,
	const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
	void* pUserData)
{
	if (messageSeverity | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
	{
		THROW_ERR(pCallbackData->pMessage);
	}
	else
	{
		LOG_WARNING(pCallbackData->pMessage);
	}
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

bool AstralCanvasVk_Initialize(IAllocator* allocator, Array<const char*> validationLayersToUse, Array<const char*> requiredExtensions, AstralCanvasWindow* window)
{
	if (!AstralCanvasVk_CreateInstance(allocator, validationLayersToUse, "", "", 0, 0))
	{
		return false;
	}
	printf("Created vulkan instance\n");

	if (glfwCreateWindowSurface(AstralCanvasVk_GetInstance(), window->handle, NULL, (VkSurfaceKHR*)&window->windowSurfaceHandle) != VK_SUCCESS)
	{
		printf("Failed to create window surface!\n");
		return false;
	}
	printf("Created window surface\n");
	option<AstralVulkanGPU> gpu = AstralCanvasVk_SelectGPU(allocator, AstralCanvasVk_GetInstance(), (VkSurfaceKHR)window->windowSurfaceHandle, requiredExtensions);
	if (!gpu.present)
	{
		return false;
	}
	AstralCanvasVk_SetCurrentGPU(gpu.value);
	printf("Created GPU interface\n");

	VmaVulkanFunctions vulkanAllocatorFunctions = {};
	vulkanAllocatorFunctions.vkGetInstanceProcAddr = &vkGetInstanceProcAddr;
	vulkanAllocatorFunctions.vkGetDeviceProcAddr = &vkGetDeviceProcAddr;

	VmaAllocator vulkanAllocator;
	VmaAllocatorCreateInfo allocatorCreateInfo = {};
	allocatorCreateInfo.device = gpu.value.logicalDevice;
	allocatorCreateInfo.instance = AstralCanvasVk_GetInstance();
	allocatorCreateInfo.physicalDevice = gpu.value.physicalDevice;
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

void AstralCanvasVk_Deinitialize(IAllocator* allocator, AstralCanvasWindow* window)
{
	VmaAllocator vma = AstralCanvasVk_GetCurrentVulkanAllocator();
	if (vma != NULL)
	{
		vmaDestroyAllocator(vma);
	}
	
	VkInstance instance = AstralCanvasVk_GetInstance();
	AstralCanvasVk_GetCurrentGPU()->deinit();

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