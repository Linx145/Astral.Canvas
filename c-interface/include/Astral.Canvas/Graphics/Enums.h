#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef enum
{
    AstralCanvas_ShaderType_VertexFragment,
    AstralCanvas_ShaderType_Compute
} AstralCanvas_ShaderType;
typedef enum
{
    AstralCanvas_InputAccessedBy_None = 0,
    AstralCanvas_InputAccessedBy_Vertex = 1,
    AstralCanvas_InputAccessedBy_Fragment = 2,
    AstralCanvas_InputAccessedBy_Compute = 4
} AstralCanvas_ShaderInputAccessedBy;
typedef enum
{
    AstralCanvas_ShaderResourceType_Uniform,
    AstralCanvas_ShaderResourceType_Sampler,
    AstralCanvas_ShaderResourceType_Texture,
    AstralCanvas_ShaderResourceType_StructuredBuffer
} AstralCanvas_ShaderResourceType;
typedef enum
{
    AstralCanvas_Blend_Disable,
    AstralCanvas_Blend_One,
    AstralCanvas_Blend_Zero,
    AstralCanvas_Blend_SourceColor,
    AstralCanvas_Blend_InverseSourceColor,
    AstralCanvas_Blend_SourceAlpha,
    AstralCanvas_Blend_InverseSourceAlpha,
    AstralCanvas_Blend_DestinationColor,
    AstralCanvas_Blend_InverseDestinationColor,
    AstralCanvas_Blend_DestinationAlpha,
    AstralCanvas_Blend_InverseDestinationAlpha
} AstralCanvas_Blend;

/// How the render pass within a program will output it's results
typedef enum
{
    /// The render pass is not the last in sequence and thus should output to the next pass (Default)
    AstralCanvas_RenderPassOutput_ToNextPass,
    /// The render pass is the last in sequence and the program should output to a render target
    AstralCanvas_RenderPassOutput_ToRenderTarget,
    /// The render pass is the last in sequence and the program should output to the window
    AstralCanvas_RenderPassOutput_ToWindow
} AstralCanvas_RenderPassOutputType;

typedef enum
{
    AstralCanvas_Backend_Vulkan,
    AstralCanvas_Backend_Metal,
    AstralCanvas_Backend_D3D12
} AstralCanvas_GraphicsBackend;

typedef enum
{
    AstralCanvas_CullMode_CullCounterClockwise,
    AstralCanvas_CullMode_CullClockwise,
    AstralCanvas_CullMode_CullNone
} AstralCanvas_CullMode;

typedef enum
{
    AstralCanvas_ImageFormat_Undefined,
    AstralCanvas_ImageFormat_R8G8B8A8Unorm,
    AstralCanvas_ImageFormat_R8G8B8A8SNorm,
    AstralCanvas_ImageFormat_R8G8B8A8Srgb,
    AstralCanvas_ImageFormat_B8G8R8A8Unorm,
    AstralCanvas_ImageFormat_B8G8R8A8SNorm,
    AstralCanvas_ImageFormat_B8G8R8A8Srgb,
    AstralCanvas_ImageFormat_HalfVector4,
    AstralCanvas_ImageFormat_Vector4,
    AstralCanvas_ImageFormat_R32Uint,
    AstralCanvas_ImageFormat_DepthNone,
    AstralCanvas_ImageFormat_Depth16, 
    AstralCanvas_ImageFormat_Depth16Stencil8, 
    AstralCanvas_ImageFormat_Depth24Stencil8, 
    AstralCanvas_ImageFormat_Depth32,
    AstralCanvas_ImageFormat_BackbufferFormat
} AstralCanvas_ImageFormat;

typedef enum
{
    AstralCanvas_SampleMode_Point,
    AstralCanvas_SampleMode_Linear,
    AstralCanvas_SampleMode_Cubic
} AstralCanvas_SampleMode;

typedef enum
{
    AstralCanvas_RepeatMode_ClampToEdgeColor,
    AstralCanvas_RepeatMode_Repeat,
    AstralCanvas_RepeatMode_ClampToSpecificColor
} AstralCanvas_RepeatMode;

typedef enum
{
    AstralCanvas_IndexBufferSize_U16, 
    AstralCanvas_IndexBufferSize_U32
} AstralCanvas_IndexBufferSize;

typedef enum
{
    AstralCanvas_UniformType_UniformBuffer,
    AstralCanvas_UniformType_Image,
    AstralCanvas_UniformType_ImageStorage,
    AstralCanvas_UniformType_Sampler,
    AstralCanvas_UniformType_StorageBuffer
} AstralCanvas_UniformType;

typedef enum
{
    AstralCanvas_VertexElement_Float,
    AstralCanvas_VertexElement_Vector2,
    AstralCanvas_VertexElement_Vector3,
    AstralCanvas_VertexElement_Vector4,
    AstralCanvas_VertexElement_Int,
    AstralCanvas_VertexElement_Color,
    AstralCanvas_VertexElement_Uint
} AstralCanvas_VertexElementFormat;

typedef enum
{
    AstralCanvas_VertexInput_PerVertex,
    AstralCanvas_VertexInput_PerInstance
} AstralCanvas_VertexInputRate;

typedef enum
{
    /// Renders the specified vertices as a sequence of isolated triangles. Each group of three vertices defines a separate triangle.
    AstralCanvas_PrimitiveType_TriangleList,

    /// Renders the vertices as a triangle strip.
    AstralCanvas_PrimitiveType_TriangleStrip,

    /// Renders the vertices as a list of isolated straight line segments; the count may be any positive integer.
    AstralCanvas_PrimitiveType_LineList,

    /// Renders the vertices as a single polyline; the count may be any positive integer.
    AstralCanvas_PrimitiveType_LineStrip,

    /// Renders the vertices as a series of triangle primitives connected to a central origin vertex
    AstralCanvas_PrimitiveType_TriangleFan,
} AstralCanvas_PrimitiveType;

#ifdef __cplusplus
}
#endif