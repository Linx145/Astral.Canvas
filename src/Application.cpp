#include "Application.hpp"
#include "Graphics/WGPU/WgpuEngine.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "Graphics/Metal/MetalEngine.h"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"
#include "array.hpp"
#include "Input/Input.hpp"
using namespace collections;

namespace AstralCanvas
{
	Application AstralCanvas_AppInstance;
	float refreshTimer = 0.0f;

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
		result.windows = vector<Window>(allocator);
		result.appName = appName;
		result.engineName = engineName;
		result.appVersion = appVersion;
		result.engineVersion = engineVersion;
		AstralCanvas_AppInstance = result;
		return &AstralCanvas_AppInstance;
	}
	bool Application::AddWindow(const char *name, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight)
	{
		Window result;
		glfwWindowHint(GLFW_REFRESH_RATE, (i32)this->framesPerSecond);
		if (WindowInit(this->allocator, name, &result, width, height, resizeable, iconData, iconWidth, iconHeight))
		{
			windows.Add(result);
			glfwSetWindowUserPointer((GLFWwindow*)result.handle, &windows.ptr[windows.count - 1]);
			return true;
		}
		return false;
	}
	bool Application::FinalizeGraphicsBackend()
	{
        switch (GetActiveBackend())
        {
#ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
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

				for (usize i = 0; i < this->windows.count; i++)
				{
					AstralCanvasVk_InitializeFor(this->allocator, validationLayersToUse, requiredExtensions, this->windows.Get(i));
				}
				//AstralCanvasWgpu_Initialize(this->allocator, &this->windows.ptr[0], Array<AstralCanvas_GraphicsFeatures>(), Array<AstralCanvas_GraphicsFeatures>());
                this->graphicsDevice = AstralCanvas::Graphics();
				break;
            }
#endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_Initialize(this->allocator, &windows.ptr[0]);
                break;
            }
#endif
            default:
                THROW_ERR("Unrecognised backend");
        }
        this->graphicsDevice.usedShaders = collections::hashset<AstralCanvas::Shader*>(this->allocator, &PointerHash<AstralCanvas::Shader>, &PointerEql<AstralCanvas::Shader>);
		return true;
	}
	void Application::ResetDeltaTimer()
	{
		glfwSetTime(0.0);
		shouldResetDeltaTimer = true;
	}
	void Application::Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc)
	{
		FinalizeGraphicsBackend();
		if (initFunc != NULL)
		{
			initFunc();
		}
		startTime = (float)glfwGetTime();
		endTime = startTime;

		bool shouldStop = false;
		while (!shouldStop)
		{
			float deltaTime = endTime - startTime;

			bool slowdownPollEvents = true;

			for (usize i = 0; i < windows.count; i++)
			{
				if (windows.ptr[i].resolution.X != 0 && windows.ptr[0].resolution.Y != 0)
				{
					slowdownPollEvents = false;
				}
			}
			if (slowdownPollEvents)
			{
				while (windows.ptr[0].resolution.X == 0 || windows.ptr[0].resolution.Y == 0)
				{
					deltaTime = endTime - startTime;
					refreshTimer += deltaTime;
					if (refreshTimer >= 0.03f)
					{
						refreshTimer = 0.0f;
						glfwPollEvents();
					}
					startTime = endTime;
					endTime = (float)glfwGetTime();
				}
			}
			else
			{
				refreshTimer = 0.0f;
				glfwPollEvents();
			}

			if (framesPerSecond < 1.0f || deltaTime >= 1.0f / framesPerSecond)
			{
				updateFunc(deltaTime);

				for (usize i = 0; i < windows.count; i++)
				{
					if (windows.ptr[i].resolution.X == 0 || windows.ptr[i].resolution.Y == 0)
					{
						continue;
					}
					if (windows.ptr[i].handle != NULL && !glfwWindowShouldClose((GLFWwindow*)windows.ptr[i].handle))
					{
						windows.ptr[i].windowInputState.ResetPerFrameInputStates();
					}
					else
					{
						windows.ptr[i].deinit();
						windows.RemoveAt_Swap(i);
						continue;
					}

					this->graphicsDevice.currentWindow = &this->windows.ptr[i];
					this->graphicsDevice.ClipArea = this->windows.ptr[i].AsRectangle();
					this->graphicsDevice.Viewport = this->windows.ptr[i].AsRectangle();
					
					bool shouldContinue = true;
					switch (AstralCanvas::GetActiveBackend())
					{
#ifdef ASTRALCANVAS_VULKAN
						case AstralCanvas::Backend_Vulkan:
						{
							shouldContinue = AstralCanvasVk_BeginDraw(this->graphicsDevice.currentWindow);
							break;
						}
#endif
#ifdef ASTRALCANVAS_METAL
						case AstralCanvas::Backend_Metal:
						{
							AstralCanvasMetal_BeginDraw();
							break;
						}
#endif
						default:
							break;
					}
					if (shouldContinue)
					{

						drawFunc(deltaTime);

						graphicsDevice.currentRenderPass = 0;
						graphicsDevice.currentRenderPipeline = NULL;
						graphicsDevice.currentRenderProgram = NULL;
						graphicsDevice.currentRenderTarget = NULL;
						for (usize i = 0; i < graphicsDevice.usedShaders.bucketsCount; i++)
						{
							if (graphicsDevice.usedShaders.buckets[i].initialized)
							{
								for (usize j = 0; j < graphicsDevice.usedShaders.buckets[i].entries.count; j++)
								{
									graphicsDevice.usedShaders.buckets[i].entries.ptr[j]->descriptorForThisDrawCall = 0;
								}
								graphicsDevice.usedShaders.buckets[i].entries.Clear();
							}
						}
						switch (AstralCanvas::GetActiveBackend())
						{
							#ifdef ASTRALCANVAS_VULKAN
							case AstralCanvas::Backend_Vulkan:
							{
								AstralCanvasVk_EndDraw(windows.Get(i));
								break;
							}
							#endif
		#ifdef ASTRALCANVAS_METAL
							case AstralCanvas::Backend_Metal:
							{
								AstralCanvasMetal_EndDraw();
								break;
							}
		#endif
							default:
								break;
						}
						if (postEndDrawFunc != NULL)
						{
							postEndDrawFunc(deltaTime);
						}
					}
				}
				if (windows.count == 0)
				{
					break;
				}
				startTime = endTime;
			}
			endTime = (float)glfwGetTime();
			if (this->shouldResetDeltaTimer)
			{
				startTime = endTime;
				this->shouldResetDeltaTimer = false;
			}
		}

		this->graphicsDevice.usedShaders.deinit();

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
        switch (AstralCanvas::GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case AstralCanvas::Backend_Vulkan:
            {
				AstralCanvasVk_Deinitialize(allocator, windows.ptr, windows.count);
				break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case AstralCanvas::Backend_Metal:
            {
                break;
            }
#endif
            default:
                break;
        }
	}
}
