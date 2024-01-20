#include <Application.hpp>
#include <allocators.hpp>

using namespace Maths;

i32 main()
{
	IAllocator defaultAllocator = GetCAllocator();
	string appName = string(&defaultAllocator, "test");
	string engineName = string(&defaultAllocator, "AstralGametech");
	AstralCanvasApplication* appPtr = AstralCanvasApplication::init(&defaultAllocator, Astral_Vulkan, appName, engineName, 0, 0);
	appPtr->AddWindow(1920, 1080, -1, true);
	appPtr->Run(NULL, NULL);

	appName.deinit();
	engineName.deinit();
	/*GLFWwindow* handle;
	if (!glfwInit())
	{
		return -1;
	}

	glfwWindowHint(GLFW_NO_API, GL_TRUE);
	handle = glfwCreateWindow(1920, 1080, "Hello Window", NULL, NULL);
	if (!handle)
	{
		glfwTerminate();
		return -1;
	}

    while (!glfwWindowShouldClose(handle))
    {
        glfwPollEvents();
    }

    glfwTerminate();*/
    return 0;
}