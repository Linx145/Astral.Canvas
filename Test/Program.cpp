#include "Application.hpp"
#include "allocators.hpp"
#include "Maths/All.h"
#include "ArenaAllocator.hpp"
#include "Json.hpp"
#include "time.h"
#include "Graphics/Graphics.hpp"
#include "Graphics/VertexDeclarations.hpp"
#include "Graphics/BlendState.hpp"
#include "ErrorHandling.hpp"
#include "Input/Input.hpp"
#include "string.hpp"
#include "path.hpp"

string exeLocation;

IAllocator cAllocator;
ArenaAllocator resourcesArena;
AstralCanvas::RenderProgram renderProgram;
AstralCanvas::Shader shader;
AstralCanvas::VertexBuffer vb;
AstralCanvas::IndexBuffer ib;
AstralCanvas::RenderPipeline pipeline;
AstralCanvas::Texture2D tbh;

struct WVP
{
	Maths::Matrix4x4 world;
	Maths::Matrix4x4 view;
	Maths::Matrix4x4 projection;

	inline WVP(Maths::Matrix4x4 worldMat, Maths::Matrix4x4 viewMat, Maths::Matrix4x4 projMat)
	{
		world = worldMat;
		view = viewMat;
		projection = projMat;
	}
};
void Initialize()
{
	/*string filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/Triangle.shaderobj");
	string fileContents = io::ReadFile(&cAllocator, filePath.buffer);
	filePath.deinit();

	if (AstralCanvas::CreateShaderFromString(&resourcesArena.asAllocator, AstralCanvas::ShaderType_VertexFragment, fileContents, &shader) != 0)
	{
		printf("Failed to create shader!\n");
		return;
	}

	collections::Array<AstralCanvas::VertexDeclaration*> vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 1);
	vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorTextureDecl();

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

	filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/tbh.png");
    tbh = AstralCanvas::CreateTextureFromFile(filePath.buffer);
	filePath.deinit();

	vb = AstralCanvas::VertexBuffer(AstralCanvas::GetVertexPositionColorTextureDecl(), 4);

	float width = 1.625f;
	float height = 1.125f;

	AstralCanvas::VertexPositionColorTexture vertices[4];
	vertices[0].position = Maths::Vec3(-width * 0.5f, -height * 0.5f, 0.0f);
	vertices[0].color = COLOR_RED.ToVector4();
	vertices[0].UV = Maths::Vec2(0.0f, 0.0f);

	vertices[1].position = Maths::Vec3(width * 0.5f, -height * 0.5f, 0.0f);
	vertices[1].color = COLOR_GREEN.ToVector4();
	vertices[1].UV = Maths::Vec2(1.0f, 0.0f);

	vertices[2].position = Maths::Vec3(width * 0.5f, height * 0.5f, 0.0f);
	vertices[2].color = COLOR_BLUE.ToVector4();
	vertices[2].UV = Maths::Vec2(1.0f, 1.0f);

	vertices[3].position = Maths::Vec3(-width * 0.5f, height * 0.5f, 0.0f);
	vertices[3].color = COLOR_BLUE.ToVector4();
	vertices[3].UV = Maths::Vec2(0.0f, 1.0f);

	vb.SetData(vertices, 4);

	ib = AstralCanvas::IndexBuffer(AstralCanvas::IndexBufferSize_U16, 6);
	u16 indices[6];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 0;
	indices[5] = 2;
	ib.SetData((u8*)indices, sizeof(u16) * 6);*/

	string filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/Triangle.shaderobj");
	string fileContents = io::ReadFile(&cAllocator, filePath.buffer);
	filePath.deinit();

	if (AstralCanvas::CreateShaderFromString(&resourcesArena.asAllocator, AstralCanvas::ShaderType_VertexFragment, fileContents, &shader) != 0)
	{
		printf("Failed to create shader!\n");
		return;
	}

	vb = AstralCanvas::VertexBuffer(AstralCanvas::GetVertexPositionColorDecl(), 4);

	float width = 0.5f;
	float height = 0.5f;

	AstralCanvas::VertexPositionColorTexture vertices[4];
	vertices[0].position = Maths::Vec3(-width * 0.5f, -height * 0.5f, 0.0f);
	vertices[0].color = COLOR_RED.ToVector4();
	vertices[0].UV = Maths::Vec2(0.0f, 0.0f);

	vertices[1].position = Maths::Vec3(width * 0.5f, -height * 0.5f, 0.0f);
	vertices[1].color = COLOR_GREEN.ToVector4();
	vertices[1].UV = Maths::Vec2(1.0f, 0.0f);

	vertices[2].position = Maths::Vec3(width * 0.5f, height * 0.5f, 0.0f);
	vertices[2].color = COLOR_BLUE.ToVector4();
	vertices[2].UV = Maths::Vec2(1.0f, 1.0f);

	vertices[3].position = Maths::Vec3(-width * 0.5f, height * 0.5f, 0.0f);
	vertices[3].color = COLOR_BLUE.ToVector4();
	vertices[3].UV = Maths::Vec2(0.0f, 1.0f);

	vb.SetData(vertices, 4);

	ib = AstralCanvas::IndexBuffer(AstralCanvas::IndexBufferSize_U16, 6);
	u16 indices[6];
	indices[0] = 0;
	indices[1] = 1;
	indices[2] = 2;
	indices[3] = 3;
	indices[4] = 0;
	indices[5] = 2;
	ib.SetData((u8*)indices, sizeof(u16) * 6);

	collections::Array<AstralCanvas::VertexDeclaration*> vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 1);
	vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorTextureDecl();

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

	renderProgram = AstralCanvas::RenderProgram(&resourcesArena.asAllocator);
	i32 color = renderProgram.AddAttachment(AstralCanvas::ImageFormat_BackbufferFormat, true, false, AstralCanvas::RenderPassOutput_ToWindow);
	//i32 depth = renderProgram.AddAttachment(AstralCanvas::ImageFormat_Depth32, false, true, AstralCanvas::RenderPassOutput_ToWindow);
	renderProgram.AddRenderPass(&resourcesArena.asAllocator, color, -1);

	renderProgram.Construct();
}
void Deinitialize()
{
	/*tbh.deinit();*/

	ib.deinit();

	vb.deinit();

	pipeline.deinit();

	shader.deinit();

	renderProgram.deinit();
}
float timer = 0.0f;
float x = 0.0f;
float y = 0.0f;
void Update(float deltaTime)
{
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_D))
	{
		x += deltaTime * 5.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_A))
	{
		x -= deltaTime * 5.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_W))
	{
		y -= deltaTime * 5.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_S))
	{
		y += deltaTime * 5.0f;
	}
	//timer += deltaTime;
	if (timer >= 1.0f)
	{
		printf("fps: %f\n", 1.0f / deltaTime);
		timer = 0.0f;
	}
}
void Draw(float time)
{
	//CreateRotationZ(Degree2Radian * 90.0f)
	//Maths::Matrix4x4 projMatrix = Maths::Matrix4x4::CreateOrthographic(40.0f, 22.5f, 0.0f, 1.0f);

	AstralCanvas::Application* app = AstralCanvas::GetAppInstance();
	app->graphicsDevice.StartRenderProgram(&renderProgram, AstralCanvas::Color(128, 128, 128));

	app->graphicsDevice.UseRenderPipeline(&pipeline);

	app->graphicsDevice.SetVertexBuffer(&vb, 0);
	app->graphicsDevice.SetIndexBuffer(&ib);

	// WVP matrices = WVP(Maths::Matrix4x4::CreateTranslation(x, y, 0.0f), Maths::Matrix4x4::Identity(), projMatrix);
	// app->graphicsDevice.SetShaderVariable("Matrices", &matrices, sizeof(WVP));
	// app->graphicsDevice.SetShaderVariableSampler("samplerState", AstralCanvas::SamplerGetPointClamp());
	// app->graphicsDevice.SetShaderVariableTexture("inputTexture", &tbh);

	app->graphicsDevice.DrawIndexedPrimitives(6, 1);

	app->graphicsDevice.EndRenderProgram();

	//shader.descriptorForThisDrawCall = 0;
}

i32 main(i32 argc, const char** argv)
{
	cAllocator = GetCAllocator();
	resourcesArena = ArenaAllocator(&cAllocator);
	exeLocation = string(&resourcesArena.asAllocator, argv[0]);
	exeLocation = path::GetDirectory(&resourcesArena.asAllocator, exeLocation);

	string appName = string(&cAllocator, "test");
	string engineName = string(&cAllocator, "AstralGametech");
	AstralCanvas::Application* appPtr = AstralCanvas::ApplicationInit(&cAllocator, appName, engineName, 0, 0, 60.0f);
	appPtr->AddWindow(1024, 768, -1, true);
	appPtr->Run(&Update, &Draw, &Initialize, &Deinitialize);

	resourcesArena.deinit();
	appName.deinit();
	engineName.deinit();
	return 0;
}
