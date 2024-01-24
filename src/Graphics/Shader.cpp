#include "Graphics/Shader.hpp"
#include "ArenaAllocator.hpp"
#include "Json.hpp"

using namespace SomnialJson;

#ifdef ASTRALCANVAS_WGPU
#include "Graphics/WGPU/WgpuEngine.hpp"

i32 AstralCanvas_CreateShaderFromString(IAllocator *allocator, AstralCanvas_ShaderType shaderType, string jsonString, AstralCanvasShader* result)
{
    *result = AstralCanvasShader(allocator, shaderType);
    ArenaAllocator localArena = ArenaAllocator(allocator);

    JsonElement root;
    usize parseJsonResult = ParseJsonDocument(&localArena.asAllocator, jsonString, &root);
    if (parseJsonResult != 0)
    {
        localArena.deinit();
        return (i32)parseJsonResult;
    }

    JsonElement *computeElement = root.GetProperty("compute");

    if (computeElement != NULL)
    {

    }
    else
    {
        JsonElement *vertexElement = root.GetProperty("vertex");
        JsonElement *fragmentElement = root.GetProperty("fragment");

        if (vertexElement != NULL && fragmentElement != NULL)
        {
            JsonElement *vertexSpirv = vertexElement->GetProperty("spirv");
            JsonElement *fragmentSpirv = fragmentElement->GetProperty("spirv");

            collections::Array<u32> vertexSpirvData = collections::Array<u32>(&localArena.asAllocator, vertexSpirv->arrayElements.length);
            collections::Array<u32> fragmentSpirvData = collections::Array<u32>(&localArena.asAllocator, fragmentSpirv->arrayElements.length);

            for (usize i = 0; i < vertexSpirv->arrayElements.length; i++)
            {
                vertexSpirvData.data[i] = vertexSpirv->arrayElements.data[i].GetUint32();
            }
            for (usize i = 0; i < fragmentSpirv->arrayElements.length; i++)
            {
                fragmentSpirvData.data[i] = fragmentSpirv->arrayElements.data[i].GetUint32();
            }
            WGPUDevice device = AstralCanvasWgpu_GetEngineInstance()->device;

            WGPUChainedStruct spirvDescriptorType;
            spirvDescriptorType.sType = WGPUSType_ShaderModuleSPIRVDescriptor;
            spirvDescriptorType.next = NULL;

            WGPUShaderModuleSPIRVDescriptor vertexSpirvDescriptor;
            vertexSpirvDescriptor.codeSize = (u32)vertexSpirvData.length;
            vertexSpirvDescriptor.code = vertexSpirvData.data;
            vertexSpirvDescriptor.chain = spirvDescriptorType;

            WGPUShaderModuleDescriptor vertexDescriptor;
            vertexDescriptor.label = "vertex";
            vertexDescriptor.hintCount = 0;
            vertexDescriptor.nextInChain = (WGPUChainedStruct *)&vertexSpirvDescriptor;

            result->shaderModule1 = wgpuDeviceCreateShaderModule(device, &vertexDescriptor);

            WGPUShaderModuleSPIRVDescriptor fragmentSpirvDescriptor;
            fragmentSpirvDescriptor.codeSize = (u32)fragmentSpirvData.length;
            fragmentSpirvDescriptor.code = fragmentSpirvData.data;
            fragmentSpirvDescriptor.chain = spirvDescriptorType;

            WGPUShaderModuleDescriptor fragmentDescriptor;
            fragmentDescriptor.label = "fragment";
            fragmentDescriptor.hintCount = 0;
            fragmentDescriptor.nextInChain = (WGPUChainedStruct *)&fragmentSpirvDescriptor;

            result->shaderModule2 = wgpuDeviceCreateShaderModule(device, &fragmentDescriptor);

        }
        else
        {
            localArena.deinit();
            return -1;
        }
    }

    localArena.deinit();
    return 0;
}
#endif

AstralCanvasShader::AstralCanvasShader()
{
    shaderType = AstralCanvasShader_VertexFragment;
    shaderModule1 = NULL;
    shaderModule2 = NULL;
    shaderPipelineLayout1 = NULL;
    shaderPipelineLayout2 = NULL;
    shaderVariables = AstralCanvasShaderVariables();
}
AstralCanvasShader::AstralCanvasShader(IAllocator *allocator, AstralCanvas_ShaderType type)
{
    shaderType = type;
    shaderModule1 = NULL;
    shaderModule2 = NULL;
    shaderPipelineLayout1 = NULL;
    shaderPipelineLayout2 = NULL;
    shaderVariables = AstralCanvasShaderVariables(allocator);
}
void AstralCanvasShader::deinit()
{
    if (this->shaderModule1 != NULL) 
    {
        
    }
        //wgpuShaderModuleRelease(this->shaderModule1);
    if (this->shaderModule2 != NULL) 
    {

    }
        //wgpuShaderModuleRelease(this->shaderModule2);
    if (this->shaderPipelineLayout1 != NULL)
    {

    }
        //wgpuPipelineLayoutRelease(this->shaderPipelineLayout1);
    if (this->shaderPipelineLayout2 != NULL)
    {

    }
        //wgpuPipelineLayoutRelease(this->shaderPipelineLayout2);

    this->shaderVariables.deinit();
}