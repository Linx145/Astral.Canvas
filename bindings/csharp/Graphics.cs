using System;

namespace Astral.Canvas
{
    public class Graphics
    {
        public IntPtr handle;

        public Graphics(IntPtr handle)
        {
            this.handle = handle;
        }
        public Graphics(Application application)
        {
            handle = AstralCanvas.Application_GetGraphicsDevice(application.handle);
        }

        public void StartRenderProgram(RenderProgram program, Color clearColor)
        {
            AstralCanvas.Graphics_StartRenderProgram(handle, program.handle, clearColor);
        }
        public void EndRenderProgram()
        {
            AstralCanvas.Graphics_EndRenderProgram(handle);
        }

        public void UseRenderPipeline(RenderPipeline pipeline)
        {
            AstralCanvas.Graphics_UseRenderPipeline(handle, pipeline.handle);
        }

        public void SetVertexBuffer(VertexBuffer vertexBuffer, uint bindSlot)
        {
            AstralCanvas.Graphics_SetVertexBuffer(handle, vertexBuffer.handle, bindSlot);
        }
        public void SetIndexBuffer(IndexBuffer indexBuffer)
        {
            AstralCanvas.Graphics_SetIndexBuffer(handle, indexBuffer.handle);
        }
        public void DrawIndexedPrimitives(uint indexCount, uint instanceCount, uint firstIndex = 0, uint vertexOffset = 0, uint firstInstance = 0)
        {
            AstralCanvas.Graphics_DrawIndexedPrimitives(handle, indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
        }

        public unsafe void SetShaderVariables<T>(string variableName, ReadOnlySpan<T> span) where T : unmanaged
        {
            fixed (T* ptr = span)
            {
                AstralCanvas.Graphics_SetShaderVariable(handle, variableName, (IntPtr)ptr, (UIntPtr)span.Length);
            }
        }
        public unsafe void SetShaderVariable<T>(string variableName, T data) where T : unmanaged
        {
            T* ptr = &data;
            AstralCanvas.Graphics_SetShaderVariable(handle, variableName, (IntPtr)ptr, (UIntPtr)sizeof(T));
        }
        public unsafe void SetShaderVariableTexture(string variableName, Texture2D texture)
        {
            AstralCanvas.Graphics_SetShaderVariableTexture(handle, variableName, texture.handle);
        }
        public unsafe void SetShaderVariableTextures(string variableName, ReadOnlySpan<Texture2D> textures)
        {
            IntPtr* ptrs = stackalloc IntPtr[textures.Length];
            for (int i = 0; i < textures.Length; i++)
            {
                ptrs[i] = textures[i].handle;
            }
            AstralCanvas.Graphics_SetShaderVariableTextures(handle, variableName, (IntPtr)ptrs, (UIntPtr)textures.Length);
        }
        public unsafe void SetShaderVariableSampler(string variableName, SamplerState sampler)
        {
            AstralCanvas.Graphics_SetShaderVariableSampler(handle, variableName, sampler.handle);
        }
        public unsafe void SetShaderVariableSamplers(string variableName, ReadOnlySpan<SamplerState> samplers)
        {
            IntPtr* ptrs = stackalloc IntPtr[samplers.Length];
            for (int i = 0; i < samplers.Length; i++)
            {
                ptrs[i] = samplers[i].handle;
            }
            AstralCanvas.Graphics_SetShaderVariableSamplers(handle, variableName, (IntPtr)ptrs, (UIntPtr)samplers.Length);
        }
    }
}
