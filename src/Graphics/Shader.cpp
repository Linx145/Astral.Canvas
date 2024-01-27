#include "Graphics/Shader.hpp"
#include "Graphics/CurrentBackend.hpp"
#include "ArenaAllocator.hpp"
#include "ErrorHandling.hpp"
#include "Json.hpp"
#include "cmath"

#ifdef ASTRALCANVAS_VULKAN
#include "Graphics/Vulkan/VulkanInstanceData.hpp"
#include "Graphics/Vulkan/VulkanEnumConverters.hpp"
#endif

using namespace SomnialJson;

namespace AstralCanvas
{
    Shader::Shader()
    {
        shaderType = ShaderType_VertexFragment;
        shaderModule1 = NULL;
        shaderModule2 = NULL;
        shaderPipelineLayout = NULL;
        shaderVariables = ShaderVariables();
    }
    Shader::Shader(IAllocator *allocator, ShaderType type)
    {
        shaderType = type;
        shaderModule1 = NULL;
        shaderModule2 = NULL;
        shaderPipelineLayout = NULL;
        shaderVariables = ShaderVariables(allocator);
    }
    void ParseShaderVariables(JsonElement *json, ShaderVariables *results, ShaderInputAccessedBy accessedByShaderOfType)
    {
        JsonElement *uniforms = json->GetProperty("uniforms");
        if (uniforms != NULL)
        {
            for (usize i = 0; i < uniforms->arrayElements.length; i++)
            {
                string name = uniforms->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 stride = uniforms->arrayElements.data[i].GetProperty("stride")->GetUint32();
                u32 set = uniforms->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = uniforms->arrayElements.data[i].GetProperty("binding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                }
                else
                {
                    ShaderResource newResource;
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.size = stride;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.arrayLength = 0;
                    newResource.type = ShaderResourceType_Uniform;
                    results->uniforms.Add(binding, newResource);
                }
                //results->uniforms.Add(binding, {name, set, binding, stride});
            }
        }
        JsonElement *textures = json->GetProperty("images");
        if (textures != NULL)
        {
            for (usize i = 0; i < textures->arrayElements.length; i++)
            {
                string name = textures->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = textures->arrayElements.data[i].GetProperty("arrayLength")->GetUint32();
                u32 set = textures->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = textures->arrayElements.data[i].GetProperty("binding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                }
                else
                {
                    ShaderResource newResource;
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.arrayLength = arrayLength;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.type = ShaderResourceType_Texture;
                    newResource.size = 0;
                    results->uniforms.Add(binding, newResource);
                }
            }
        }
        JsonElement *samplers = json->GetProperty("samplers");
        if (samplers != NULL)
        {
            for (usize i = 0; i < samplers->arrayElements.length; i++)
            {
                string name = samplers->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 arrayLength = samplers->arrayElements.data[i].GetProperty("arrayLength")->GetUint32();
                u32 set = samplers->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = samplers->arrayElements.data[i].GetProperty("binding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                }
                else
                {
                    ShaderResource newResource;
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.arrayLength = arrayLength;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.type = ShaderResourceType_Sampler;
                    newResource.size = 0;
                    results->uniforms.Add(binding, newResource);
                }
            }
        }
    }
    void Shader::deinit()
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                VkDevice logicalDevice = AstralCanvasVk_GetCurrentGPU()->logicalDevice;
                
                if (this->shaderPipelineLayout != NULL)
                {
                    vkDestroyDescriptorSetLayout(logicalDevice, (VkDescriptorSetLayout)this->shaderPipelineLayout, NULL);
                }
                if (this->shaderModule1 != NULL)
                {
                    vkDestroyShaderModule(logicalDevice, (VkShaderModule)this->shaderModule1, NULL);
                }
                if (this->shaderModule2 != NULL)
                {
                    vkDestroyShaderModule(logicalDevice, (VkShaderModule)this->shaderModule2, NULL);
                }
                if (this->shaderPipelineLayout != NULL)
                {
                    vkDestroyDescriptorSetLayout(logicalDevice, (VkDescriptorSetLayout)this->shaderPipelineLayout, NULL);
                }
                this->shaderVariables.deinit();
                break;
            }
            #endif
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }

        this->shaderVariables.deinit();
    }
    i32 AstralCanvas_CreateShaderFromString(IAllocator *allocator, ShaderType shaderType, string jsonString, Shader* result)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                *result = AstralCanvas::Shader(allocator, shaderType);
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
                    ParseShaderVariables(computeElement, &result->shaderVariables, InputAccessedBy_Compute);
                }
                else
                {
                    JsonElement *vertexElement = root.GetProperty("vertex");
                    JsonElement *fragmentElement = root.GetProperty("fragment");

                    if (vertexElement != NULL && fragmentElement != NULL)
                    {
                        ParseShaderVariables(vertexElement, &result->shaderVariables, InputAccessedBy_Vertex);
                        ParseShaderVariables(fragmentElement, &result->shaderVariables, InputAccessedBy_Fragment);

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

                        VkDevice logicalDevice = AstralCanvasVk_GetCurrentGPU()->logicalDevice;

                        VkShaderModuleCreateInfo vertexCreateInfo = {};
                        vertexCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                        vertexCreateInfo.codeSize = vertexSpirvData.length;
                        vertexCreateInfo.pCode = vertexSpirvData.data;

                        VkShaderModule vertexShaderModule;
                        if (vkCreateShaderModule(logicalDevice, &vertexCreateInfo, NULL, &vertexShaderModule) != VK_SUCCESS)
                        {
                            localArena.deinit();
                            return -1;
                        }

                        VkShaderModuleCreateInfo fragmentCreateInfo = {};
                        fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                        fragmentCreateInfo.codeSize = fragmentSpirvData.length;
                        fragmentCreateInfo.pCode = fragmentSpirvData.data;

                        VkShaderModule fragmentShaderModule;
                        if (vkCreateShaderModule(logicalDevice, &fragmentCreateInfo, NULL, &fragmentShaderModule) != VK_SUCCESS)
                        {
                            localArena.deinit();
                            return -1;
                        }

                        result->shaderModule1 = vertexShaderModule;
                        result->shaderModule2 = fragmentShaderModule;

                        //create descriptor
                        VkDescriptorSetLayoutCreateInfo layoutInfo{};
                        layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                        layoutInfo.flags = 0;
                        layoutInfo.bindingCount = result->shaderVariables.uniforms.Count;

                        if (layoutInfo.bindingCount > 0)
                        {
                            collections::Array<VkDescriptorSetLayoutBinding> bindings = collections::Array<VkDescriptorSetLayoutBinding>(&localArena.asAllocator, layoutInfo.bindingCount);

                            for (usize i = 0; i < result->shaderVariables.uniforms.bucketsCount; i++)
                            {
                                if (result->shaderVariables.uniforms.buckets[i].initialized)
                                {
                                    for (usize j = 0; j < result->shaderVariables.uniforms.buckets[i].entries.count; j++)
                                    {
                                        ShaderResource resource = result->shaderVariables.uniforms.buckets[i].entries.ptr[j].value;
                                        VkDescriptorSetLayoutBinding layoutBinding = {};
                                        layoutBinding.binding = resource.binding;
                                        layoutBinding.descriptorCount = max(resource.arrayLength, 1);
                                        layoutBinding.descriptorType = AstralCanvasVk_FromResourceType(resource.type);
                                        layoutBinding.stageFlags = AstralCanvasVk_FromAccessedBy(resource.accessedBy);
                                        layoutBinding.pImmutableSamplers = NULL;

                                        bindings.data[resource.binding] = layoutBinding;
                                    }
                                }
                            }
                            layoutInfo.pBindings = bindings.data;
                        }

                        vkCreateDescriptorSetLayout(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &layoutInfo, NULL, (VkDescriptorSetLayout*)&result->shaderPipelineLayout);
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
            default:
                THROW_ERR("Unimplemented backend");
                break;
        }
    }
}