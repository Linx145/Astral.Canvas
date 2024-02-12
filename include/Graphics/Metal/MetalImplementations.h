#pragma once
#ifdef ASTRALCANVAS_METAL
#include "Linxc.h"
#include "string.hpp"
#include "Graphics/RenderProgram.hpp"
#include "Graphics/RenderTarget.hpp"
#include "Graphics/RenderPipeline.hpp"
#include "Graphics/VertexBuffer.hpp"
#include "Graphics/InstanceBuffer.hpp"
#include "Graphics/IndexBuffer.hpp"
#include "Graphics/Texture2D.hpp"
#include "Graphics/Color.hpp"
#include "Maths/Rectangle.hpp"

AstralCanvas::ImageFormat AstralCanvasMetal_GetSwapchainFormat();

void AstralCanvasMetal_DestroyShaderProgram(void* vertex, void* fragment);
bool AstralCanvasMetal_CreateShaderProgram(string vertexSource, string fragmentSource, void** vertexOut, void** fragmentOut);

bool AstralCanvasMetal_CreateRenderProgram(AstralCanvas::RenderProgram *program);
void AstralCanvasMetal_DestroyRenderProgram(AstralCanvas::RenderProgram *program);

void* AstralCanvasMetal_StartRenderProgram(AstralCanvas::RenderProgram *program, AstralCanvas::RenderTarget *target, u32 currentRenderPass, AstralCanvas::Color color);
void AstralCanvasMetal_EndRenderProgram(void* encoder);

void *AstralCanvasMetal_CreateRenderPipeline(AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPass);
void AstralCanvasMetal_DestroyRenderPipeline(void *pipeline);
void AstralCanvasMetal_UseRenderPipeline(void *commandEncoder, AstralCanvas::RenderPipeline *pipeline, AstralCanvas::RenderProgram *renderProgram, u32 renderPassToUse, Maths::Rectangle viewport, Maths::Rectangle clipArea);

void AstralCanvasMetal_CreateVertexBuffer(AstralCanvas::VertexBuffer *vertexBuffer);
void AstralCanvasMetal_CreateInstanceBuffer(AstralCanvas::InstanceBuffer *instanceBuffer);
void AstralCanvasMetal_SetVertexData(AstralCanvas::VertexBuffer *vertexBuffer, void *data, usize count);
void AstralCanvasMetal_SetInstanceData(AstralCanvas::InstanceBuffer *instanceBuffer, void *data, usize count);
void AstralCanvasMetal_SetVertexBuffer(void *commandEncoder, const AstralCanvas::VertexBuffer *vertexBuffer, u32 bindingPoint);
void AstralCanvasMetal_DestroyVertexBuffer(AstralCanvas::VertexBuffer *vertexBuffer);

void AstralCanvasMetal_CreateUniformBuffer(AstralCanvas::UniformBuffer *uniformBuffer, void* uniformData, usize count);
void AstralCanvasMetal_DestroyUniformBuffer(AstralCanvas::UniformBuffer *uniformBuffer);

void AstralCanvasMetal_CreateIndexBuffer(AstralCanvas::IndexBuffer *indexBuffer, void* indicesData, usize count);
//void AstralCanvasMetal_SetIndexBuffer(void *commandEncoder, const AstralCanvas::IndexBuffer *indexBuffer);
void AstralCanvasMetal_DestroyIndexBuffer(AstralCanvas::IndexBuffer *indexBuffer);

void AstralCanvasMetal_DrawIndexedPrimitives(void *currentCommandEncoderInstance, const AstralCanvas::IndexBuffer *indexBuffer, u32 indexCount, u32 instanceCount, u32 firstIndex, u32 vertexOffset, u32 firstInstance);

void AstralCanvasMetal_CreateTexture(AstralCanvas::Texture2D *texture);
void AstralCanvasMetal_DestroyTexture(AstralCanvas::Texture2D *texture);
void AstralCanvasMetal_CreateSampler(AstralCanvas::SamplerState *sampler);
void AstralCanvasMetal_DestroySampler(AstralCanvas::SamplerState *sampler);

void AstralCanvasMetal_SetClipArea(Maths::Rectangle clipArea);

void AstralCanvasMetal_AddUniformDescriptorSets(AstralCanvas::Shader *shader);
void AstralCanvasMetal_SyncUniformsWithGPU(void *commandEncoder, AstralCanvas::Shader *shader);
#endif
