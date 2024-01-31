#include "Application.hpp"
#include "Graphics/WGPU/WgpuEngine.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "array.hpp"
using namespace collections;

namespace AstralCanvas
{
	Application AstralCanvas_AppInstance;

	Application* GetAppInstance()
	{
		return &AstralCanvas_AppInstance;
	}
	Application::Application()
	{
	}
	Application* ApplicationInit(IAllocator* allocator, string appName, string engineName, u32 appVersion, u32 engineVersion, float framesPerSecond)
	{
		Application result;
		result.framesPerSecond = framesPerSecond;
		result.allocator = allocator;
		result.onInitialize = NULL;
		result.onUpdate = NULL;
		result.windows = vector<Window>(allocator);
		result.appName = appName;
		result.engineName = engineName;
		result.appVersion = appVersion;
		result.engineVersion = engineVersion;
		AstralCanvas_AppInstance = result;
		return &AstralCanvas_AppInstance;
	}
	bool Application::AddWindow(i32 width, i32 height, i32 fps, bool resizeable)
	{
		Window result;
		if (WindowInit(this->allocator, &result, width, height, fps, resizeable))
		{
			windows.Add(result);
			glfwSetWindowUserPointer(result.handle, &windows.ptr[windows.count - 1]);
			return true;
		}
		return false;
	}
	bool Application::FinalizeGraphicsBackend()
	{
		#if DEBUG
		collections::Array<const char *> validationLayersToUse = collections::Array<const char *>(this->allocator, 1);
		validationLayersToUse.data[0] = "VK_LAYER_KHRONOS_validation";
		#endif
		#if NDEBUG
		collections::Array<const char *> validationLayersToUse = collections::Array<const char *>();
		#endif

		collections::Array<const char *> requiredExtensions = collections::Array<const char *>(allocator, 1);
		requiredExtensions.data[0] = VK_KHR_SWAPCHAIN_EXTENSION_NAME;

		AstralCanvasVk_Initialize(this->allocator, validationLayersToUse, requiredExtensions, &windows.ptr[0]);
		//AstralCanvasWgpu_Initialize(this->allocator, &this->windows.ptr[0], Array<AstralCanvas_GraphicsFeatures>(), Array<AstralCanvas_GraphicsFeatures>());
		this->graphicsDevice = AstralCanvas::Graphics();
		this->graphicsDevice.ClipArea = this->windows.ptr[0].AsRectangle();
		this->graphicsDevice.Viewport = this->windows.ptr[0].AsRectangle();
		return true;
	}
	void Application::Run(AstralCanvas_Update updateFunc, AstralCanvas_Update drawFunc, AstralCanvas_Init initFunc, AstralCanvas_Deinit deinitFunc)
	{
		FinalizeGraphicsBackend();
		if (initFunc != NULL)
		{
			initFunc();
		}
		float startTime = (float)glfwGetTime();
		float endTime = startTime;

		bool shouldStop = false;
		while (!shouldStop)
		{
			glfwPollEvents();

			float deltaTime = endTime - startTime;

			shouldStop = true;
			for (usize i = 0; i < windows.count; i++)
			{
				if (!glfwWindowShouldClose(windows.ptr[i].handle))
				{
					windows.ptr[i].windowInputState.ResetPerFrameInputStates();
					shouldStop = false;
				}
			}

			if (framesPerSecond < 1.0f || deltaTime >= 1.0f / framesPerSecond)
			{
				updateFunc(deltaTime);

				switch (AstralCanvas::GetActiveBackend())
				{
					#ifdef ASTRALCANVAS_VULKAN
					case AstralCanvas::Backend_Vulkan:
					{
						AstralCanvasVk_BeginDraw();
						break;
					}
					#endif
					default:
						break;
				}

				drawFunc(deltaTime);

				graphicsDevice.currentRenderPass = 0;
				graphicsDevice.currentRenderPipeline = NULL;
				graphicsDevice.currentRenderProgram = NULL;
				graphicsDevice.currentRenderTarget = NULL;
				switch (AstralCanvas::GetActiveBackend())
				{
					#ifdef ASTRALCANVAS_VULKAN
					case AstralCanvas::Backend_Vulkan:
					{
						AstralCanvasVk_EndDraw();
						break;
					}
					#endif
					default:
						break;
				}
				startTime = endTime;
			}
			endTime = (float)glfwGetTime();
		}

		//await rendering process shutdown
		switch (AstralCanvas::GetActiveBackend())
		{
			#ifdef ASTRALCANVAS_VULKAN
			case AstralCanvas::Backend_Vulkan:
			{
				AstralCanvasVk_AwaitShutdown();
				//vkQueueWaitIdle(gpu->DedicatedGraphicsQueue.queue);
				break;
			}
			#endif
			default:
				break;
		}

		if (deinitFunc != NULL)
		{
			deinitFunc();
		}
		AstralCanvasVk_Deinitialize(allocator, &windows.ptr[0]);
		//AstralCanvasWgpu_Deinit();
	}
}