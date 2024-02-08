using System;

namespace Astral.Canvas
{
    public class RenderProgram
    {
        public IntPtr handle;

        public RenderProgram()
        {
            handle = AstralCanvas.RenderProgram_Init();
        }
        public int AddAttachment(ImageFormat imageFormat, bool clearColor, bool clearDepth, RenderPassOutputType outputType)
        {
            return AstralCanvas.RenderProgram_AddAttachment(handle, imageFormat, clearColor, clearDepth, outputType);
        }
        public void AddRenderPass(int colorAttachmentIndex, int depthAttachmentIndex = -1)
        {
            AstralCanvas.RenderProgram_AddRenderPass(handle, colorAttachmentIndex, depthAttachmentIndex);
        }
        public unsafe void AddRenderPasses(ReadOnlySpan<int> colorAttachmentIndex, int depthAttachmentIndex = -1)
        {
            fixed (int* ptr = colorAttachmentIndex)
            {
                AstralCanvas.RenderProgram_AddRenderPasses(handle, (IntPtr)ptr, (UIntPtr)colorAttachmentIndex.Length, depthAttachmentIndex);
            }
        }
        public void Construct()
        {
            AstralCanvas.RenderProgram_Construct(handle);
        }
        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                AstralCanvas.RenderProgram_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }
    }
}
