#include "Application.hpp"
#include "Graphics/WGPU/WgpuEngine.hpp"
#include "Graphics/Vulkan/VulkanEngine.hpp"
#include "Graphics/Metal/MetalEngine.h"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"
#include "array.hpp"
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
	bool Application::AddWindow(i32 width, i32 height, bool resizeable)
	{
		Window result;
		if (WindowInit(this->allocator, &result, width, height, resizeable))
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
                
                AstralCanvasVk_Initialize(this->allocator, validationLayersToUse, requiredExtensions, &windows.ptr[0]);
                //AstralCanvasWgpu_Initialize(this->allocator, &this->windows.ptr[0], Array<AstralCanvas_GraphicsFeatures>(), Array<AstralCanvas_GraphicsFeatures>());
                this->graphicsDevice = AstralCanvas::Graphics();
                this->graphicsDevice.ClipArea = this->windows.ptr[0].AsRectangle();
                this->graphicsDevice.Viewport = this->windows.ptr[0].AsRectangle();
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
	void Application::Run(ApplicationUpdateFunction updateFunc, ApplicationUpdateFunction drawFunc, ApplicationUpdateFunction postEndDrawFunc, ApplicationInitFunction initFunc, ApplicationDeinitFunction deinitFunc)
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
			float deltaTime = endTime - startTime;
			
			if (windows.ptr[0].resolution.X == 0 || windows.ptr[0].resolution.Y == 0)
			{
				while (windows.ptr[0].resolution.X == 0 || windows.ptr[0].resolution.Y == 0)
				{
					refreshTimer += deltaTime;
					if (refreshTimer >= 0.03f)
					{
						refreshTimer = 0.0f;
						glfwPollEvents();
					}
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

				switch (AstralCanvas::GetActiveBackend())
				{
					#ifdef ASTRALCANVAS_VULKAN
					case AstralCanvas::Backend_Vulkan:
					{
						AstralCanvasVk_BeginDraw();
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

				shouldStop = true;
				for (usize i = 0; i < windows.count; i++)
				{
					if (!glfwWindowShouldClose((GLFWwindow*)windows.ptr[i].handle))
					{
						windows.ptr[i].windowInputState.ResetPerFrameInputStates();
						shouldStop = false;
					}
				}

				if (shouldStop)
				{
					break;
				}
				graphicsDevice.Viewport.X = 0;
				graphicsDevice.Viewport.Y = 0;
				graphicsDevice.Viewport.Width = windows.ptr[0].resolution.X;
				graphicsDevice.Viewport.Height = windows.ptr[0].resolution.Y;

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
						AstralCanvasVk_EndDraw();
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
				startTime = endTime;
			}
			endTime = (float)glfwGetTime();
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
                AstralCanvasVk_Deinitialize(allocator, &windows.ptr[0]);
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
