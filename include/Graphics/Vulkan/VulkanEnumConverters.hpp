#pragma once
#include "vulkan/vulkan.h"
#include "Graphics/Enums.hpp"

inline VkFilter AstralCanvasVk_FromSampleMode(AstralCanvas::SampleMode sampleMode)
{
    switch (sampleMode)
    {
        case AstralCanvas::SampleMode::SampleMode_Linear:
            return VK_FILTER_LINEAR;
        case AstralCanvas::SampleMode::SampleMode_Point:
            return VK_FILTER_NEAREST;
        case AstralCanvas::SampleMode::SampleMode_Cubic:
            return VK_FILTER_CUBIC_EXT;
    }
}
inline VkSamplerAddressMode AstralCanvasVk_FromRepeatMode(AstralCanvas::RepeatMode repeatMode)
{
    switch (repeatMode)
    {
        case AstralCanvas::RepeatMode::RepeatMode_ClampToEdgeColor:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case AstralCanvas::RepeatMode::RepeatMode_ClampToSpecificColor:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case AstralCanvas::RepeatMode::RepeatMode_Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}
inline VkFormat AstralCanvasVk_FromVertexElementFormat(AstralCanvas::VertexElementFormat format)
{
    switch (format)
    {
        case AstralCanvas::VertexElementFormat::VertexElement_Float:
            return VK_FORMAT_R32_SFLOAT;
        case AstralCanvas::VertexElementFormat::VertexElement_Vector2:
            return VK_FORMAT_R32G32_SFLOAT;
        case AstralCanvas::VertexElementFormat::VertexElement_Vector3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case AstralCanvas::VertexElementFormat::VertexElement_Vector4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case AstralCanvas::VertexElementFormat::VertexElement_Int:
            return VK_FORMAT_R32_SINT;
        case AstralCanvas::VertexElementFormat::VertexElement_Color:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case AstralCanvas::VertexElementFormat::VertexElement_Uint:
            return VK_FORMAT_R32_UINT;
    }
}
inline VkFormat AstralCanvasVk_FromImageFormat(AstralCanvas::ImageFormat format)
{
    switch (format)
    {
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8SNorm:
            return VK_FORMAT_B8G8R8A8_SNORM;
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Srgb:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Unorm:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8SNorm:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Srgb:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Unorm:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case AstralCanvas::ImageFormat::ImageFormat_HalfVector4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case AstralCanvas::ImageFormat::ImageFormat_R32Uint:
            return VK_FORMAT_R32_UINT;
        case AstralCanvas::ImageFormat::ImageFormat_Vector4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case AstralCanvas::ImageFormat::ImageFormat_Depth16:
            return VK_FORMAT_D16_UNORM;
        case AstralCanvas::ImageFormat::ImageFormat_Depth16Stencil8:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case AstralCanvas::ImageFormat::ImageFormat_Depth24Stencil8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case AstralCanvas::ImageFormat::ImageFormat_Depth32:
            return VK_FORMAT_D32_SFLOAT;
        
        default:
            return VK_FORMAT_UNDEFINED;
    }
}
inline AstralCanvas::ImageFormat AstralCanvasVk_FromVkFormat(VkFormat format)
{
    switch (format)
    {
        case VK_FORMAT_B8G8R8A8_SNORM:
            return AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8SNorm;
        case VK_FORMAT_B8G8R8A8_SRGB:
            return AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Srgb;
        case VK_FORMAT_B8G8R8A8_UNORM:
            return AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Unorm;

        case VK_FORMAT_R8G8B8A8_SNORM:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8SNorm;
        case VK_FORMAT_R8G8B8A8_SRGB:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Srgb;
        case VK_FORMAT_R8G8B8A8_UNORM:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Unorm;

        case VK_FORMAT_R16G16B16A16_SFLOAT:
            return AstralCanvas::ImageFormat::ImageFormat_HalfVector4;
        case VK_FORMAT_R32_UINT:
            return AstralCanvas::ImageFormat::ImageFormat_R32Uint;
        case VK_FORMAT_R32G32B32A32_SFLOAT:
            return AstralCanvas::ImageFormat::ImageFormat_Vector4;

        case VK_FORMAT_D16_UNORM:
            return AstralCanvas::ImageFormat::ImageFormat_Depth16;
        case VK_FORMAT_D16_UNORM_S8_UINT:
            return AstralCanvas::ImageFormat::ImageFormat_Depth16Stencil8;
        case VK_FORMAT_D24_UNORM_S8_UINT:
            return AstralCanvas::ImageFormat::ImageFormat_Depth24Stencil8;
        case VK_FORMAT_D32_SFLOAT:
            return AstralCanvas::ImageFormat::ImageFormat_Depth32;
        
        default:
            return AstralCanvas::ImageFormat_Undefined;
    }
}
inline VkPrimitiveTopology AstralCanvasVk_FromPrimitiveType(AstralCanvas::PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case AstralCanvas::PrimitiveType::PrimitiveType_LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case AstralCanvas::PrimitiveType::PrimitiveType_LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case AstralCanvas::PrimitiveType::PrimitiveType_TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case AstralCanvas::PrimitiveType::PrimitiveType_TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case AstralCanvas::PrimitiveType::PrimitiveType_TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    }
}
inline VkCullModeFlags AstralCanvasVk_FromCullMode(AstralCanvas::CullMode cullMode)
{
    switch (cullMode)
    {
        case AstralCanvas::CullMode::CullMode_CullNone:
            return VK_CULL_MODE_NONE;
        case AstralCanvas::CullMode::CullMode_CullClockwise:
            return VK_CULL_MODE_FRONT_BIT;
        case AstralCanvas::CullMode::CullMode_CullCounterClockwise:
            return VK_CULL_MODE_BACK_BIT;
    }
}
inline VkDescriptorType AstralCanvasVk_FromResourceType(AstralCanvas::ShaderResourceType type)
{
    switch (type)
    {
        case AstralCanvas::ShaderResourceType::ShaderResourceType_Texture:
            return VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
        case AstralCanvas::ShaderResourceType::ShaderResourceType_Sampler:
            return VK_DESCRIPTOR_TYPE_SAMPLER;
        case AstralCanvas::ShaderResourceType::ShaderResourceType_Uniform:
            return VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        case AstralCanvas::ShaderResourceType::ShaderResourceType_StructuredBuffer:
            return VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    }
}
inline VkShaderStageFlags AstralCanvasVk_FromAccessedBy(AstralCanvas::ShaderInputAccessedBy accessedBy)
{
    VkShaderStageFlags stageFlags = 0;
    if ((accessedBy & AstralCanvas::ShaderInputAccessedBy::InputAccessedBy_Compute) != 0)
    {
        stageFlags = stageFlags | VK_SHADER_STAGE_COMPUTE_BIT;
    }
    if ((accessedBy & AstralCanvas::ShaderInputAccessedBy::InputAccessedBy_Fragment) != 0)
    {
        stageFlags = stageFlags | VK_SHADER_STAGE_FRAGMENT_BIT;
    }
    if ((accessedBy & AstralCanvas::ShaderInputAccessedBy::InputAccessedBy_Vertex) != 0)
    {
        stageFlags = stageFlags | VK_SHADER_STAGE_VERTEX_BIT;
    }
    return stageFlags;
    // switch (blend)
    // {
    //     case InputAccessedBy_Compute:
    //         return VK_SHADER_STAGE_COMPUTE_BIT;
    //     case InputAccessedBy_Fragment:
    // }
}
inline VkBlendFactor AstralCanvasVk_FromBlend(AstralCanvas::Blend blend)
{
    switch (blend)
    {
        case AstralCanvas::Blend::Blend_DestinationAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case AstralCanvas::Blend::Blend_DestinationColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case AstralCanvas::Blend::Blend_SourceAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case AstralCanvas::Blend::Blend_SourceColor:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case AstralCanvas::Blend::Blend_InverseDestinationAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case AstralCanvas::Blend::Blend_InverseDestinationColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case AstralCanvas::Blend::Blend_InverseSourceAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case AstralCanvas::Blend::Blend_InverseSourceColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        
        case AstralCanvas::Blend::Blend_One:
            return VK_BLEND_FACTOR_ONE;
        case AstralCanvas::Blend::Blend_Zero:
            return VK_BLEND_FACTOR_ZERO;

        default:
            return VK_BLEND_FACTOR_ONE;
    }
}