#include <Application.hpp>
#include <allocators.hpp>
#include "ArenaAllocator.hpp"
#include "Json.hpp"
#include "time.h"
#include "Graphics/Shader.hpp"

void Initialize()
{
	/*IAllocator cAllocator = GetCAllocator();
	string fileContents = io::ReadFile(&cAllocator, "C:/Users/Linus/source/repos/SomnialGametech/TestContent/Triangle.shaderobj");
	if (fileContents.buffer != NULL)
	{
		i32 errorCode = AstralCanvas_CreateShaderFromString(&cAllocator, AstralCanvasShader_VertexFragment, fileContents, &shader);
		if (errorCode != 0)
		{
			fprintf(stderr, "Failed to load shader: Error code %i\n", errorCode);
		}
		else printf("Created shader successfully\n");
	}
	else
		fprintf(stderr, "Failed to load file\n");

	fileContents.deinit();*/
}
void Deinitialize()
{
}

i32 main()
{
	IAllocator defaultAllocator = GetCAllocator();
	string appName = string(&defaultAllocator, "test");
	string engineName = string(&defaultAllocator, "AstralGametech");
	AstralCanvasApplication* appPtr = AstralCanvasApplication::init(&defaultAllocator, appName, engineName, 0, 0);
	appPtr->AddWindow(1920, 1080, -1, true);
	appPtr->Run(NULL, &Initialize, &Deinitialize);

	appName.deinit();
	engineName.deinit();
	return 0;
}