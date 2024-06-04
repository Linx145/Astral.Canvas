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

#ifdef MACOS
#include "Graphics/Metal/MetalImplementations.h"
#endif

#ifdef ASTRALCANVAS_OPENGL
#include "Graphics/Glad/glad.h"
#endif

using namespace Json;

namespace AstralCanvas
{
    Shader::Shader()
    {
        this->allocator = IAllocator{};
        shaderType = ShaderType_VertexFragment;
        shaderModule1 = NULL;
        shaderModule2 = NULL;
        shaderPipelineLayout = NULL;
        shaderVariables = ShaderVariables();
        uniformsHasBeenSet = false;

        this->descriptorForThisDrawCall = 0;
        this->descriptorSets = collections::vector<void *>();
        this->usedMaterials = collections::Array<ShaderMaterialExport>();
    }
    Shader::Shader(IAllocator allocator, ShaderType type)
    {
        this->allocator = allocator;
        shaderType = type;
        shaderModule1 = NULL;
        shaderModule2 = NULL;
        shaderPipelineLayout = NULL;
        shaderVariables = ShaderVariables(allocator);
        uniformsHasBeenSet = false;

        this->descriptorForThisDrawCall = 0;
        this->descriptorSets = collections::vector<void *>(allocator);
        this->usedMaterials = collections::Array<ShaderMaterialExport>();
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
                if (resource != NULL && resource->variableName.buffer != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource{};
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.size = stride;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.arrayLength = 0;
                    newResource.stagingData = collections::vector<ShaderStagingMutableState>(results->allocator);
                    newResource.type = ShaderResourceType_Uniform;
                    results->uniforms.Insert((usize)binding, newResource);
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
                u32 mslBinding = textures->arrayElements.data[i].GetProperty("mslBinding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL && resource->variableName.buffer != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                    name.deinit();
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
                    newResource.mslBinding = mslBinding;
                    newResource.stagingData = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->uniforms.Insert((usize)binding, newResource);
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
                u32 mslBinding = samplers->arrayElements.data[i].GetProperty("mslBinding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL && resource->variableName.buffer != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                    name.deinit();
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
                    newResource.mslBinding = mslBinding;
                    newResource.stagingData = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->uniforms.Insert((usize)binding, newResource);
                }
            }
        }
        JsonElement *inputAttachments = json->GetProperty("inputAttachments");
        if (inputAttachments != NULL)
        {
            for (usize i = 0; i < inputAttachments->arrayElements.length; i++)
            {
                string name = inputAttachments->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 index = inputAttachments->arrayElements.data[i].GetProperty("index")->GetUint32();
                u32 set = inputAttachments->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = inputAttachments->arrayElements.data[i].GetProperty("binding")->GetUint32();
                u32 mslBinding = inputAttachments->arrayElements.data[i].GetProperty("mslBinding")->GetUint32();

                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL && resource->variableName.buffer != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.arrayLength = 0;
                    newResource.inputAttachmentIndex = index;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.type = ShaderResourceType_InputAttachment;
                    newResource.size = 0;
                    newResource.mslBinding = mslBinding;
                    newResource.stagingData = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->uniforms.Insert((usize)binding, newResource);
                }
            }
        }
        JsonElement *storageBuffers = json->GetProperty("storageBuffers");
        if (storageBuffers != NULL)
        {
            for (usize i = 0; i < storageBuffers->arrayElements.length; i++)
            {
                string name = storageBuffers->arrayElements.data[i].GetProperty("name")->GetString(results->allocator);
                u32 set = storageBuffers->arrayElements.data[i].GetProperty("set")->GetUint32();
                u32 binding = storageBuffers->arrayElements.data[i].GetProperty("binding")->GetUint32();
                u32 mslBinding = storageBuffers->arrayElements.data[i].GetProperty("mslBinding")->GetUint32();
            
                ShaderResource *resource = results->uniforms.Get(binding);
                if (resource != NULL && resource->variableName.buffer != NULL)
                {
                    resource->accessedBy = (ShaderInputAccessedBy)((u32)resource->accessedBy | (u32)accessedByShaderOfType);
                    name.deinit();
                }
                else
                {
                    ShaderResource newResource;
                    newResource.binding = binding;
                    newResource.set = set;
                    newResource.variableName = name;
                    newResource.arrayLength = 0;
                    newResource.accessedBy = accessedByShaderOfType;
                    newResource.type = ShaderResourceType_StructuredBuffer;
                    newResource.size = 0;
                    newResource.mslBinding = mslBinding;
                    newResource.stagingData = collections::vector<ShaderStagingMutableState>(results->allocator);
                    results->uniforms.Insert((usize)binding, newResource);
                }
            }
        }
    }
    i32 Shader::GetVariableBinding(const char* variableName)
    {
        for (usize i = 0; i < this->shaderVariables.uniforms.capacity; i++)
        {
            if (this->shaderVariables.uniforms.ptr[i].variableName.buffer == NULL)
            {
                break;
            }
            if (this->shaderVariables.uniforms.ptr[i].variableName == variableName)
            {
                return (i32)i;
            }
        }
        return -1;
    }
    void Shader::CheckDescriptorSetAvailability(bool forceAddNewDescriptor)
    {
        if (descriptorForThisDrawCall >= descriptorSets.count || forceAddNewDescriptor)
        {
            switch (GetActiveBackend())
            {
    #ifdef ASTRALCANVAS_VULKAN
                case Backend_Vulkan:
                {
                    VkDescriptorPool descriptorPool = AstralCanvasVk_GetDescriptorPool();
                    VkDescriptorSet result;
                    VkDescriptorSetAllocateInfo allocInfo{};
                    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
                    allocInfo.descriptorSetCount = 1;
                    allocInfo.descriptorPool = descriptorPool;
                    allocInfo.pSetLayouts = (VkDescriptorSetLayout*)&this->shaderPipelineLayout;
                    
                    if (vkAllocateDescriptorSets(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &allocInfo, &result) != VK_SUCCESS)
                    {
                        THROW_ERR("Error creating descriptor set!");
                    }
                    descriptorSets.Add(result);
                    for (usize i = 0; i < shaderVariables.uniforms.capacity; i++)
                    {
                        ShaderResource *resource = &shaderVariables.uniforms.ptr[i];
                        if (resource->variableName.buffer == NULL)
                        {
                            break;
                        }
                        ShaderStagingMutableState newMutableState{};
                        switch (resource->type)
                        {
                            case ShaderResourceType_StructuredBuffer:
                            {
                                newMutableState.computeBuffer = NULL;
                                break;
                            }
                            case ShaderResourceType_Uniform:
                            {
                                newMutableState.ub = UniformBuffer(resource->size);
                                newMutableState.ownsUniformBuffer = true;
                                break;
                            }
                            case ShaderResourceType_InputAttachment:
                            {
                                newMutableState.textures = collections::Array<Texture2D*>(this->allocator, 1);
                                newMutableState.imageInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.textures.length);
                                break;
                            }
                            case ShaderResourceType_Texture:
                            {
                                newMutableState.textures = collections::Array<Texture2D*>(this->allocator, max(resource->arrayLength, 1));
                                newMutableState.imageInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.textures.length);
                                break;
                            }
                            case ShaderResourceType_Sampler:
                            {
                                newMutableState.samplers = collections::Array<SamplerState*>(this->allocator, max(resource->arrayLength, 1));
                                newMutableState.samplerInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.samplers.length);
                                break;
                            }
                        }
                        shaderVariables.uniforms.ptr[i].stagingData.Add(newMutableState);
                    }
                    break;
                }
    #endif
    #ifdef ASTRALCANVAS_METAL
                case Backend_Metal:
                    {
                        AstralCanvasMetal_AddUniformDescriptorSets(this);
                        break;
                    }
    #endif
    #ifdef ASTRALCANVAS_OPENGL
                case Backend_OpenGL:
                {
                    descriptorSets.Add(NULL);
                    for (usize i = 0; i < shaderVariables.uniforms.capacity; i++)
                    {
                        ShaderResource* resource = &shaderVariables.uniforms.ptr[i];
                        if (resource->variableName.buffer == NULL)
                        {
                            break;
                        }
                        ShaderStagingMutableState newMutableState{};
                        switch (resource->type)
                        {
                            case ShaderResourceType_StructuredBuffer:
                            {
                                newMutableState.computeBuffer = NULL;
                                break;
                            }
                            case ShaderResourceType_Uniform:
                            {
                                newMutableState.ub = UniformBuffer(resource->size);
                                newMutableState.ownsUniformBuffer = true;
                                break;
                            }
                            case ShaderResourceType_InputAttachment:
                            {
                                newMutableState.textures = collections::Array<Texture2D*>(this->allocator, 1);
                                //newMutableState.imageInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.textures.length);
                                break;
                            }
                            case ShaderResourceType_Texture:
                            {
                                newMutableState.textures = collections::Array<Texture2D*>(this->allocator, max(resource->arrayLength, 1));
                                //newMutableState.imageInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.textures.length);
                                break;
                            }
                            case ShaderResourceType_Sampler:
                            {
                                newMutableState.samplers = collections::Array<SamplerState*>(this->allocator, max(resource->arrayLength, 1));
                                //newMutableState.samplerInfos = this->allocator.Allocate(sizeof(VkDescriptorImageInfo) * newMutableState.samplers.length);
                                break;
                            }
                        }
                        shaderVariables.uniforms.ptr[i].stagingData.Add(newMutableState);
                    }
                    break;
                }
    #endif
                default:
                    break;
            }
        }
    }
    void Shader::SyncUniformsWithGPU(void *commandEncoder)
    {
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {
                u32 setWriteCount = 0;
                u32 bufferInfoCount = 0;
                VkWriteDescriptorSet setWrites[MAX_UNIFORMS_IN_SHADER];
                VkDescriptorBufferInfo bufferInfos[MAX_UNIFORMS_IN_SHADER];

                if (this->shaderVariables.uniforms.ptr == NULL)
                {
                    THROW_ERR("Cannot sync shader without uniforms with GPU");
                }
                for (usize i = 0; i < this->shaderVariables.uniforms.capacity; i++)
                {
                    if (this->shaderVariables.uniforms.ptr[i].variableName.buffer == NULL)
                    {
                        break;
                    }
                    //should never throw out of range error since CheckDescriptorSetAvailability() is always called prior to this
                    ShaderStagingMutableState *toMutate = &this->shaderVariables.uniforms.ptr[i].stagingData.ptr[this->descriptorForThisDrawCall];
                    if (!toMutate->mutated)
                    {
                        continue;
                    }
                    //set mutated to false in anticipation of reuse for next frame
                    toMutate->mutated = false;

                    VkWriteDescriptorSet setWrite{};
                    setWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    setWrite.dstSet = (VkDescriptorSet)this->descriptorSets.ptr[descriptorForThisDrawCall];
                    setWrite.dstBinding = this->shaderVariables.uniforms.ptr[i].binding;

                    switch (this->shaderVariables.uniforms.ptr[i].type)
                    {
                        case ShaderResourceType_InputAttachment:
                        {
                            VkDescriptorImageInfo imageInfo{};
                            imageInfo.sampler = NULL;
                            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
                            imageInfo.imageView = (VkImageView)toMutate->textures.data[0]->imageView;
                            ((VkDescriptorImageInfo*)toMutate->imageInfos)[0] = imageInfo;

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->textures.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->imageInfos;
                            break;
                        }
                        case ShaderResourceType_Uniform:
                        {
                            UniformBuffer buffer = toMutate->ub;
                            bufferInfos[bufferInfoCount].buffer = (VkBuffer)buffer.handle;
                            bufferInfos[bufferInfoCount].offset = 0;
                            bufferInfos[bufferInfoCount].range = buffer.size;

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = 1;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
                            setWrite.pBufferInfo = &bufferInfos[bufferInfoCount];

                            bufferInfoCount += 1;

                            break;
                        }
                        case ShaderResourceType_Texture:
                        {
                            for (usize i = 0; i < toMutate->textures.length; i++)
                            {
                                VkDescriptorImageInfo imageInfo{};
                                imageInfo.sampler = NULL;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
                                imageInfo.imageView = (VkImageView)toMutate->textures.data[i]->imageView;
                                ((VkDescriptorImageInfo*)toMutate->imageInfos)[i] = imageInfo;
                            }

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->textures.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->imageInfos;

                            break;
                        }
                        case ShaderResourceType_Sampler:
                        {
                            for (usize i = 0; i < toMutate->samplers.length; i++)
                            {
                                VkDescriptorImageInfo samplerInfo{};
                                samplerInfo.sampler = (VkSampler)toMutate->samplers.data[i]->handle;
                                samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                samplerInfo.imageView = NULL;
                                ((VkDescriptorImageInfo*)toMutate->samplerInfos)[i] = samplerInfo;
                            }

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->samplers.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->samplerInfos;

                            break;
                        }
                        case ShaderResourceType_StructuredBuffer:
                        {
                            ComputeBuffer *buffer = toMutate->computeBuffer;
                            bufferInfos[bufferInfoCount].buffer = (VkBuffer)buffer->handle;
                            bufferInfos[bufferInfoCount].offset = 0;
                            bufferInfos[bufferInfoCount].range = buffer->elementSize * buffer->elementCount;

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = 1;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            setWrite.pBufferInfo = &bufferInfos[bufferInfoCount];

                            bufferInfoCount += 1;

                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    setWrites[setWriteCount] = setWrite;
                    setWriteCount += 1;
                }

                vkUpdateDescriptorSets(AstralCanvasVk_GetCurrentGPU()->logicalDevice, setWriteCount, setWrites, 0, NULL);

                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_SyncUniformsWithGPU(commandEncoder, this);
                
                break;
            }
#endif
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                for (usize i = 0; i < this->shaderVariables.uniforms.capacity; i++)
                {
                    if (this->shaderVariables.uniforms.ptr[i].variableName.buffer == NULL)
                    {
                        break;
                    }

                    ShaderStagingMutableState* toMutate = &this->shaderVariables.uniforms.ptr[i].stagingData.ptr[this->descriptorForThisDrawCall];
                    if (!toMutate->mutated)
                    {
                        continue;
                    }
                    //set mutated to false in anticipation of reuse for next frame
                    toMutate->mutated = false;

                    switch (this->shaderVariables.uniforms.ptr[i].type)
                    {
                        case ShaderResourceType_InputAttachment:
                        {
                            VkDescriptorImageInfo imageInfo{};
                            imageInfo.sampler = NULL;
                            imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
                            imageInfo.imageView = (VkImageView)toMutate->textures.data[0]->imageView;
                            ((VkDescriptorImageInfo*)toMutate->imageInfos)[0] = imageInfo;

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->textures.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->imageInfos;
                            break;
                        }
                        case ShaderResourceType_Uniform:
                        {
                            UniformBuffer buffer = toMutate->ub;
                            /*bufferInfos[bufferInfoCount].buffer = (VkBuffer)buffer.handle;
                            bufferInfos[bufferInfoCount].offset = 0;
                            bufferInfos[bufferInfoCount].range = buffer.size;*/

                            glBindBuffer(GL_UNIFORM_BUFFER, (GLuint)buffer.handle);

                            break;
                        }
                        case ShaderResourceType_Texture:
                        {
                            glUniform1i(unifrom location, "variablename");
                            /*for (usize i = 0; i < toMutate->textures.length; i++)
                            {
                                VkDescriptorImageInfo imageInfo{};
                                imageInfo.sampler = NULL;
                                imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; //(VkImageLayout)toMutate->textures.data[i]->imageLayout;
                                imageInfo.imageView = (VkImageView)toMutate->textures.data[i]->imageView;
                                ((VkDescriptorImageInfo*)toMutate->imageInfos)[i] = imageInfo;
                            }

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->textures.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->imageInfos;*/

                            break;
                        }
                        case ShaderResourceType_Sampler:
                        {
                            for (usize i = 0; i < toMutate->samplers.length; i++)
                            {
                                VkDescriptorImageInfo samplerInfo{};
                                samplerInfo.sampler = (VkSampler)toMutate->samplers.data[i]->handle;
                                samplerInfo.imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
                                samplerInfo.imageView = NULL;
                                ((VkDescriptorImageInfo*)toMutate->samplerInfos)[i] = samplerInfo;
                            }

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = toMutate->samplers.length;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;
                            setWrite.pImageInfo = (VkDescriptorImageInfo*)toMutate->samplerInfos;

                            break;
                        }
                        case ShaderResourceType_StructuredBuffer:
                        {
                            ComputeBuffer* buffer = toMutate->computeBuffer;
                            bufferInfos[bufferInfoCount].buffer = (VkBuffer)buffer->handle;
                            bufferInfos[bufferInfoCount].offset = 0;
                            bufferInfos[bufferInfoCount].range = buffer->elementSize * buffer->elementCount;

                            setWrite.dstArrayElement = 0;
                            setWrite.descriptorCount = 1;
                            setWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
                            setWrite.pBufferInfo = &bufferInfos[bufferInfoCount];

                            bufferInfoCount += 1;

                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                }
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: Shader SyncUniformsWithGPU");
                break;
        }
    }
    void Shader::SetShaderVariableComputeBuffer(const char* variableName, ComputeBuffer* buffer)
    {
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].mutated = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].computeBuffer = buffer;
            }
        }
    }
    void Shader::SetShaderVariable(const char* variableName, void* ptr, usize size)
    {
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].mutated = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].ub.SetData(ptr, size);
            }
        }
    }
    void Shader::SetShaderVariableTextures(const char* variableName, Texture2D **textures, usize count)
    {
#ifdef ASTRALCANVAS_OPENGL
        THROW_ERR("Bindless texturing not supported in OpenGL!");
#endif
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                ShaderStagingMutableState *mutableState = &variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall];
                mutableState->mutated = true;
                for (usize j = 0; j < count; j++)
                {
                    variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].textures.data[j] = textures[j];
                }
                break;
            }
        }
    }
    void Shader::SetShaderVariableTexture(const char* variableName, Texture2D *texture)
    {
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].mutated = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].textures.data[0] = texture;
                return;
            }
        }
        fprintf(stderr, "Variable of name %s not found\n", variableName);
    }
    void Shader::SetShaderVariableSamplers(const char* variableName, SamplerState **samplers, usize count)
    {
#ifdef ASTRALCANVAS_OPENGL
        THROW_ERR("Bindless texturing not supported in OpenGL!");
#endif
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                ShaderStagingMutableState *mutableState = &variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall];
                mutableState->mutated = true;
                for (usize j = 0; j < count; j++)
                {
                    variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].samplers.data[j] = samplers[j];
                }
                break;
            }
        }
    }
    void Shader::SetShaderVariableSampler(const char* variableName, SamplerState *sampler)
    {
        CheckDescriptorSetAvailability();
        ShaderVariables *variables = &shaderVariables;
        for (usize i = 0; i < variables->uniforms.capacity; i++)
        {
            if (variables->uniforms.ptr[i].variableName.buffer == NULL)
            {
                if (GetActiveBackend() == Backend_Vulkan)
                {
                    break; //can only break in vulkan as metal has non-continguous uniform indices
                }
                continue;
            }
            if (variables->uniforms.ptr[i].variableName == variableName)
            {
                uniformsHasBeenSet = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].mutated = true;
                variables->uniforms.ptr[i].stagingData.ptr[descriptorForThisDrawCall].samplers.data[0] = sampler;
                break;
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
                break;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                AstralCanvasMetal_DestroyShaderProgram(this->shaderModule1, this->shaderModule2);
                this->shaderModule1 = NULL;
                this->shaderModule2 = NULL;
                break;
            }
#endif
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                glDeleteShader((u32)shaderModule1);
                glDeleteShader((u32)shaderModule2);
                break;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: Shader deinit");
                break;
        }

        this->shaderVariables.deinit();
        if (this->usedMaterials.data != NULL)
        {
            for (usize i = 0; i < this->usedMaterials.length; i++)
            {
                this->usedMaterials.data[i].deinit();
            }
            this->usedMaterials.deinit();
        }
    }
    i32 CreateShaderFromString(IAllocator allocator, string jsonString, Shader* result)
    {
        *result = AstralCanvas::Shader(allocator, ShaderType_VertexFragment);
        ArenaAllocator localArena = ArenaAllocator(allocator);
        
        JsonElement root;
        usize parseJsonResult = ParseJsonDocument(localArena.AsAllocator(), jsonString, &root);
        if (parseJsonResult != 0)
        {
            localArena.deinit();
            return (i32)parseJsonResult;
        }
        Json::JsonElement *materialsElement = root.GetProperty("materials");
        if (materialsElement != NULL)
        {
            result->usedMaterials = collections::Array<ShaderMaterialExport>(result->allocator, materialsElement->childObjects.Count);
            u32 materialIndex = 0;
            for (usize i = 0; i < materialsElement->childObjects.bucketsCount; i++)
            {
                if (materialsElement->childObjects.buckets[i].initialized)
                {
                    for (usize j = 0; j < materialsElement->childObjects.buckets[i].entries.count; j++)
                    {
                        Json::JsonElement *materialElement = &materialsElement->childObjects.buckets[i].entries.ptr[j].value;
                        result->usedMaterials.data[materialIndex].name = string(result->allocator, materialsElement->childObjects.buckets[i].entries.ptr[j].key.buffer);
                        result->usedMaterials.data[materialIndex].params = collections::Array<ShaderMaterialExportParam>(result->allocator, materialElement->childObjects.Count);
                        u32 paramIndex = 0;
                        for (usize c = 0; c < materialElement->childObjects.bucketsCount; c++)
                        {
                            if (materialElement->childObjects.buckets[c].initialized)
                            {
                                for (usize d = 0; d < materialElement->childObjects.buckets[c].entries.count; d++)
                                {
                                    result->usedMaterials.data[materialIndex].params.data[paramIndex].name = string(result->allocator, materialElement->childObjects.buckets[c].entries.ptr[d].key.buffer);
                                    result->usedMaterials.data[materialIndex].params.data[paramIndex].size = materialElement->childObjects.buckets[c].entries.ptr[d].value.GetUint32();
                                    paramIndex++;
                                }
                            }
                        }
                        materialIndex++;
                    }
                }
            }
        }
        
        switch (GetActiveBackend())
        {
            #ifdef ASTRALCANVAS_VULKAN
            case Backend_Vulkan:
            {

                JsonElement *computeElement = root.GetProperty("compute");

                if (computeElement != NULL)
                {
                    result->shaderType = ShaderType_Compute;
                    ParseShaderVariables(computeElement, &result->shaderVariables, InputAccessedBy_Compute);
                
                    JsonElement *computeSpirv = computeElement->GetProperty("spirv");
                    collections::Array<u32> computeSpirvData = collections::Array<u32>(localArena.AsAllocator(), computeSpirv->arrayElements.length);
                    for (usize i = 0; i < computeSpirv->arrayElements.length; i++)
                    {
                        computeSpirvData.data[i] = computeSpirv->arrayElements.data[i].GetUint32();
                    }

                    VkDevice logicalDevice = AstralCanvasVk_GetCurrentGPU()->logicalDevice;

                    VkShaderModuleCreateInfo computeCreateInfo = {};
                    computeCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                    computeCreateInfo.codeSize = computeSpirvData.length * 4;
                    computeCreateInfo.pCode = computeSpirvData.data;

                    VkShaderModule computeShaderModule;
                    #if DEBUG
                    printf("Attempting to create compute shader module\n");
                    #endif
                    if (vkCreateShaderModule(logicalDevice, &computeCreateInfo, NULL, &computeShaderModule) != VK_SUCCESS)
                    {
                        localArena.deinit();
                        return -1;
                    }
                    result->shaderModule1 = computeShaderModule;

                }
                else
                {
                    result->shaderType = ShaderType_VertexFragment;
                    JsonElement *vertexElement = root.GetProperty("vertex");
                    JsonElement *fragmentElement = root.GetProperty("fragment");

                    if (vertexElement != NULL && fragmentElement != NULL)
                    {
                        ParseShaderVariables(vertexElement, &result->shaderVariables, InputAccessedBy_Vertex);
                        ParseShaderVariables(fragmentElement, &result->shaderVariables, InputAccessedBy_Fragment);

                        JsonElement *vertexSpirv = vertexElement->GetProperty("spirv");
                        JsonElement *fragmentSpirv = fragmentElement->GetProperty("spirv");

                        collections::Array<u32> vertexSpirvData = collections::Array<u32>(localArena.AsAllocator(), vertexSpirv->arrayElements.length);
                        collections::Array<u32> fragmentSpirvData = collections::Array<u32>(localArena.AsAllocator(), fragmentSpirv->arrayElements.length);

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
                        vertexCreateInfo.codeSize = vertexSpirvData.length * 4;
                        vertexCreateInfo.pCode = vertexSpirvData.data;

                        VkShaderModule vertexShaderModule;
                        if (vkCreateShaderModule(logicalDevice, &vertexCreateInfo, NULL, &vertexShaderModule) != VK_SUCCESS)
                        {
                            printf("Failed to load vertex shader module\n");
                            localArena.deinit();
                            return -1;
                        }

                        VkShaderModuleCreateInfo fragmentCreateInfo = {};
                        fragmentCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
                        fragmentCreateInfo.codeSize = fragmentSpirvData.length * 4;
                        fragmentCreateInfo.pCode = fragmentSpirvData.data;

                        VkShaderModule fragmentShaderModule;
                        if (vkCreateShaderModule(logicalDevice, &fragmentCreateInfo, NULL, &fragmentShaderModule) != VK_SUCCESS)
                        {
                            printf("Failed to load fragment shader module\n");
                            localArena.deinit();
                            return -1;
                        }

                        result->shaderModule1 = vertexShaderModule;
                        result->shaderModule2 = fragmentShaderModule;
                    }
                    else
                    {
                        printf("Failed to detect shader type\n");
                        localArena.deinit();
                        return -1;
                    }
                }

                //create descriptor
                VkDescriptorSetLayoutCreateInfo layoutInfo{};
                layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
                layoutInfo.flags = 0;
                layoutInfo.bindingCount = 0;

                for (usize i = 0; i < result->shaderVariables.uniforms.capacity; i++)
                {
                    if (result->shaderVariables.uniforms.ptr[i].variableName.buffer == NULL)
                    {
                        break;
                    }
                    layoutInfo.bindingCount++;
                }

                result->shaderPipelineLayout = NULL;
                if (layoutInfo.bindingCount > 0)
                {
                    collections::Array<VkDescriptorSetLayoutBinding> bindings = collections::Array<VkDescriptorSetLayoutBinding>(localArena.AsAllocator(), layoutInfo.bindingCount);

                    for (usize i = 0; i < result->shaderVariables.uniforms.capacity; i++)
                    {
                        if (result->shaderVariables.uniforms.ptr[i].variableName.buffer == NULL)
                        {
                            break;
                        }

                        ShaderResource resource = result->shaderVariables.uniforms.ptr[i];
                        VkDescriptorSetLayoutBinding layoutBinding = {};
                        layoutBinding.binding = resource.binding;
                        layoutBinding.descriptorCount = max(resource.arrayLength, 1);
                        layoutBinding.descriptorType = AstralCanvasVk_FromResourceType(resource.type);
                        layoutBinding.stageFlags = AstralCanvasVk_FromAccessedBy(resource.accessedBy);
                        layoutBinding.pImmutableSamplers = NULL;

                        bindings.data[resource.binding] = layoutBinding;
                    }
                    layoutInfo.pBindings = bindings.data;

                    if (vkCreateDescriptorSetLayout(AstralCanvasVk_GetCurrentGPU()->logicalDevice, &layoutInfo, NULL, (VkDescriptorSetLayout*)&result->shaderPipelineLayout) != VK_SUCCESS)
                    {
                        THROW_ERR("Failed to create descriptor set layout");
                    }
                }

                localArena.deinit();
                return 0;
            }
            #endif
#ifdef ASTRALCANVAS_METAL
            case Backend_Metal:
            {
                JsonElement *computeElement = root.GetProperty("compute");

                if (computeElement != NULL)
                {
                    result->shaderType = ShaderType_Compute;
                    ParseShaderVariables(computeElement, &result->shaderVariables, InputAccessedBy_Compute);
                }
                else
                {
                    result->shaderType = ShaderType_VertexFragment;
                    JsonElement *vertexElement = root.GetProperty("vertex");
                    JsonElement *fragmentElement = root.GetProperty("fragment");

                    if (vertexElement != NULL && fragmentElement != NULL)
                    {
                        ParseShaderVariables(vertexElement, &result->shaderVariables, InputAccessedBy_Vertex);
                        ParseShaderVariables(fragmentElement, &result->shaderVariables, InputAccessedBy_Fragment);
                        
                        JsonElement *vertexMetal = vertexElement->GetProperty("msl");
                        JsonElement *fragmentMetal = fragmentElement->GetProperty("msl");
                        
                        string vertexMetalString = vertexMetal->GetString(&localArena.AsAllocator());
                        string fragmentMetalString = fragmentMetal->GetString(&localArena.AsAllocator());
                        
                        if (!AstralCanvasMetal_CreateShaderProgram(vertexMetalString, fragmentMetalString, &result->shaderModule1, &result->shaderModule2))
                        {
                            THROW_ERR("Failed to create metal shader program!");
                        }
                    }
                }
                return 0;
            }
#endif
#ifdef ASTRALCANVAS_OPENGL
            case Backend_OpenGL:
            {
                JsonElement* computeElement = root.GetProperty("compute");

                if (computeElement != NULL)
                {
                    result->shaderType = ShaderType_Compute;
                    ParseShaderVariables(computeElement, &result->shaderVariables, InputAccessedBy_Compute);

                    // get shader binary as string
                    JsonElement* computeSpirv = computeElement->GetProperty("spirv");
                    string computeString = computeSpirv->value;

                    u32 computeHandle = glCreateShader(GL_COMPUTE_SHADER);

                    // Apply the shader SPIR-V to the shader object
                    glShaderBinary(1, &computeHandle, GL_SHADER_BINARY_FORMAT_SPIR_V,
                        computeString.buffer, computeString.length);

                    // compile spirv --> glsl
                    glSpecializeShader(computeHandle, "main", 0, nullptr, nullptr);
                    
                    GLint isCompiled = 0;
                    glGetShaderiv(computeHandle, GL_COMPILE_STATUS, &isCompiled);
                    if (isCompiled == GL_FALSE)
                    {
                        glDeleteShader(computeHandle);
                        THROW_ERR("Failed to compile OpenGL compute shader!");
                    }

                    result->shaderModule1 = (void*)computeHandle;

                    //// create shader program
                    //u32 programHandle = glCreateProgram();
                    //
                    //glAttachShader(programHandle, computeHandle);
                    //glLinkProgram(programHandle);
                    //
                    //// delete shader
                    //glDetachShader(programHandle, computeHandle);
                    //glDeleteShader(computeHandle);
                    //
                    //GLint isLinked = 0;
                    //glGetProgramiv(programHandle, GL_LINK_STATUS, (int*)&isLinked);
                    //if (isLinked == GL_FALSE)
                    //{
                    //    glDeleteProgram(programHandle);
                    //    THROW_ERR("Failed to link OpenGL compute program!");
                    //}
                }
                else
                {
                    // get the spirv shaders and compile
                    result->shaderType = ShaderType_VertexFragment;
                    JsonElement* vertexElement = root.GetProperty("vertex");
                    JsonElement* fragmentElement = root.GetProperty("fragment");

                    if (vertexElement != NULL && fragmentElement != NULL)
                    {
                        ParseShaderVariables(vertexElement, &result->shaderVariables, InputAccessedBy_Vertex);
                        ParseShaderVariables(fragmentElement, &result->shaderVariables, InputAccessedBy_Fragment);

                        // get spirv binary to convert to string
                        JsonElement* vertexSpirv = vertexElement->GetProperty("spirv");
                        JsonElement* fragmentSpirv = fragmentElement->GetProperty("spirv");

                        collections::Array<u32> vertexSpirvData = collections::Array<u32>(localArena.AsAllocator(), vertexSpirv->arrayElements.length);
                        collections::Array<u32> fragmentSpirvData = collections::Array<u32>(localArena.AsAllocator(), fragmentSpirv->arrayElements.length);

                        for (usize i = 0; i < vertexSpirv->arrayElements.length; i++)
                        {
                            vertexSpirvData.data[i] = vertexSpirv->arrayElements.data[i].GetUint32();
                        }
                        for (usize i = 0; i < fragmentSpirv->arrayElements.length; i++)
                        {
                            fragmentSpirvData.data[i] = fragmentSpirv->arrayElements.data[i].GetUint32();
                        }

                        u32 vertexHandle = glCreateShader(GL_VERTEX_SHADER);
                        u32 fragmentHandle = glCreateShader(GL_FRAGMENT_SHADER);

                        // Apply the shaders SPIR-V to the shader objects
                        glShaderBinary(1, &vertexHandle, GL_SHADER_BINARY_FORMAT_SPIR_V,
                            vertexSpirvData.data, vertexSpirvData.length * 4);

                        glShaderBinary(1, &fragmentHandle, GL_SHADER_BINARY_FORMAT_SPIR_V,
                            fragmentSpirvData.data, fragmentSpirvData.length * 4);

                        // specialize the shaders (analogous to compiling glsl shader)
                        glSpecializeShader(vertexHandle, "main", 0, nullptr, nullptr);
                        glSpecializeShader(fragmentHandle, "main", 0, nullptr, nullptr);

                        

                        GLint isCompiled = 0;
                        glGetShaderiv(vertexHandle, GL_COMPILE_STATUS, &isCompiled);
                        if (isCompiled == GL_FALSE)
                        {
                            glDeleteShader(vertexHandle);
                            THROW_ERR("Failed to compile OpenGL vertex shader!");
                        }
                        glGetShaderiv(fragmentHandle, GL_COMPILE_STATUS, &isCompiled);
                        if (isCompiled == GL_FALSE)
                        {
                            glDeleteShader(fragmentHandle);
                            THROW_ERR("Failed to compile OpenGL fragment shader!");
                        }

                        result->shaderModule1 = (void*)vertexHandle;
                        result->shaderModule1 = (void*)fragmentHandle;
                    }
                }
                return 0;
            }
#endif
            default:
                THROW_ERR("Unimplemented backend: Shader CreateShaderFromString");
                break;
        }
        localArena.deinit();
        return -1;
    }
}
