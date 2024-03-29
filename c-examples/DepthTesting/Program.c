#include <stdbool.h>
#include "stdlib.h"
#include "stdio.h"
#include "Linxc.h"
#include "Astral.Canvas/Application.h"
#include "string.h"

typedef struct
{
    float X;
    float Y;
    float Z;
} Vector3;
typedef struct
{
    float X;
    float Y;
    float Z;
    float W;
} Vector4;
typedef struct
{
    Vector4 position;
    Vector4 color;
} VertexPositionColor;

AstralCanvasVertexBuffer vertexBuffer1;
AstralCanvasVertexBuffer vertexBuffer2;
AstralCanvasIndexBuffer indexBuffer;
AstralCanvasShader shader;
AstralCanvasRenderPipeline renderPipeline;
AstralCanvasRenderProgram renderProgram;
AstralCanvasTexture2D texture;

void Update(float deltaTime)
{

}
void Draw(float deltaTime)
{
    AstralCanvasGraphics graphicsDevice = AstralCanvasApplication_GetGraphicsDevice(AstralCanvasApplication_GetInstance());

    AstralCanvasColor clearColor = {
        .R = 0,
        .G = 0,
        .B = 0,
        .A = 255
    };
    AstralCanvasGraphics_StartRenderProgram(graphicsDevice, renderProgram, clearColor);

    AstralCanvasGraphics_UseRenderPipeline(graphicsDevice, renderPipeline);

    AstralCanvasGraphics_SetVertexBuffer(graphicsDevice, vertexBuffer1, 0);
    AstralCanvasGraphics_SetIndexBuffer(graphicsDevice, indexBuffer);

    AstralCanvasGraphics_DrawIndexedPrimitives(graphicsDevice, 3, 1, 0, 0, 0);

    AstralCanvasGraphics_SetVertexBuffer(graphicsDevice, vertexBuffer2, 0);
    AstralCanvasGraphics_DrawIndexedPrimitives(graphicsDevice, 3, 1, 0, 0, 0);

    AstralCanvasGraphics_EndRenderProgram(graphicsDevice);
}
char* ReadFile(const char* path)
{
    char *result = NULL;
    FILE *fs = fopen(path, "r");
    if (fs != NULL)
    {
        usize size = 0;
        while (fgetc(fs) != EOF)
        {
            size += 1;
        }
        fseek(fs, 0, SEEK_SET);

        result = (char *)malloc(size + 1);
        if (result != NULL)
        {
            fread(result, sizeof(char), size, fs);
            result[size] = '\0';
        }

        fclose(fs);
    }
    return result;
}
void Initialize()
{
    vertexBuffer1 = AstralCanvasVertexBuffer_Create(AstralCanvasGetVertexPositionColorDecl(), 3, false, false);
    vertexBuffer2 = AstralCanvasVertexBuffer_Create(AstralCanvasGetVertexPositionColorDecl(), 3, false, false);
    
    indexBuffer = AstralCanvasIndexBuffer_Create(AstralCanvas_IndexBufferSize_U16, 3);

    VertexPositionColor vertices1[3] =
    {
        {
            {-1.0f, 1.0f, 0.0f, 0.0f}, //position xyz + padding
            {1.0f, 0.0f, 0.0f, 1.0f} //color rgba
        },
        {
            {1.0f, 1.0f, 0.0f, 0.0f}, //position xyz + padding
            {0.0f, 1.0f, 0.0f, 1.0f} //color rgba
        },
        {
            {0.0f, -1.0f, 0.0f, 0.0f}, //position xyz + padding
            {0.0f, 0.0f, 1.0f, 1.0f} //color rgba
        }
    };
    AstralCanvasVertexBuffer_SetData(vertexBuffer1, vertices1, 3);

    VertexPositionColor vertices2[3] =
    {
        {
            {-1.0f, 0.0f, 1.0f, 0.0f}, //position xyz + padding
            {1.0f, 1.0f, 1.0f, 1.0f} //color rgba
        },
        {
            {1.0f, -1.0f, 1.0f, 0.0f}, //position xyz + padding
            {1.0f, 1.0f, 1.0f, 1.0f} //color rgba
        },
        {
            {1.0f, 1.0f, 1.0f, 0.0f}, //position xyz + padding
            {1.0f, 1.0f, 1.0f, 1.0f} //color rgba
        }
    };
    AstralCanvasVertexBuffer_SetData(vertexBuffer2, vertices2, 3);

    u16 indices[] = {0, 1, 2};
    AstralCanvasIndexBuffer_SetData(indexBuffer, (u8*)indices, sizeof(u16) * 3);

    char *fileData = ReadFile("Triangle.shaderobj");

    if (fileData == NULL)
    {
        fprintf(stderr, "Failed to load file\n");
        exit(1);
    }

    int errorCode = AstralCanvasShader_FromString(AstralCanvas_ShaderType_VertexFragment, fileData, &shader);
    if (errorCode != 0)
    {
        fprintf(stderr, "Failed to load shader, error: %i\n", errorCode);
        exit(1);
    }
    free(fileData);

    //alpha blend state
    AstralCanvasBlendState blendState = {
        .srcColor = AstralCanvas_Blend_One,
        .srcAlpha = AstralCanvas_Blend_One,
        .destColor = AstralCanvas_Blend_InverseSourceAlpha,
        .destAlpha = AstralCanvas_Blend_InverseSourceAlpha};
    AstralCanvasVertexDeclaration vertexDecls = AstralCanvasGetVertexPositionColorDecl();
    renderPipeline = AstralCanvasRenderPipeline_Init(shader, AstralCanvas_CullMode_CullNone, AstralCanvas_PrimitiveType_TriangleList, blendState, true, true, &vertexDecls, 1);

    renderProgram = AstralCanvasRenderProgram_Init();
    int outputAttachmentIndex = AstralCanvasRenderProgram_AddAttachment(renderProgram, AstralCanvas_ImageFormat_BackbufferFormat, true, false, AstralCanvas_RenderPassOutput_ToWindow);
    int outputDepthIndex = AstralCanvasRenderProgram_AddAttachment(renderProgram, AstralCanvas_ImageFormat_Depth32, false, true, AstralCanvas_RenderPassOutput_ToWindow);
    AstralCanvasRenderProgram_AddRenderPass(renderProgram, outputAttachmentIndex, outputDepthIndex);
    AstralCanvasRenderProgram_Construct(renderProgram);
    
    texture = AstralCanvasTexture2D_FromFile("tbh.png");
    printf("texture loaded\n");
}
void Deinitialize()
{
    AstralCanvasRenderProgram_Deinit(renderProgram);

    AstralCanvasRenderPipeline_Deinit(renderPipeline);

    AstralCanvasShader_Deinit(shader);
    
    AstralCanvasTexture2D_Deinit(texture);

    AstralCanvasVertexBuffer_Deinit(vertexBuffer2);
    AstralCanvasVertexBuffer_Deinit(vertexBuffer1);

    AstralCanvasIndexBuffer_Deinit(indexBuffer);
}
int main()
{
    AstralCanvasApplication application = AstralCanvasApplication_Init("Triangle Demo", "My Engine", 0, 0, 0.0f);
    AstralCanvasApplication_AddWindow(application, 1024, 768, true);
    AstralCanvasApplication_Run(application, &Update, &Draw, &Initialize, &Deinitialize);
}
