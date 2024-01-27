#pragma once
#include "Linxc.h"
#include "Graphics/VertexDeclarations.hpp"
#include "vulkan/vulkan.h"
#include "array.hpp"

struct AstralVulkanVertexDecl
{
    VkVertexInputBindingDescription bindingDescription;
    collections::Array<VkVertexInputAttributeDescription> attributeDescriptions;
};

AstralVulkanVertexDecl AstralCanvasVk_CreateVertexDecl(IAllocator *allocator, AstralCanvas::VertexDeclaration *vertexDecl);
void AstralCanvasVk_DestroyVertexDecl(AstralVulkanVertexDecl *vertexDecl);