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
    }
}
