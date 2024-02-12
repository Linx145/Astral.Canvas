#pragma once
#ifdef ASTRALCANVAS_METAL
#import "Metal/Metal.h"
#include "Graphics/Enums.hpp"

inline MTLPixelFormat AstralCanvasMetal_FromImageFormat(AstralCanvas::ImageFormat format)
{
    switch (format)
    {
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8SNorm:
            return MTLPixelFormatInvalid;
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Srgb:
            return MTLPixelFormatBGRA8Unorm_sRGB;
        case AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Unorm:
            return MTLPixelFormatBGRA8Unorm;

        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8SNorm:
            return MTLPixelFormatRGBA8Snorm;
        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Srgb:
            return MTLPixelFormatRGBA8Unorm_sRGB;
        case AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Unorm:
            return MTLPixelFormatRGBA8Unorm;

        case AstralCanvas::ImageFormat::ImageFormat_HalfVector4:
            return MTLPixelFormatRGBA16Float;
        case AstralCanvas::ImageFormat::ImageFormat_R32Uint:
            return MTLPixelFormatR32Uint;
        case AstralCanvas::ImageFormat::ImageFormat_Vector4:
            return MTLPixelFormatRGBA32Float;

        case AstralCanvas::ImageFormat::ImageFormat_Depth16:
            return MTLPixelFormatDepth16Unorm;
        case AstralCanvas::ImageFormat::ImageFormat_Depth16Stencil8:
            
        case AstralCanvas::ImageFormat::ImageFormat_Depth24Stencil8:
            return MTLPixelFormatDepth24Unorm_Stencil8;
        case AstralCanvas::ImageFormat::ImageFormat_Depth32:
            return MTLPixelFormatDepth32Float;
        
        default:
            return MTLPixelFormatInvalid;
    }
}

inline AstralCanvas::ImageFormat AstralCanvasMetal_ToImageFormat(MTLPixelFormat format)
{
    switch (format)
    {
        case MTLPixelFormatInvalid:
            return AstralCanvas::ImageFormat::ImageFormat_Undefined;
        case MTLPixelFormatBGRA8Unorm_sRGB:
            return AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Srgb;
        case MTLPixelFormatBGRA8Unorm:
            return AstralCanvas::ImageFormat::ImageFormat_B8G8R8A8Unorm;

        case MTLPixelFormatRGBA8Snorm:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8SNorm;
        case MTLPixelFormatRGBA8Unorm_sRGB:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Srgb;
        case MTLPixelFormatRGBA8Unorm:
            return AstralCanvas::ImageFormat::ImageFormat_R8G8B8A8Unorm;

        case MTLPixelFormatRGBA16Float:
            return AstralCanvas::ImageFormat::ImageFormat_HalfVector4;
        case MTLPixelFormatR32Uint:
            return AstralCanvas::ImageFormat::ImageFormat_R32Uint;
        case MTLPixelFormatRGBA32Float:
            return AstralCanvas::ImageFormat::ImageFormat_Vector4;

        case MTLPixelFormatDepth16Unorm:
            return AstralCanvas::ImageFormat::ImageFormat_Depth16;
        case MTLPixelFormatDepth24Unorm_Stencil8:
            return AstralCanvas::ImageFormat::ImageFormat_Depth24Stencil8;
        case MTLPixelFormatDepth32Float:
            return AstralCanvas::ImageFormat::ImageFormat_Depth32;
        
        default:
            return AstralCanvas::ImageFormat::ImageFormat_Undefined;
    }
}

inline MTLCullMode AstralCanvasMetal_FromCullMode(AstralCanvas::CullMode cullMode)
{
    switch (cullMode)
    {
        case AstralCanvas::CullMode_CullCounterClockwise:
            return MTLCullModeBack;
        case AstralCanvas::CullMode_CullClockwise:
            return MTLCullModeFront;
        case AstralCanvas::CullMode_CullNone:
            return MTLCullModeNone;
    }
}

inline MTLSamplerAddressMode AstralCanvasMetal_FromRepeatMode(AstralCanvas::RepeatMode repeatMode)
{
    switch (repeatMode)
    {
        case AstralCanvas::RepeatMode_ClampToEdgeColor:
            return MTLSamplerAddressModeClampToEdge;
        case AstralCanvas::RepeatMode_Repeat:
            return MTLSamplerAddressModeRepeat;
        case AstralCanvas::RepeatMode_ClampToSpecificColor:
            return MTLSamplerAddressModeClampToBorderColor;
    }
}
inline MTLSamplerMinMagFilter AstralCanvasMetal_FromSampleMode(AstralCanvas::SampleMode sampleMode)
{
    switch (sampleMode)
    {
        case AstralCanvas::SampleMode_Point:
            return MTLSamplerMinMagFilterNearest;
        case AstralCanvas::SampleMode_Linear:
            return MTLSamplerMinMagFilterLinear;
        case AstralCanvas::SampleMode_Cubic:
            return MTLSamplerMinMagFilterLinear;
    }
}

inline MTLVertexFormat AstralCanvasMetal_FromVertexElementFormat(AstralCanvas::VertexElementFormat format)
{
    switch (format)
    {
        case AstralCanvas::VertexElementFormat::VertexElement_Float:
            return MTLVertexFormatFloat;

        case AstralCanvas::VertexElementFormat::VertexElement_Vector2:
            return MTLVertexFormatFloat2;

        case AstralCanvas::VertexElementFormat::VertexElement_Vector3:
            return MTLVertexFormatFloat3;

        case AstralCanvas::VertexElementFormat::VertexElement_Vector4:
            return MTLVertexFormatFloat4;

        case AstralCanvas::VertexElementFormat::VertexElement_Int:
            return MTLVertexFormatInt;

        case AstralCanvas::VertexElementFormat::VertexElement_Color:
            return MTLVertexFormatInvalid;

        case AstralCanvas::VertexElementFormat::VertexElement_Uint:
            return MTLVertexFormatUInt;
    }
}
#endif