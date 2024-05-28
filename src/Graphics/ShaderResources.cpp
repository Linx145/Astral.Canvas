#include "Graphics/ShaderResources.hpp"
#include "hash.hpp"

namespace AstralCanvas
{
    ShaderVariables::ShaderVariables()
    {
        this->uniforms = collections::denseset<ShaderResource>();
        this->allocator = IAllocator{};
    }
    ShaderVariables::ShaderVariables(IAllocator allocator)
    {
        this->allocator = allocator;
        this->uniforms = collections::denseset<ShaderResource>(allocator, 16);
    }
    void ShaderVariables::deinit()
    {
        for (usize i = 0; i < this->uniforms.capacity; i++)
        {
            if (this->uniforms.ptr[i].variableName.buffer != NULL)
            {
                for (usize j = 0; j < this->uniforms.ptr[i].stagingData.count; j++)
                {
                    if (this->uniforms.ptr[i].type == ShaderResourceType_Uniform)
                    {
                        //false if we do Graphics::SetData(name, uniformBuffer)
                        if (this->uniforms.ptr[i].stagingData.ptr[j].ownsUniformBuffer)
                        {
                            this->uniforms.ptr[i].stagingData.ptr[j].ub.deinit();
                        }
                    }
                    else if (this->uniforms.ptr[i].type == ShaderResourceType_Sampler)
                    {
                        //dont free samplers here
                        this->allocator.Free(this->uniforms.ptr[i].stagingData.ptr[j].samplerInfos);
                    }
                    else if (this->uniforms.ptr[i].type == ShaderResourceType_Texture)
                    {
                        //dont free textures here
                        this->allocator.Free(this->uniforms.ptr[i].stagingData.ptr[j].imageInfos);
                    }
                    //dont free compute buffers/textures as those are never owned
                }
                this->uniforms.ptr[i].variableName.deinit();
            }
        }
        this->uniforms.deinit();
    }
}
