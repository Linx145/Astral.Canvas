#include "Application.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"

AstralCanvasApplication AstralCanvas_AppInstance;

AstralCanvasApplication* AstralCanvas_GetAppInstance()
{
	return &AstralCanvas_AppInstance;
}
AstralCanvasApplication::AstralCanvasApplication()
{
}
AstralCanvasApplication* AstralCanvasApplication::init(IAllocator* allocator, AstralGraphicsBackend graphicsBackendToUse, string appName, string engineName, u32 appVersion, u32 engineVersion)
{
	AstralCanvasApplication result;
	result.allocator = allocator;
	result.graphicsBackend = graphicsBackendToUse;
	result.memoryForBuffersMiB = 4;
	result.memoryForImagesMiB = 64;
	result.onInitialize = NULL;
	result.onUpdate = NULL;
	result.windows = vector<AstralCanvasWindow>(allocator);
	result.appName = appName;
	result.engineName = engineName;
	result.appVersion = appVersion;
	result.engineVersion = engineVersion;
	AstralCanvas_AppInstance = result;
	return &AstralCanvas_AppInstance;
}
bool AstralCanvasApplication::AddWindow(i32 width, i32 height, i32 fps, bool resizeable)
{
	option<AstralCanvasWindow> result = AstralCanvasWindow::init(width, height, fps, resizeable);
	if (result.present)
	{
		windows.Add(result.value);
		return true;
	}
	else return false;
}
bool AstralCanvasApplication::FinalizeGraphicsBackend()
{
	if (graphicsBackend == Astral_Vulkan)
	{
#ifdef MACOS
		//On a mac, don't even need to check because it won't work
		graphicsBackend = Astral_Metal;
		return FinalizeGraphicsBackend();
#endif
		//check if vulkan can run, failing that, fallback to WGPU

		Array<const char*> validationLayersToUse = Array<const char*>();
		AstralCanvasVk_validationLayers = false;
#if DEBUG
		AstralCanvasVk_validationLayers = true;
		validationLayersToUse = Array<const char*>(allocator, 1);
		validationLayersToUse.data[0] = "VK_LAYER_KHRONOS_validation";
#endif
		if (!AstralCanvasVk_Initialize(allocator, validationLayersToUse, this->appName, this->engineName, this->appVersion, this->engineVersion))
		{
			graphicsBackend = Astral_WGPU;
			return FinalizeGraphicsBackend();
		}

		validationLayersToUse.deinit();

		return true;
	}
	else if (graphicsBackend == Astral_Metal)
	{
#ifdef MACOS && (WINDOWS || LINUX)
#error("More than 1 backend defined")
#endif

#ifdef WINDOWS || LINUX
		graphicsBackend = Astral_Vulkan;
		return FinalizeGraphicsBackend();
#endif

		THROW_ERR("Attempting to initialize Metal, which has not been implemented!");
	}
	else
	{
		THROW_ERR("Attempting to initialize WGPU, which has not been implemented!");
		return false;
	}
}
void AstralCanvasApplication::Run(AstralCanvas_Update updateFunc, AstralCanvas_Init initFunc)
{
	FinalizeGraphicsBackend();

	bool shouldStop = false;
	while (!shouldStop)
	{
		shouldStop = true;
		for (usize i = 0; i < windows.count; i++)
		{
			if (!glfwWindowShouldClose(windows.ptr[i].handle))
			{
				shouldStop = false;
			}
		}

		glfwPollEvents();
	}
	switch (graphicsBackend)
	{
	case Astral_Vulkan:
		AstralCanvasVk_Deinitialize(this->allocator);
		break;
	default:
		break;
	}
}