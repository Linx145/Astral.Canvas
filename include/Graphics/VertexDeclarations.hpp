#pragma once
#include "Linxc.h"
#include "Enums.hpp"
#include "vector.hpp"
#include "Maths/Vec2.hpp"
#include "Maths/Vec4.hpp"

namespace AstralCanvas
{
    struct VertexElement
    {
        VertexElementFormat format;
        usize offset;
    };
    struct VertexDeclaration
    {
        IAllocator *allocator;
        collections::vector<VertexElement> elements;
        usize size;
        VertexInputRate inputRate;

        void deinit();
        VertexDeclaration();
        VertexDeclaration(IAllocator *thisAllocator, usize size, VertexInputRate inputRate);
    };

    struct VertexPositionColor
    {
        alignas(0) Maths::Vec3 position;
        alignas(16) Maths::Vec4 color;
    };
    struct VertexPositionColorTexture
    {
        alignas(0) Maths::Vec3 position;
        alignas(16) Maths::Vec4 color;
        alignas(32) Maths::Vec2 UV;
    };
    struct VertexPositionTextureColor
    {
        alignas(0) Maths::Vec3 position;
        alignas(16) Maths::Vec2 UV;
        alignas(32) Maths::Vec4 color;
    };
    struct VertexPositionNormalTexture
    {
        alignas(0) Maths::Vec3 position;
        alignas(16) Maths::Vec3 normal;
        alignas(32) Maths::Vec2 UV;
    };

    VertexDeclaration *GetVertexPositionColorDecl();
    VertexDeclaration *GetVertexPositionColorTextureDecl();
    VertexDeclaration *GetVertexPositionTextureColorDecl();
    VertexDeclaration *GetVertexPositionNormalTextureDecl();
    VertexDeclaration *GetInstanceDataMatrixDecl();
}