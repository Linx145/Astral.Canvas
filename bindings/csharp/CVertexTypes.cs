using System;
using System.Runtime.InteropServices;
using System.Numerics;

namespace Astral.Canvas
{
    [StructLayout(LayoutKind.Explicit, Size  = 32)]
    public struct VertexPositionColor
    {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(16)]
        public Vector4 color;

        public VertexPositionColor(Vector3 position, Vector4 color)
        {
            this.position = position;
            this.color = color;
        }

        public static readonly VertexDeclaration Declaration = new VertexDeclaration(AstralCanvas.GetVertexPositionColorDecl());
    }

    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct VertexPositionColorTexture
    {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(16)]
        public Vector4 color;
        [FieldOffset(32)]
        public Vector2 texture;

        public VertexPositionColorTexture(Vector3 position, Vector4 color, Vector2 texture)
        {
            this.position = position;
            this.color = color;
            this.texture = texture;
        }

        public static readonly VertexDeclaration Declaration = new VertexDeclaration(AstralCanvas.GetVertexPositionColorTextureDecl());
    }

    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct VertexPositionTextureColor
    {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(16)]
        public Vector2 texture;
        [FieldOffset(32)]
        public Vector4 color;

        public VertexPositionTextureColor(Vector3 position, Vector2 texture, Vector4 color)
        {
            this.position = position;
            this.texture = texture;
            this.color = color;
        }

        public static readonly VertexDeclaration VertexPositionTextureColorDecl = new VertexDeclaration(AstralCanvas.GetVertexPositionTextureColorDecl());
    }

    [StructLayout(LayoutKind.Explicit, Size = 48)]
    public struct VertexPositionNormalTexture
    {
        [FieldOffset(0)]
        public Vector3 position;
        [FieldOffset(16)]
        public Vector3 normal;
        [FieldOffset(32)]
        public Vector2 texture;

        public VertexPositionNormalTexture(Vector3 position, Vector3 normal, Vector2 texture)
        {
            this.position = position;
            this.normal = normal;
            this.texture = texture;
        }

        public static readonly VertexDeclaration VertexPositionNormalTextureDecl = new VertexDeclaration(AstralCanvas.GetVertexPositionNormalTextureDecl());
    }
}
