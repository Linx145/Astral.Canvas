using System;
using System.Runtime.InteropServices;

namespace Astral.Canvas
{
    [StructLayout(LayoutKind.Explicit)]
    public struct VertexElement
    {
        [FieldOffset(0)]
        VertexElementFormat format;
        [FieldOffset(4)]
        UIntPtr offset;

        public VertexElement(VertexElementFormat format, UIntPtr offset)
        {
            this.format = format;
            this.offset = offset;
        }
    }
    public class VertexDeclaration
    {
        public IntPtr handle;
        public VertexElement[] elements;
        public uint size;
        public VertexInputRate inputRate;

        public VertexDeclaration(IntPtr handle, uint size, params VertexElement[] elements)
        {
            this.handle = handle;
            this.size = size;
            this.elements = elements;
        }
        public unsafe VertexDeclaration(IntPtr handle)
        {
            this.handle = handle;
            this.size = (uint)AstralCanvas.VertexDeclaration_GetElementSize(handle);
            this.inputRate = AstralCanvas.VertexDeclaration_GetInputRate(handle);

            UIntPtr outputCount = UIntPtr.Zero;
            AstralCanvas.VertexDeclaration_GetElements(handle, &outputCount, null);

            elements = new VertexElement[(int)outputCount];
            fixed (VertexElement* ptr = elements)
            {
                AstralCanvas.VertexDeclaration_GetElements(handle, &outputCount, ptr);
            }
        }
    }
}
