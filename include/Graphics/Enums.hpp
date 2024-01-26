#pragma once

namespace AstralCanvas
{
    enum BlendState
    {
        BlendState_One,
        BlendState_Zero,
        BlendState_SourceColor,
        BlendState_InverseSourceColor,
        BlendState_SourceAlpha,
        BlendState_InverseSourceAlpha,
        BlendState_DestinationColor,
        BlendState_InverseDestinationColor,
        BlendState_DestinationAlpha,
        BlendState_InverseDestinationAlpha
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
}