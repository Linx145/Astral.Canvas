using System;

namespace Astral.Canvas
{
    public class VertexBuffer : IDisposable
    {
        public IntPtr handle;
        public int vertexCount;
        public VertexDeclaration vertexDeclaration;

        public VertexBuffer(VertexDeclaration vertexDeclaration, int vertexCount, bool isDynamic, bool canRead = false)
        {
            handle = AstralCanvas.VertexBuffer_Create(vertexDeclaration.handle, (UIntPtr)vertexCount, isDynamic, canRead);
            this.vertexDeclaration = vertexDeclaration;
            this.vertexCount = vertexCount;
        }
        public unsafe void SetData<T>(T[] vertices) where T : unmanaged
        {
            fixed (T* ptr = vertices)
            {
                AstralCanvas.VertexBuffer_SetData(handle, (IntPtr)ptr, (UIntPtr)vertices.Length);
            }
        }
        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                AstralCanvas.VertexBuffer_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }
    }
}
