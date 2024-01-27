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
        Maths::Vec3 position;
        Maths::Vec4 color;
    };
    struct VertexPositionColorTexture
    {
        Maths::Vec3 position;
        Maths::Vec4 color;
        Maths::Vec2 UV;
    };
    struct VertexPositionTextureColor
    {
        Maths::Vec3 position;
        Maths::Vec2 UV;
        Maths::Vec4 color;
    };
    struct VertexPositionNormalTexture
    {
        Maths::Vec3 position;
        Maths::Vec3 normal;
        Maths::Vec2 UV;
    };

    VertexDeclaration *GetVertexPositionColorDecl();
    VertexDeclaration *GetVertexPositionColorTextureDecl();
    VertexDeclaration *GetVertexPositionTextureColorDecl();
    VertexDeclaration *GetVertexPositionNormalTextureDecl();
}