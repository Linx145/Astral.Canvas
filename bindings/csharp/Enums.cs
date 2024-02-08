namespace Astral.Canvas
{
    public enum ShaderType
    {
        VertexFragment,
        Compute
    }
    public enum ShaderInputAccessedBy
    {
        None,
        Vertex,
        Fragment,
        Compute
    }
    public enum ShaderResourceType
    {
        Uniform,
        Sampler,
        Texture,
        StructuredBuffer
    }
    public enum Blend
    {
        Disable,
        One,
        Zero,
        SourceColor,
        InverseSourceColor,
        SourceAlpha,
        InverseSourceAlpha,
        DestinationColor,
        InverseDestinationColor,
        DestinationAlpha,
        InverseDestinationAlpha
    }
    public enum RenderPassOutputType
    {
        ToNextPass,
        ToRenderTarget,
        ToWindow
    }
    public enum GraphicsBackend
    {
        Vulkan,
        Metal,
        D3D12
    }
    public enum CullMode
    {
        CullCounterClockwise,
        CullClockwise,
        CullNone
    }
    public enum ImageFormat
    {
        Undefined,
        R8G8B8A8Unorm,
        R8G8B8A8SNorm,
        R8G8B8A8Srgb,
        B8G8R8A8Unorm,
        B8G8R8A8SNorm,
        B8G8R8A8Srgb,
        HalfVector4,
        Vector4,
        R32Uint,
        DepthNone,
        Depth16,
        Depth16Stencil8,
        Depth24Stencil8,
        Depth32,
        BackbufferFormat
    }
    public enum SampleMode
    {
        Point,
        Linear,
        Cubic
    }
    public enum RepeatMode
    {
        ClampToEdgeColor,
        Repeat,
        ClampToSpecificColor
    }
    public enum IndexBufferSize
    {
        UInt16,
        UInt32
    }
    public enum UniformType
    {
        UniformBuffer,
        Image,
        StorageImage,
        Sampler,
        StorageBuffer
    }
    public enum VertexElementFormat
    {
        Float,
        Vector2,
        Vector3,
        Vector4,
        Int,
        Color,
        Uint
    }
    public enum VertexInputRate
    {
        PerVertex,
        PerInstance
    }
    public enum PrimitiveType
    {
        TriangleList,
        TriangleStrip,
        LineList,
        LineStrip,
        TriangleFan
    }
}
