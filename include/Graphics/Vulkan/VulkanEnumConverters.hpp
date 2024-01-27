#pragma once
#include "vulkan/vulkan.h"
#include "Graphics/Enums.hpp"

inline VkFilter AstralCanvasVk_FromSampleMode(SampleMode sampleMode)
{
    switch (sampleMode)
    {
        case SampleMode_Linear:
            return VK_FILTER_LINEAR;
        case SampleMode_Point:
            return VK_FILTER_NEAREST;
        case SampleMode_Cubic:
            return VK_FILTER_CUBIC_EXT;
    }
}
inline VkSamplerAddressMode AstralCanvasVk_FromRepeatMode(RepeatMode repeatMode)
{
    switch (repeatMode)
    {
        case RepeatMode_ClampToEdgeColor:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
        case RepeatMode_ClampToSpecificColor:
            return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER;
        case RepeatMode_Repeat:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
        default:
            return VK_SAMPLER_ADDRESS_MODE_REPEAT;
    }
}
inline VkFormat AstralCanvasVk_FromVertexElementFormat(VertexElementFormat format)
{
    switch (format)
    {
        case VertexElement_Float:
            return VK_FORMAT_R32_SFLOAT;
        case VertexElement_Vector2:
            return VK_FORMAT_R32G32_SFLOAT;
        case VertexElement_Vector3:
            return VK_FORMAT_R32G32B32_SFLOAT;
        case VertexElement_Vector4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;
        case VertexElement_Int:
            return VK_FORMAT_R32_SINT;
        case VertexElement_Color:
            return VK_FORMAT_R8G8B8A8_UNORM;
        case VertexElement_Uint:
            return VK_FORMAT_R32_UINT;
    }
}
inline VkFormat AstralCanvasVk_FromImageFormat(ImageFormat format)
{
    switch (format)
    {
        case ImageFormat_B8G8R8A8SNorm:
            return VK_FORMAT_B8G8R8A8_SNORM;
        case ImageFormat_B8G8R8A8Srgb:
            return VK_FORMAT_B8G8R8A8_SRGB;
        case ImageFormat_B8G8R8A8Unorm:
            return VK_FORMAT_B8G8R8A8_UNORM;

        case ImageFormat_R8G8B8A8SNorm:
            return VK_FORMAT_R8G8B8A8_SNORM;
        case ImageFormat_R8G8B8A8Srgb:
            return VK_FORMAT_R8G8B8A8_SRGB;
        case ImageFormat_R8G8B8A8Unorm:
            return VK_FORMAT_R8G8B8A8_UNORM;

        case ImageFormat_HalfVector4:
            return VK_FORMAT_R16G16B16A16_SFLOAT;
        case ImageFormat_R32Uint:
            return VK_FORMAT_R32_UINT;
        case ImageFormat_Vector4:
            return VK_FORMAT_R32G32B32A32_SFLOAT;

        case ImageFormat_Depth16:
            return VK_FORMAT_D16_UNORM;
        case ImageFormat_Depth16Stencil8:
            return VK_FORMAT_D16_UNORM_S8_UINT;
        case ImageFormat_Depth24Stencil8:
            return VK_FORMAT_D24_UNORM_S8_UINT;
        case ImageFormat_Depth32:
            return VK_FORMAT_D32_SFLOAT;
        
        default:
            return VK_FORMAT_UNDEFINED;
    }
}
inline VkPrimitiveTopology AstralCanvasVk_FromPrimitiveType(AstralCanvas::PrimitiveType primitiveType)
{
    switch (primitiveType)
    {
        case PrimitiveType_LineList:
            return VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
        case PrimitiveType_LineStrip:
            return VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;
        case PrimitiveType_TriangleList:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        case PrimitiveType_TriangleStrip:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;
        case PrimitiveType_TriangleFan:
            return VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;
    }
}
inline VkCullModeFlags AstralCanvasVk_FromCullMode(AstralCanvas::CullMode cullMode)
{
    switch (cullMode)
    {
        case CullMode_CullNone:
            return VK_CULL_MODE_NONE;
        case CullMode_CullClockwise:
            return VK_CULL_MODE_FRONT_BIT;
        case CullMode_CullCounterClockwise:
            return VK_CULL_MODE_BACK_BIT;
    }
}
inline VkBlendFactor AstralCanvasVk_FromBlend(Blend blend)
{
    switch (blend)
    {
        case Blend_DestinationAlpha:
            return VK_BLEND_FACTOR_DST_ALPHA;
        case Blend_DestinationColor:
            return VK_BLEND_FACTOR_DST_COLOR;
        case Blend_SourceAlpha:
            return VK_BLEND_FACTOR_SRC_ALPHA;
        case Blend_SourceColor:
            return VK_BLEND_FACTOR_SRC_COLOR;

        case Blend_InverseDestinationAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA;
        case Blend_InverseDestinationColor:
            return VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR;
        case Blend_InverseSourceAlpha:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
        case Blend_InverseSourceColor:
            return VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR;
        
        case Blend_One:
            return VK_BLEND_FACTOR_ONE;
        case Blend_Zero:
            return VK_BLEND_FACTOR_ZERO;
    }
}