#pragma once

#ifdef ASTRALCANVAS_WGPU
#include "wgpu.h"
#include "webgpu-headers/webgpu.h"
#include "Windowing/Window.h"
#include "array.hpp"
#include "Graphics/Features.h"
#include "vector.hpp"
#include "string.hpp"

struct AstralCanvasWgpu_Engine
{
    WGPUInstance instance;
    WGPUSurface surface;
    WGPUSurfaceCapabilities surfaceCapabilities;
    WGPUAdapter adapter;
    WGPUDevice device;
    WGPUSurfaceConfiguration config;
    WGPUQueue queue;

    collections::vector<AstralCanvas_GraphicsFeatures> enabledFeatures;
};

void AstralCanvasWgpu_LogCallback(WGPULogLevel level, char const *message, void *userdata);
void AstralCanvasWgpu_InitLogging(WGPULogLevel logLevel);

void AstralCanvasWgpu_HandleRequestedDevice(WGPURequestDeviceStatus status, WGPUDevice device, char const *message, void *userdata);

AstralCanvasWgpu_Engine* AstralCanvasWgpu_GetEngineInstance();
void AstralCanvasWgpu_Initialize(IAllocator allocator, AstralCanvas::Window* window, collections::Array<AstralCanvas_GraphicsFeatures> requiredFeatures, collections::Array<AstralCanvas_GraphicsFeatures> optionalFeatures);

void AstralCanvasWgpu_ResizeWindow(AstralCanvas::Window *window);

void AstralCanvasWgpu_Deinit();

i32 AstralCanvasWgpu_CreateShaderFromString(IAllocator allocator, ShaderType shaderType, string jsonString, Shader *result);
#endif