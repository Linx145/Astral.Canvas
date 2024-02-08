using System;

namespace Astral.Canvas
{
    public class IndexBuffer : IDisposable
    {
        public IntPtr handle;
        public IndexBufferSize indexSize;
        public uint indexCount;

        public IndexBuffer(IndexBufferSize indexSize, uint indexCount)
        {
            this.indexSize = indexSize;
            this.indexCount= indexCount;
            this.handle = AstralCanvas.IndexBuffer_Create(indexSize, (UIntPtr)indexCount);
        }
        public unsafe void SetData<T>(T[] indices) where T : unmanaged
        {
            fixed (T* ptr = indices)
            {
                AstralCanvas.IndexBuffer_SetData(handle, (IntPtr)ptr, (UIntPtr)(sizeof(T) * indices.Length));
            }
        }

        public void Dispose()
        {
            if (handle !=  IntPtr.Zero)
            {
                AstralCanvas.IndexBuffer_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }
    }
}
