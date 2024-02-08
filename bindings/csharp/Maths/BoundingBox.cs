using System;
using System.Collections.Generic;
using System.Numerics;
using System.Runtime.CompilerServices;

namespace Astral.Canvas
{
    public struct BoundingBox
    {
        public Vector3 Min;
        public Vector3 Max;

        public BoundingBox(Vector3 min, Vector3 max)
        {
            this.Min = min;
            this.Max = max;
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public BoundingBox GetTransformedBounds(in Matrix4x4 matrix)
        {
            Span<Vector3> vertices = stackalloc Vector3[8];
            Vector3 min = new Vector3(float.MaxValue);
            Vector3 max = new Vector3(float.MinValue);
            GetVertices(vertices);
            for (int i = 0; i < 8; i++)
            {
                Vector3 transformed  = Vector3.Transform(vertices[i], matrix);
                min = Vector3.Min(transformed, min);
                max = Vector3.Max(transformed, max);
            }
            return new BoundingBox(min, max);
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public void GetVertices(Span<Vector3> span, in Matrix4x4 matrix)
        {
            GetVertices(span);
            for (int i = 0; i < 8; i++)
            {
                span[i] = Vector3.Transform(span[i], matrix);
            }
        }
        public void GetVertices(Span<Vector3> span)
        {
            //(min.x, min.y, min.z)(max.x, min.y, min.z)
            //(min.x, min.y, max.z)(max.x, min.y, max.z)
            //
            //(min.x, max.y, min.z)(max.x, max.y, min.z)
            //(min.x, max.y, max.z)(max.x, max.y, max.z)
            span[0] = Min;

            span[1].X = Max.X;
            span[1].Y = Min.Y;
            span[1].Z = Min.Z;

            span[2].X = Min.X;
            span[2].Y = Min.Y;
            span[2].Z = Max.Z;

            span[3].X = Max.X;
            span[3].Y = Min.Y;
            span[3].Z = Max.Z;

            span[4].X = Min.X;
            span[4].Y = Max.Y;
            span[4].Z = Min.Z;

            span[5].X = Max.X;
            span[5].Y = Max.Y;
            span[5].Z = Min.Z;

            span[6].X = Min.X;
            span[6].Y = Max.Y;
            span[6].Z = Max.Z;

            span[7] = Max;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool Contains(BoundingBox other)
        {
            if (other.Max.X < Min.X
                || other.Min.X > Max.X
                || other.Max.Y < Min.Y
                || other.Min.Y > Max.Y
                || other.Max.Z < Min.Z
                || other.Min.Z > Max.Z)
                return false;

            return true;
        }

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public bool Intersects(BoundingBox other)
        {
            if (other.Max.X <= Min.X
                || other.Min.X >= Max.X
                || other.Max.Y <= Min.Y
                || other.Min.Y >= Max.Y
                || other.Max.Z <= Min.Z
                || other.Min.Z >= Max.Z)
                return false;

            return true;
        }
    }
}
