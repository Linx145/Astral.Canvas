#include "Windowing/Window.hpp"

using namespace Maths;

bool AstralCanvasWindow::windowLibraryInitialized = false;

AstralCanvasWindow::AstralCanvasWindow()
{
	this->handle = NULL;
	this->position = Point2(0);
	this->resolution = Point2(0);
	this->windowSurfaceHandle = NULL;
}

option<AstralCanvasWindow> AstralCanvasWindow::init(i32 width, i32 height, i32 fps, bool resizeable)
{
	if (!AstralCanvasWindow::windowLibraryInitialized)
	{
		if (!glfwInit())
		{
			return option<AstralCanvasWindow>();
		}
		AstralCanvasWindow::windowLibraryInitialized = true;
	}
	glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
	glfwWindowHint(GLFW_RESIZABLE, resizeable);
	GLFWwindow* handle = glfwCreateWindow(width, height, "Hello Window", NULL, NULL);

	if (handle)
	{
		AstralCanvasWindow result = AstralCanvasWindow();
		result.handle = handle;
		result.resolution = Point2(width, height);

		glfwGetWindowPos(handle, &result.position.Y, &result.position.Y);

		return option<AstralCanvasWindow>(result);
	}

	return option<AstralCanvasWindow>();
}