#pragma once

namespace AstralCanvas
{
    enum ShaderType
    {
        ShaderType_VertexFragment,
        ShaderType_Compute
    };
    enum ShaderInputAccessedBy
    {
        InputAccessedBy_None = 0,
        InputAccessedBy_Vertex = 1,
        InputAccessedBy_Fragment = 2,
        InputAccessedBy_Compute = 4
    };
    enum ShaderResourceType
    {
        ShaderResourceType_Uniform,
        ShaderResourceType_Sampler,
        ShaderResourceType_Texture,
        ShaderResourceType_StructuredBuffer
    };
    enum Blend
    {
        Blend_One,
        Blend_Zero,
        Blend_SourceColor,
        Blend_InverseSourceColor,
        Blend_SourceAlpha,
        Blend_InverseSourceAlpha,
        Blend_DestinationColor,
        Blend_InverseDestinationColor,
        Blend_DestinationAlpha,
        Blend_InverseDestinationAlpha
    };

    enum GraphicsBackend
    {
        Backend_Vulkan,
        Backend_Metal,
        Backend_D3D12
    };

    enum CullMode
    {
        CullMode_CullCounterClockwise,
        CullMode_CullClockwise,
        CullMode_CullNone
    };

    enum ImageFormat
    {
        ImageFormat_R8G8B8A8Unorm,
        ImageFormat_R8G8B8A8SNorm,
        ImageFormat_R8G8B8A8Srgb,
        ImageFormat_B8G8R8A8Unorm,
        ImageFormat_B8G8R8A8SNorm,
        ImageFormat_B8G8R8A8Srgb,
        ImageFormat_HalfVector4,
        ImageFormat_Vector4,
        ImageFormat_R32Uint,

        ImageFormat_DepthNone,
        ImageFormat_Depth16, 
        ImageFormat_Depth16Stencil8, 
        ImageFormat_Depth24Stencil8, 
        ImageFormat_Depth32
    };

    enum SampleMode
    {
        SampleMode_Point,
        SampleMode_Linear,
        SampleMode_Cubic
    };

    enum RepeatMode
    {
        RepeatMode_ClampToEdgeColor,
        RepeatMode_Repeat,
        RepeatMode_ClampToSpecificColor
    };

    enum IndexBufferSize
    {
        IndexBufferSize_U16, IndexBufferSize_U32
    };

    enum UniformType
    {
        UniformType_UniformBuffer,
        UniformType_Image,
        UniformType_ImageStorage,
        UniformType_Sampler,
        UniformType_StorageBuffer
    };

    enum VertexElementFormat
    {
        VertexElement_Float,
        VertexElement_Vector2,
        VertexElement_Vector3,
        VertexElement_Vector4,
        VertexElement_Int,
        VertexElement_Color,
        VertexElement_Uint
    };

    enum VertexInputRate
    {
        VertexInput_PerVertex,
        VertexInput_PerInstance
    };

    enum PrimitiveType
    {
        /// Renders the specified vertices as a sequence of isolated triangles. Each group of three vertices defines a separate triangle.
        PrimitiveType_TriangleList,

        /// Renders the vertices as a triangle strip.
        PrimitiveType_TriangleStrip,

        /// Renders the vertices as a list of isolated straight line segments; the count may be any positive integer.
        PrimitiveType_LineList,

        /// Renders the vertices as a single polyline; the count may be any positive integer.
        PrimitiveType_LineStrip,

        // Renders the vertices as individual points; the count may be any positive integer.
        //PrimitiveType_PointList,

        /// Renders the vertices as a series of triangle primitives connected to a central origin vertex
        PrimitiveType_TriangleFan,

        //PrimitiveType_TriangleListWithAdjacency,

        //PrimitiveType_LineStripWithAdjacency
    };
}