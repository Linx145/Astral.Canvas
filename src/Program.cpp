#include <Application.hpp>
#include <allocators.hpp>
#include "ArenaAllocator.hpp"
#include "Json.hpp"
#include "time.h"
#include "Graphics/Graphics.hpp"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/BlendState.hpp"

IAllocator cAllocator;
ArenaAllocator resourcesArena;
//AstralCanvas::RenderProgram renderProgram;
AstralCanvas::RenderProgram renderProgram;
AstralCanvas::Shader shader;
AstralCanvas::VertexBuffer vb;
AstralCanvas::IndexBuffer ib;
AstralCanvas::RenderPipeline pipeline;

void Initialize()
{
	string fileContents = io::ReadFile(&cAllocator, "C:/Users/Linus/source/repos/SomnialGametech/TestContent/Triangle.shaderobj");

	if (AstralCanvas::CreateShaderFromString(&resourcesArena.asAllocator, AstralCanvas::ShaderType_VertexFragment, fileContents, &shader) != 0)
	{
		printf("Failed to create shader!\n");
		return;
	}

	collections::Array<AstralCanvas::VertexDeclaration*> vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 1);
	vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorDecl();

	pipeline = AstralCanvas::RenderPipeline(
		&resourcesArena.asAllocator,
		&shader,
		AstralCanvas::CullMode_CullNone,
		AstralCanvas::PrimitiveType_TriangleList,
		OPAQUE_BLEND,
		false,
		false,
		vertexDeclsUsed
	); 
	// RenderPipeline(&resourcesArena.asAllocator, &shader, AstralCanvas::CullMode_CullNone, AstralCanvas::PrimitiveType_TriangleList, OPAQUE_BLEND, false, false, vertexDeclsUsed);

	vb = AstralCanvas::VertexBuffer(AstralCanvas::GetVertexPositionColorDecl(), 3);

	AstralCanvas::VertexPositionColor vertices[3];
	vertices[0].position = Maths::Vec3(0.0f, -1.0f, 0.0f);
	vertices[0].color = COLOR_RED.ToVector4();
	vertices[1].position = Maths::Vec3(1.0f, 1.0f, 0.0f);
	vertices[1].color = COLOR_GREEN.ToVector4();
	vertices[2].position = Maths::Vec3(-1.0f, 1.0f, 0.0f);
	vertices[2].color = COLOR_BLUE.ToVector4();

	vb.SetData(vertices, 3);

	ib = AstralCanvas::IndexBuffer(AstralCanvas::IndexBufferSize_U16, 3);
	u16 indices[3];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	ib.SetData((u8*)indices, sizeof(u16) * 3);

	renderProgram = AstralCanvas::RenderProgram(&resourcesArena.asAllocator);
	i32 color = renderProgram.AddAttachment(AstralCanvas::ImageFormat_BackbufferFormat, true, false, AstralCanvas::RenderPassOutput_ToWindow);
	i32 depth = renderProgram.AddAttachment(AstralCanvas::ImageFormat_Depth32, false, true, AstralCanvas::RenderPassOutput_ToWindow);
	renderProgram.AddRenderPass(&resourcesArena.asAllocator, color, depth);

	renderProgram.Construct();

	fileContents.deinit();

	// usize dataLength = 0;
	// AstralCanvas::VertexPositionColor *readResults = (AstralCanvas::VertexPositionColor*)vb.GetData(&cAllocator, &dataLength);
	// for (usize i = 0; i < 3; i++)
	// {
	// 	printf("xyzw: %f, %f, %f, %f\n", readResults[i].color.X, readResults[i].color.Y, readResults[i].color.Z, readResults[i].color.W);
	// }
	// free(readResults);
}
void Deinitialize()
{
	renderProgram.deinit();

	ib.deinit();

	vb.deinit();

	pipeline.deinit();

	shader.deinit();
}
void Update(float time)
{

}
void Draw(float time)
{
	AstralCanvasApplication* app = AstralCanvas_GetAppInstance();
	app->graphicsDevice.StartRenderProgram(&renderProgram, COLOR_BLACK);

	app->graphicsDevice.SetVertexBuffer(&vb, 0);
	app->graphicsDevice.SetIndexBuffer(&ib);

	app->graphicsDevice.UseRenderPipeline(&pipeline);

	app->graphicsDevice.DrawIndexedPrimitives(3, 1);

	app->graphicsDevice.EndRenderProgram();
}

i32 main()
{
	cAllocator = GetCAllocator();
	resourcesArena = ArenaAllocator(&cAllocator);
	string appName = string(&cAllocator, "test");
	string engineName = string(&cAllocator, "AstralGametech");
	AstralCanvasApplication* appPtr = AstralCanvasApplication::init(&cAllocator, appName, engineName, 0, 0);
	appPtr->AddWindow(1920, 1080, -1, true);
	appPtr->Run(&Update, &Draw, &Initialize, &Deinitialize);

	resourcesArena.deinit();
	appName.deinit();
	engineName.deinit();
	return 0;
}