#pragma once
#include "Graphics/ComputeBuffer.hpp"
#include "Graphics/Shader.hpp"

namespace AstralCanvas
{
    struct ComputePipeline
    {
        void *handle;
        Shader *shader;
#ifdef ASTRALCANVAS_VULKAN
        VkPipelineLayout layout;
#endif
        ComputePipeline();
        ComputePipeline(Shader *computeShader);
        void Construct();
        void deinit();
        void DispatchNow(i32 threadsX, i32 threadsY, i32 threadsZ);
    };
}