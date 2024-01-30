#include "Windowing/Window.hpp"

using namespace Maths;

bool AstralCanvasWindow::windowLibraryInitialized = false;

AstralCanvasWindow::AstralCanvasWindow()
{
	this->handle = NULL;
	this->position = Point2(0);
	this->resolution = Point2(0);
	this->windowSurfaceHandle = NULL;
	this->windowInputState = {};
}

/// Called when the window is minimized, otherwise known as iconified
void AstralCanvas_WindowMinimized(GLFWwindow* window, i32 iconified)
{
	if (iconified)
	{
		AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
		canvas->windowInputState.ClearAllInputStates();
	}
}
/// Called when the window is maximized or restored to original size
void AstralCanvas_WindowMaximized(GLFWwindow* window, i32 maximized)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
	glfwGetWindowSize(window, &canvas->resolution.X, &canvas->resolution.Y);
}
void AstralCanvas_OnTextInput(GLFWwindow* window, u32 characterUnicode)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
	canvas->windowInputState.textInputCharacter = characterUnicode;
}
void AstralCanvas_OnKeyInteracted(GLFWwindow* window, i32 glfwKey, i32 scancode, i32 action, i32 mods)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
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
}
void AstralCanvas_OnMouseInteracted(GLFWwindow *window, i32 button, i32 action, i32 mods)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
	if (action == GLFW_PRESS)
	{
		if (canvas->windowInputState.mouseStatuses.Get(button) == NULL || !canvas->windowInputState.mouseStatuses.Get(button)->status)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Pressed;
			status.status = true;
			canvas->windowInputState.mouseStatuses.Insert(button, status);
		}
	}
	else if (action == GLFW_RELEASE)
	{
		if (canvas->windowInputState.mouseStatuses.Get(button) != NULL && canvas->windowInputState.mouseStatuses.Get(button)->status)
		{
			AstralCanvas::KeyStateStatus status;
			status.perFrameState = AstralCanvas::KeyState_Released;
			status.status = false;
			canvas->windowInputState.mouseStatuses.Insert(button, status);
		}
	}
}
void AstralCanvas_OnMouseScrolled(GLFWwindow *window, double xPos, double yPos)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
	canvas->windowInputState.scroll = Maths::Vec2((float)xPos, (float)yPos);
}
void AstralCanvas_OnCursorMoved(GLFWwindow *window, double xPos, double yPos)
{
	AstralCanvasWindow *canvas = (AstralCanvasWindow*)glfwGetWindowUserPointer(window);
	canvas->windowInputState.mousePosition = Maths::Vec2((float)xPos, (float)yPos);
}

bool AstralCanvasWindow_Init(IAllocator *allocator, AstralCanvasWindow * result, i32 width, i32 height, i32 fps, bool resizeable)
{
	*result = {};
	if (!AstralCanvasWindow::windowLibraryInitialized)
	{
		if (!glfwInit())
		{
			return false;
		}
		AstralCanvasWindow::windowLibraryInitialized = true;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizeable);
	GLFWwindow* handle = glfwCreateWindow(width, height, "Hello Window", NULL, NULL);

	if (handle)
	{
		*result = AstralCanvasWindow();
		result->windowInputState = AstralCanvas::InputState(allocator);
		result->handle = handle;
		result->resolution = Point2(width, height);

		glfwSetWindowIconifyCallback(handle, &AstralCanvas_WindowMinimized);
		glfwSetWindowMaximizeCallback(handle, &AstralCanvas_WindowMaximized);
		glfwSetCharCallback(handle, &AstralCanvas_OnTextInput);
		glfwSetKeyCallback(handle, &AstralCanvas_OnKeyInteracted);
		glfwSetMouseButtonCallback(handle, &AstralCanvas_OnMouseInteracted);
		glfwSetScrollCallback(handle, &AstralCanvas_OnMouseScrolled);
		glfwSetCursorPosCallback(handle, &AstralCanvas_OnCursorMoved);
		glfwGetWindowPos(handle, &result->position.Y, &result->position.Y);

		return true;
	}

	return false;
}