using System;
using System.Runtime.InteropServices;

namespace Astral.Canvas
{
    [StructLayout(LayoutKind.Explicit)]
    public struct BlendState
    {
        [FieldOffset(0)]
        Blend srcColor;
        [FieldOffset(4)]
        Blend destColor;
        [FieldOffset(8)]
        Blend srcAlpha;
        [FieldOffset(12)]
        Blend destAlpha;

        public BlendState(Blend src, Blend dest)
        {
            srcColor = src;
            destColor = dest;
            srcAlpha = src;
            destAlpha = dest;
        }
        /*Additive = new BlendState(Blend.SourceAlpha, Blend.One);
        AlphaBlend = new BlendState(Blend.One, Blend.InverseSourceAlpha);
        NonPremultiplied = new BlendState(Blend.SourceAlpha, Blend.InverseSourceAlpha);
        Opaque = new BlendState(Blend.One, Blend.Zero);*/
        public static readonly BlendState Additive = new BlendState(Blend.SourceAlpha, Blend.One);
        public static readonly BlendState AlphaBlend = new BlendState(Blend.One, Blend.InverseSourceAlpha);
        public static readonly BlendState NonPremultiplied = new BlendState(Blend.SourceAlpha, Blend.InverseSourceAlpha);
        public static readonly BlendState Opaque = new BlendState(Blend.One, Blend.Zero);
    }
    public class RenderPipeline : IDisposable
    {
        public IntPtr handle;
        public Shader shader;
        public BlendState blendState;
        public bool depthWrite;
        public bool depthTest;
        public CullMode cullMode;
        public PrimitiveType primitiveType;
        public VertexDeclaration[] vertexDeclarations;

        public unsafe RenderPipeline(Shader shader,
            CullMode pipelineCullMode,
            PrimitiveType pipelinePrimitiveType,
            BlendState pipelineBlendState,
            bool testDepth,
            bool writeToDepth,
            VertexDeclaration[] vertexDeclarations)
        {
            Span<IntPtr> vertexHandles = stackalloc IntPtr[8];
            for (int i = 0; i < vertexDeclarations.Length; i++)
            {
                vertexHandles[i] = vertexDeclarations[i].handle;
            }
            fixed (IntPtr* ptr = vertexHandles)
            {
                handle = AstralCanvas.RenderPipeline_Init(shader.handle, pipelineCullMode, pipelinePrimitiveType, pipelineBlendState, testDepth, writeToDepth, ptr, (UIntPtr)vertexDeclarations.Length);
            }
            this.cullMode = pipelineCullMode;
            this.primitiveType = pipelinePrimitiveType;
            this.blendState = pipelineBlendState;
            this.depthTest = testDepth;
            this.depthWrite = writeToDepth;
            this.vertexDeclarations = vertexDeclarations;
        }
        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                AstralCanvas.RenderPipeline_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }
    }
}
