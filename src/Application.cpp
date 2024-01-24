#include "Application.hpp"
#include "Graphics/WGPU/WgpuEngine.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "array.hpp"
using namespace collections;

AstralCanvasApplication AstralCanvas_AppInstance;

AstralCanvasApplication* AstralCanvas_GetAppInstance()
{
	return &AstralCanvas_AppInstance;
}
AstralCanvasApplication::AstralCanvasApplication()
{
}
AstralCanvasApplication* AstralCanvasApplication::init(IAllocator* allocator, string appName, string engineName, u32 appVersion, u32 engineVersion)
{
	AstralCanvasApplication result;
	result.allocator = allocator;
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
	#if DEBUG
	collections::Array<const char *> validationLayersToUse = collections::Array<const char *>(this->allocator, 1);
	validationLayersToUse.data[0] = "VK_LAYER_KHRONOS_validation";
	#endif
	#if NDEBUG
	collections::Array<const char *> validationLayersToUse = collections::Array<const char *>();
	#endif

	collections::Array<const char *> requiredExtensions = collections::Array<const char *>();

	AstralCanvasVk_Initialize(this->allocator, validationLayersToUse, requiredExtensions, &windows.ptr[0]);
	//AstralCanvasWgpu_Initialize(this->allocator, &this->windows.ptr[0], Array<AstralCanvas_GraphicsFeatures>(), Array<AstralCanvas_GraphicsFeatures>());
	return true;
}
void AstralCanvasApplication::Run(AstralCanvas_Update updateFunc, AstralCanvas_Init initFunc, AstralCanvas_Deinit deinitFunc)
{
	FinalizeGraphicsBackend();
	if (initFunc != NULL)
	{
		initFunc();
	}

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
	if (deinitFunc != NULL)
	{
		deinitFunc();
	}
	AstralCanvasVk_Deinitialize(allocator, &windows.ptr[0]);
	//AstralCanvasWgpu_Deinit();
}