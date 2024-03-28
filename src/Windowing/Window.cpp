#include "Windowing/Window.hpp"
#include <stdio.h>
#include "GLFW/glfw3.h"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanSwapchain.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#endif

using namespace Maths;

GLFWmonitor* GetCurrentMonitor(GLFWwindow *window)
{
    i32 nmonitors, i;
    i32 wx, wy, ww, wh;
    i32 mx, my, mw, mh;
    i32 overlap, bestoverlap;
	GLFWmonitor *bestmonitor = NULL;
	GLFWmonitor **monitors;
    const GLFWvidmode *mode;

    bestoverlap = 0;
    bestmonitor = NULL;

    glfwGetWindowPos(window, &wx, &wy);
    glfwGetWindowSize(window, &ww, &wh);
    monitors = glfwGetMonitors(&nmonitors);

    for (i = 0; i < nmonitors; i++) 
	{
        mode = glfwGetVideoMode(monitors[i]);
        glfwGetMonitorPos(monitors[i], &mx, &my);
        mw = mode->width;
        mh = mode->height;

        overlap =
            fmaxl(0, fminl(wx + ww, mx + mw) - fmaxl(wx, mx)) *
            fmaxl(0, fminl(wy + wh, my + mh) - fmaxl(wy, my));

        if (bestoverlap < overlap) {
            bestoverlap = overlap;
            bestmonitor = monitors[i];
        }
    }

    return bestmonitor;
}

namespace AstralCanvas
{
	bool windowLibraryInitialized;

	Window::Window()
	{
		this->handle = NULL;
		this->position = Point2(0);
		this->resolution = Point2(0);
		this->windowSurfaceHandle = NULL;
		this->windowInputState = {};
		this->onTextInputFunc = NULL;
		this->onKeyInteractFunc = NULL;
		this->swapchain = NULL;
		this->isDisposed = false;
		this->justResized = NULL;
	}

	void Window::deinit()
	{
		if (!isDisposed)
		{
			switch (AstralCanvas::GetActiveBackend())
			{
				#ifdef ASTRALCANVAS_VULKAN
				case AstralCanvas::Backend_Vulkan:
				{
					if (swapchain != NULL)
					{
						AstralCanvasVk_DestroySwapchain((AstralVulkanSwapchain *)swapchain);
						free(swapchain);
						vkDestroySurfaceKHR(AstralCanvasVk_GetInstance(), (VkSurfaceKHR)this->windowSurfaceHandle, NULL);
						this->swapchain = NULL;
						this->windowSurfaceHandle = NULL;
					}
					break;
				}
				#endif
				#ifdef ASTRALCANVAS_METAL
				case AstralCanvas::Backend_Metal:
				{
					break;
				}
				#endif
			}
			glfwDestroyWindow((GLFWwindow*)this->handle);
			this->handle = NULL;
			isDisposed = true;
		}
	}
	void Window::CloseWindow()
	{
		glfwSetWindowShouldClose((GLFWwindow *)this->handle, GLFW_TRUE);
		deinit();
	}
	void Window::SetMouseVisible(bool value)
	{
		if (value)
		{
			glfwSetInputMode((GLFWwindow *)handle, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else 
			glfwSetInputMode((GLFWwindow *)handle, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	void Window::SetMouseIcon(void *iconData, u32 iconWidth, u32 iconHeight, i32 originX, i32 originY)
	{
		if (this->customCursorHandle == NULL)
		{
			glfwDestroyCursor((GLFWcursor*)this->customCursorHandle);
			this->customCursorHandle = NULL;
		}
		if (iconData != NULL)
		{
			GLFWimage image;
			image.pixels = (u8 *)iconData;
			image.width = iconWidth;
			image.height = iconHeight;
			this->customCursorHandle = glfwCreateCursor(&image, originX, originY);
			glfwSetCursor((GLFWwindow *)this->handle, (GLFWcursor *)this->customCursorHandle);
		}
		else
		{
			glfwSetCursor((GLFWwindow *)this->handle, NULL);
		}
	}
	/// Called when the window is minimized, otherwise known as iconified
	void WindowMinimized(GLFWwindow* window, i32 iconified)
	{
		if (iconified)
		{
			Window *canvas = (Window*)glfwGetWindowUserPointer(window);
			canvas->windowInputState.ClearAllInputStates();
		}
	}
	/// Called when the window is maximized or restored to original size
	void WindowMaximized(GLFWwindow* window, i32 maximized)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		glfwGetWindowSize(window, &canvas->resolution.X, &canvas->resolution.Y);
	}
	void OnTextInput(GLFWwindow* window, u32 characterUnicode)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.textInputCharacter = characterUnicode;

		if (canvas->onTextInputFunc != NULL)
		{
			canvas->onTextInputFunc(canvas, characterUnicode);
		}
	}
	void OnKeyInteracted(GLFWwindow* window, i32 glfwKey, i32 scancode, i32 action, i32 mods)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		if (glfwKey == GLFW_KEY_UNKNOWN)
		{
			return;
		}
		AstralCanvas::Keys key = (AstralCanvas::Keys)glfwKey;
		if (action == GLFW_PRESS)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Pressed;
			status.status = true;
			canvas->windowInputState.keyStatuses.Insert(glfwKey, status);
		}
		else if (action == GLFW_RELEASE)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Released;
			status.status = false;
			canvas->windowInputState.keyStatuses.Insert(glfwKey, status);
		}
		if (key == GLFW_KEY_BACKSPACE)
		{
			if (action == GLFW_PRESS || action == GLFW_REPEAT)
			{
				canvas->windowInputState.textInputCharacter = '\b';
			}
		}
		if (canvas->onKeyInteractFunc != NULL)
		{
			canvas->onKeyInteractFunc(canvas, key, action);
		}
	}
	void OnMouseInteracted(GLFWwindow *window, i32 button, i32 action, i32 mods)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);

		AstralCanvas::MouseButtons btn = MouseButton_Other;
		if (button == GLFW_MOUSE_BUTTON_LEFT)
		{
			btn = MouseButton_Left;
		}
		else if (button == GLFW_MOUSE_BUTTON_RIGHT)
		{
			btn = MouseButton_Right;
		}
		else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
		{
			btn = MouseButton_Middle;
		}

		if (action == GLFW_PRESS)
		{
			if (canvas->windowInputState.mouseStatuses.Get(btn) == NULL || !canvas->windowInputState.mouseStatuses.Get(btn)->status)
			{
				AstralCanvas::KeyStateStatus status;
				status.perFrameState = AstralCanvas::KeyState_Pressed;
				status.status = true;
				canvas->windowInputState.mouseStatuses.Insert(btn, status);
			}
		}
		else if (action == GLFW_RELEASE)
		{
			if (canvas->windowInputState.mouseStatuses.Get(btn) != NULL && canvas->windowInputState.mouseStatuses.Get(btn)->status)
			{
				AstralCanvas::KeyStateStatus status;
				status.perFrameState = AstralCanvas::KeyState_Released;
				status.status = false;
				canvas->windowInputState.mouseStatuses.Insert(btn, status);
			}
		}
	}
	void OnMouseScrolled(GLFWwindow *window, double xPos, double yPos)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.scroll = Maths::Vec2((float)xPos, (float)yPos);
	}
	void OnCursorMoved(GLFWwindow *window, double xPos, double yPos)
	{
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.mousePosition = Maths::Vec2((float)xPos, (float)yPos);
	}
    void WindowSizeChanged(GLFWwindow *window, i32 width, i32 height)
    {
	}
	void WindowFramebufferSizeChanged(GLFWwindow* window, i32 width, i32 height)
	{
		string str = string(GetCAllocator(), "Framebuffer size changed: ");
		str.Append((i64)width);
		str.Append(", ");
		str.Append((i64)height);
		LOG_WARNING(str.buffer);
		str.deinit();
		Window *canvas = (Window*)glfwGetWindowUserPointer(window);
        canvas->resolution.X = width;
        canvas->resolution.Y = height;
		if (canvas->justResized != NULL)
		{
			*canvas->justResized = true;
		}
	}

	bool WindowInit(IAllocator allocator, const char *name, Window * result, i32 width, i32 height, bool resizeable, void *iconData, u32 iconWidth, u32 iconHeight)
	{
		*result = {};
		if (!windowLibraryInitialized)
		{
			if (!glfwInit())
			{
				return false;
			}
			windowLibraryInitialized = true;
		}
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_RESIZABLE, resizeable);
		
		GLFWwindow* handle = glfwCreateWindow(width, height, name, NULL, NULL);

		if (iconData != NULL)
		{
			GLFWimage image;
			image.width = iconWidth;
			image.height = iconHeight;
			image.pixels = (u8 *)iconData;
			glfwSetWindowIcon(handle, 1, &image);
		}

		if (handle)
		{
			*result = Window();
			result->windowInputState = AstralCanvas::InputState(allocator);
			result->handle = handle;
			result->resolution = Point2(width, height);

			glfwSetWindowIconifyCallback(handle, &WindowMinimized);
			glfwSetWindowMaximizeCallback(handle, &WindowMaximized);
            glfwSetWindowSizeCallback(handle, &WindowSizeChanged);
			glfwSetFramebufferSizeCallback(handle, &WindowFramebufferSizeChanged);
			glfwSetCharCallback(handle, &OnTextInput);
			glfwSetKeyCallback(handle, &OnKeyInteracted);
			glfwSetMouseButtonCallback(handle, &OnMouseInteracted);
			glfwSetScrollCallback(handle, &OnMouseScrolled);
			glfwSetCursorPosCallback(handle, &OnCursorMoved);
			glfwGetWindowPos(handle, &result->position.Y, &result->position.Y);

			return true;
		}

		return false;
	}

	void Window::SetWindowTitle(string title)
	{
		glfwSetWindowTitle((GLFWwindow*)handle, title.buffer);
		windowTitle = title;
	}
	i32 Window::GetCurrentMonitorFramerate()
	{
		GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow*)handle);
		const GLFWvidmode *vidmode = glfwGetVideoMode(monitor);

		return vidmode->refreshRate;
	}
	void Window::SetFullscreen(bool value)
	{
		if (value)
		{
			GLFWmonitor *monitor = GetCurrentMonitor((GLFWwindow *)handle);
			if (monitor == NULL)
			{
				monitor = glfwGetPrimaryMonitor();
			}
			i32 xpos;
			i32 ypos;
			i32 w;
			i32 h;
			glfwGetMonitorWorkarea(monitor, &xpos, &ypos, &w, &h);

			const GLFWvidmode *videoMode = glfwGetVideoMode(monitor);

			glfwSetWindowMonitor((GLFWwindow*)handle, monitor, xpos, ypos, w, h, videoMode->refreshRate);
			resolution.X = videoMode->width;
			resolution.Y = videoMode->height;
		}
		else
		{
			glfwSetWindowMonitor((GLFWwindow*)handle, NULL, position.X, position.Y, resolution.X, resolution.Y, GLFW_DONT_CARE);
		}
		isFullscreen = value;
	}
}
