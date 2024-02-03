#pragma once
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
