#include "Graphics/RenderPipeline.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ErrorHandling.hpp"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanImplementations.hpp"
#endif

namespace AstralCanvas
{
    RenderPipeline::RenderPipeline(IAllocator *allocator, Shader *pipelineShader, CullMode pipelineCullMode, PrimitiveType pipelinePrimitiveType, BlendState *pipelineBlendState, bool testDepth, bool writeToDepth, collections::Array<VertexDeclaration> pipelineVertexDeclarations)
    {
        this->shader = pipelineShader;
        this->cullMode = pipelineCullMode;
        this->primitiveType = pipelinePrimitiveType;
        this->blendState = pipelineBlendState;
        this->depthTest = testDepth;
        this->depthWrite = writeToDepth;
        this->vertexDeclarations = pipelineVertexDeclarations;
        this->zoneToPipelineInstance = collections::hashmap<RenderPipelineBindZone, void *>(allocator, &RenderPipelineBindZoneHash, &RenderPipelineBindZoneEql);
    }
    void RenderPipeline::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                AstralCanvasVk_DestroyRenderPipeline(this);
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }
    }
}