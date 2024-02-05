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
#include "Graphics/MemoryAllocation.hpp"

#include "stdlib.h"

#define INSTANCE_COUNT 100000
#define BINDLESS

#ifdef BINDLESS
struct BindlessData
{
	i32 textureID;
	Maths::Matrix4x4 matrix;
};
#endif

string exeLocation;

IAllocator cAllocator;
ArenaAllocator resourcesArena;
AstralCanvas::RenderProgram renderProgram;
AstralCanvas::Shader shader;
AstralCanvas::VertexBuffer vb;
AstralCanvas::IndexBuffer ib;
AstralCanvas::InstanceBuffer instanceBuffer;
AstralCanvas::RenderPipeline pipeline;
AstralCanvas::Texture2D tbh;

#ifdef BINDLESS
AstralCanvas::Texture2D red;
AstralCanvas::Texture2D green;
AstralCanvas::Texture2D blue;
BindlessData *datas;
#else
Maths::Matrix4x4 *matrices;
#endif

AstralCanvas::VertexDeclaration instanceDataBindlessDecl;

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
#ifdef BINDLESS
        instanceDataBindlessDecl = AstralCanvas::VertexDeclaration(
            &resourcesArena.asAllocator, 
            sizeof(BindlessData),
            AstralCanvas::VertexInput_PerInstance);

		instanceDataBindlessDecl.elements.Add({AstralCanvas::VertexElement_Int, 0});
		instanceDataBindlessDecl.elements.Add({AstralCanvas::VertexElement_Vector4, 16});
        instanceDataBindlessDecl.elements.Add({AstralCanvas::VertexElement_Vector4, 32});
        instanceDataBindlessDecl.elements.Add({AstralCanvas::VertexElement_Vector4, 48});
        instanceDataBindlessDecl.elements.Add({AstralCanvas::VertexElement_Vector4, 64});
#endif

	string filePath = exeLocation.Clone(&cAllocator);
	#ifdef BINDLESS
	filePath.Append("/BindlessTexturing.shaderobj");
	#else
	filePath.Append(INSTANCE_COUNT > 1 ? "/Instancing.shaderobj" : "/Texture.shaderobj");
	#endif

	string fileContents = io::ReadFile(&cAllocator, filePath.buffer);
	filePath.deinit();

	if (AstralCanvas::CreateShaderFromString(&resourcesArena.asAllocator, AstralCanvas::ShaderType_VertexFragment, fileContents, &shader) != 0)
	{
		printf("Failed to create shader!\n");
		return;
	}

	collections::Array<AstralCanvas::VertexDeclaration *> vertexDeclsUsed;

	#ifdef BINDLESS
		vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 2);
		vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorTextureDecl();
		vertexDeclsUsed.data[1] = &instanceDataBindlessDecl;
	#else
	if (INSTANCE_COUNT > 1)
	{
		vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 2);
		vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorTextureDecl();
		vertexDeclsUsed.data[1] = AstralCanvas::GetInstanceDataMatrixDecl();
	}
	else
	{
		vertexDeclsUsed = collections::Array<AstralCanvas::VertexDeclaration*>(&resourcesArena.asAllocator, 1);
		vertexDeclsUsed.data[0] = AstralCanvas::GetVertexPositionColorTextureDecl();
	}
	#endif

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

	#ifdef BINDLESS
	filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/RedSlime.png");
    red = AstralCanvas::CreateTextureFromFile(filePath.buffer);
	filePath.deinit();

	filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/GreenSlime.png");
    green = AstralCanvas::CreateTextureFromFile(filePath.buffer);
	filePath.deinit();

	filePath = exeLocation.Clone(&cAllocator);
	filePath.Append("/BlueSlime.png");
    blue = AstralCanvas::CreateTextureFromFile(filePath.buffer);
	filePath.deinit();
	#endif

	vb = AstralCanvas::VertexBuffer(AstralCanvas::GetVertexPositionColorTextureDecl(), 4);

    float width = 26;//1.625f;
    float height = 18;//1.125f;

	AstralCanvas::VertexPositionColorTexture vertices[4];
	vertices[0].position = Maths::Vec3(-width * 0.5f, -height * 0.5f, 0.0f);
	vertices[0].color = COLOR_WHITE.ToVector4();
	vertices[0].UV = Maths::Vec2(0.0f, 0.0f);

	vertices[1].position = Maths::Vec3(width * 0.5f, -height * 0.5f, 0.0f);
	vertices[1].color = COLOR_WHITE.ToVector4();
	vertices[1].UV = Maths::Vec2(1.0f, 0.0f);

	vertices[2].position = Maths::Vec3(width * 0.5f, height * 0.5f, 0.0f);
	vertices[2].color = COLOR_WHITE.ToVector4();
	vertices[2].UV = Maths::Vec2(1.0f, 1.0f);

	vertices[3].position = Maths::Vec3(-width * 0.5f, height * 0.5f, 0.0f);
	vertices[3].color = COLOR_WHITE.ToVector4();
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

	//instancing test
	#ifdef BINDLESS
	if (INSTANCE_COUNT > 1)
	{
		srand(time(NULL));

		instanceBuffer = AstralCanvas::InstanceBuffer(&instanceDataBindlessDecl, INSTANCE_COUNT, true);
		datas = (BindlessData *)resourcesArena.asAllocator.Allocate(sizeof(BindlessData) * INSTANCE_COUNT);
		for (usize i = 0; i < INSTANCE_COUNT; i++)
		{
			float randFloatX = (float)rand() / (float)RAND_MAX;
			float randFloatY = (float)rand() / (float)RAND_MAX;
			datas[i] = {(i32)i % 4, Maths::Matrix4x4::CreateTranslation(Maths::Lerp(1024.0f * -0.5f, 1024.0f * 0.5f, randFloatX), Maths::Lerp(768.0f * -0.5f, 768.0f * 0.5f, randFloatY), 0.0f)};
		}
		instanceBuffer.SetData(datas, INSTANCE_COUNT);
	}
	#else
	if (INSTANCE_COUNT > 1)
	{
		srand(time(NULL));

		instanceBuffer = AstralCanvas::InstanceBuffer(AstralCanvas::GetInstanceDataMatrixDecl(), INSTANCE_COUNT, true);
		matrices = (Maths::Matrix4x4 *)resourcesArena.asAllocator.Allocate(sizeof(Maths::Matrix4x4) * INSTANCE_COUNT);
		for (usize i = 0; i < INSTANCE_COUNT; i++)
		{
			float randFloatX = (float)rand() / (float)RAND_MAX;
			float randFloatY = (float)rand() / (float)RAND_MAX;
			matrices[i] = Maths::Matrix4x4::CreateTranslation(Maths::Lerp(1024.0f * -0.5f, 1024.0f * 0.5f, randFloatX), Maths::Lerp(768.0f * -0.5f, 768.0f * 0.5f, randFloatY), 0.0f);
		}
		instanceBuffer.SetData(matrices, INSTANCE_COUNT);
	}
	#endif

	renderProgram = AstralCanvas::RenderProgram(&resourcesArena.asAllocator);
	i32 color = renderProgram.AddAttachment(AstralCanvas::ImageFormat_BackbufferFormat, true, false, AstralCanvas::RenderPassOutput_ToWindow);
#ifdef MACOS
    i32 depth = -1;
#else
    i32 depth = renderProgram.AddAttachment(AstralCanvas::ImageFormat_Depth32, false, true, AstralCanvas::RenderPassOutput_ToWindow);
#endif
    renderProgram.AddRenderPass(&resourcesArena.asAllocator, color, depth);

	renderProgram.Construct();
}
void Deinitialize()
{
	tbh.deinit();

	//test
	if (INSTANCE_COUNT > 1)
	{
		instanceBuffer.deinit();
	}

	#ifdef BINDLESS
	red.deinit();
	green.deinit();
	blue.deinit();
	#endif

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
		x += deltaTime * 128.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_A))
	{
		x -= deltaTime * 128.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_W))
	{
		y -= deltaTime * 128.0f;
	}
	if (AstralCanvas::Input_IsKeyDown(AstralCanvas::Keys_S))
	{
		y += deltaTime * 128.0f;
	}
	timer += deltaTime;
	if (timer >= 1.0f)
	{
		printf("fps: %f\n", 1.0f / deltaTime);
		timer = 0.0f;
	}
}
void Draw(float time)
{
	//CreateRotationZ(Degree2Radian * 90.0f)
    Maths::Point2 resolution = AstralCanvas::GetAppInstance()->windows.ptr[0].resolution;
    Maths::Matrix4x4 projMatrix = Maths::Matrix4x4::CreateOrthographic(resolution.X, resolution.Y, 0.0f, 1.0f);
	Maths::Matrix4x4 viewMatrix = Maths::Matrix4x4::Identity();
	#ifdef MACOS
	viewMatrix = Maths::Matrix4x4::CreateScale(1.0f, -1.0f, 1.0f);
	#endif

	AstralCanvas::Application* app = AstralCanvas::GetAppInstance();
	app->graphicsDevice.StartRenderProgram(&renderProgram, AstralCanvas::Color(128, 128, 128));
    
	app->graphicsDevice.UseRenderPipeline(&pipeline);

	app->graphicsDevice.SetVertexBuffer(&vb, 0);
	if (INSTANCE_COUNT > 1)
	{
		app->graphicsDevice.SetVertexBuffer(&instanceBuffer, 1);
	}
	app->graphicsDevice.SetIndexBuffer(&ib);

	WVP matrices = WVP(Maths::Matrix4x4::CreateTranslation(x, y, 0.0f), viewMatrix, projMatrix);
	app->graphicsDevice.SetShaderVariable("Matrices", &matrices, sizeof(WVP));
	app->graphicsDevice.SetShaderVariableSampler("samplerState", AstralCanvas::SamplerGetPointClamp());

	AstralCanvas::Texture2D *textures[4] = {&tbh, &red, &green, &blue};
	#ifdef BINDLESS
	app->graphicsDevice.SetShaderVariableTextures("inputTexture", textures, 4);
	#else
	app->graphicsDevice.SetShaderVariableTexture("inputTexture", &tbh);
	#endif

    app->graphicsDevice.DrawIndexedPrimitives(6, INSTANCE_COUNT);

	app->graphicsDevice.EndRenderProgram();

	shader.descriptorForThisDrawCall = 0;
}

i32 main(i32 argc, const char** argv)
{
	cAllocator = GetCAllocator();
	resourcesArena = ArenaAllocator(&cAllocator);
	exeLocation = string(&resourcesArena.asAllocator, argv[0]);
	exeLocation = path::GetDirectory(&resourcesArena.asAllocator, exeLocation);

	string appName = string(&cAllocator, "test");
	string engineName = string(&cAllocator, "Astral Gametech");
	AstralCanvas::Application* appPtr = AstralCanvas::ApplicationInit(&cAllocator, appName, engineName, 0, 0, 0.0f);
	appPtr->AddWindow(1600, 900, -1, true);
	appPtr->Run(&Update, &Draw, &Initialize, &Deinitialize);

	resourcesArena.deinit();
	appName.deinit();
	engineName.deinit();
	return 0;
}
