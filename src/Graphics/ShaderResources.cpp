#include "Graphics/ShaderResources.hpp"
#include "hash.hpp"

namespace AstralCanvas
{
    ShaderVariables::ShaderVariables()
    {
        this->uniforms = collections::hashmap<i32, ShaderResource>();
        this->allocator = NULL;
    }
    ShaderVariables::ShaderVariables(IAllocator *allocator)
    {
        this->allocator = allocator;
        this->uniforms = collections::hashmap<i32, ShaderResource>(allocator, &i32Hash, &i32Eql);
    }
    void ShaderVariables::deinit()
    {
        this->uniforms.deinit();
    }
}