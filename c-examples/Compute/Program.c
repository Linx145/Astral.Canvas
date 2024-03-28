#include <stdbool.h>
#include "stdlib.h"
#include "stdio.h"
#include "Linxc.h"
#include "Astral.Canvas/Application.h"
#include "Astral.Canvas/Graphics/Compute.h"
#include "string.h"
#include "Astral.Canvas/Input.h"

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
    Vector4 velocity;
} Particle;

#define PARTICLES_COUNT 256

AstralCanvasShader computeShader = NULL;
bool onBufferB = false;
AstralCanvasComputePipeline computePipeline = NULL;
AstralCanvasComputeBuffer computeBufferA = NULL;
AstralCanvasComputeBuffer computeBufferB = NULL;
AstralCanvasRenderProgram renderProgram = NULL;

void Update(float deltaTime)
{
    if (AstralCanvasInput_IsKeyPressed(AstralCanvas_Keys_Space))
    {
        float fauxDeltaTime = 1.0f;
        AstralCanvasComputeBuffer from = computeBufferA;
        AstralCanvasComputeBuffer to = computeBufferB;
        if (onBufferB)
        {
            from = computeBufferB;
            to = computeBufferA;
        }
        onBufferB = !onBufferB;

        AstralCanvasShader_SetShaderVariableComputeBuffer(computeShader, "ParticlesIn", from);
        AstralCanvasShader_SetShaderVariableComputeBuffer(computeShader, "ParticlesOut", to);
        AstralCanvasShader_SetShaderVariable(computeShader, "TimeData", &fauxDeltaTime, sizeof(float));

        AstralCanvasComputePipeline_DispatchNow(computePipeline, PARTICLES_COUNT, 1, 1);

        usize dataLength;
        Particle* data = (Particle*)AstralCanvasComputeBuffer_GetData(to, &dataLength);
        printf("Particle 0 position.X = %f, velocity.X = %f\n", data[0].position.X, data[0].velocity.X);
        free(data);
    }
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
    char *fileData = ReadFile("C:/Users/Linus/source/repos/Astral.Canvas/c-examples/Compute/Particles.shaderobj");

    if (fileData == NULL)
    {
        fprintf(stderr, "Failed to load file\n");
        exit(1);
    }

    int errorCode = AstralCanvasShader_FromString(AstralCanvas_ShaderType_VertexFragment, fileData, &computeShader);
    if (errorCode != 0 || computeShader == NULL)
    {
        fprintf(stderr, "Failed to load shader, error: %i\n", errorCode);
        exit(1);
    }
    free(fileData);

    computePipeline = AstralCanvasComputePipeline_Create(computeShader);
    printf("Created compute pipeline\n");

    computeBufferA = AstralCanvasComputeBuffer_Create(sizeof(Particle), PARTICLES_COUNT, false, true);
    computeBufferB = AstralCanvasComputeBuffer_Create(sizeof(Particle), PARTICLES_COUNT, false, true);

    printf("Created compute buffers\n");
    Particle *particleArray = malloc(sizeof(Particle) * PARTICLES_COUNT);
    for (usize i = 0; i < PARTICLES_COUNT; i++)
    {
        particleArray[i].position.X = i;
        particleArray[i].position.Y = 0.0f;
        particleArray[i].position.Z = 0.0f;
        particleArray[i].position.W = 0.0f;

        particleArray[i].velocity.X = 1.0f;
        particleArray[i].velocity.Y = 0.0f;
        particleArray[i].velocity.Z = 0.0f;
        particleArray[i].velocity.W = 0.0f;
    }
    AstralCanvasComputeBuffer_SetData(computeBufferA, particleArray, PARTICLES_COUNT);
    free(particleArray);
    printf("Set compute buffers\n");

    renderProgram = AstralCanvasRenderProgram_Init();
    int outputAttachmentIndex = AstralCanvasRenderProgram_AddAttachment(renderProgram, AstralCanvas_ImageFormat_BackbufferFormat, true, false, AstralCanvas_RenderPassOutput_ToWindow);
    int outputDepthIndex = AstralCanvasRenderProgram_AddAttachment(renderProgram, AstralCanvas_ImageFormat_Depth32, false, true, AstralCanvas_RenderPassOutput_ToWindow);
    AstralCanvasRenderProgram_AddRenderPass(renderProgram, outputAttachmentIndex, outputDepthIndex);
    AstralCanvasRenderProgram_Construct(renderProgram);
}
void Deinitialize()
{
    AstralCanvasRenderProgram_Deinit(renderProgram);

    AstralCanvasComputeBuffer_Deinit(computeBufferA);
    AstralCanvasComputeBuffer_Deinit(computeBufferB);
    AstralCanvasComputePipeline_Deinit(computePipeline);

    AstralCanvasShader_Deinit(computeShader);
}
int main()
{
    AstralCanvasApplication application = AstralCanvasApplication_Init("Compute Demo", "My Engine", 0, 0, 0.0f);
    AstralCanvasApplication_AddWindow(application, "Compute (Press Space)", 1024, 768, true, NULL, 0, 0);
    AstralCanvasApplication_Run(application, &Update, &Draw, NULL, &Initialize, &Deinitialize);
}
