#pragma once
#include "Linxc.h"
#include "Graphics/VertexDeclarations.hpp"
#include "vulkan/vulkan.h"
#include "array.hpp"

struct AstralCanvasVk_VertexDecl
{
    VkVertexInputBindingDescription bindingDescription;
    collections::Array<VkVertexInputAttributeDescription> attributeDescriptions;
};

AstralCanvasVk_VertexDecl AstralCanvasVk_CreateVertexDecl(IAllocator *allocator, AstralCanvas::VertexDeclaration *vertexDecl);
void AstralCanvasVk_DestroyVertexDecl(AstralCanvasVk_VertexDecl *vertexDecl);