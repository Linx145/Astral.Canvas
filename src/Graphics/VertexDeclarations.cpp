#include "Graphics/VertexDeclarations.hpp"

namespace AstralCanvas
{
    VertexDeclaration vertexPositionColorDecl = VertexDeclaration();
    VertexDeclaration vertexPositionColorTextureDecl = VertexDeclaration();
    VertexDeclaration vertexPositionTextureColorDecl = VertexDeclaration();
    VertexDeclaration vertexPositionNormalTextureDecl = VertexDeclaration();

    IAllocator vertexElementsAllocator = GetCAllocator();

    VertexDeclaration::VertexDeclaration()
    {
        this->allocator = NULL;
        this->elements = collections::vector<VertexElement>();
        this->inputRate = VertexInput_PerVertex;
        this->size = 0;
    }
    VertexDeclaration::VertexDeclaration(IAllocator *thisAllocator, usize thisSize, VertexInputRate thisInputRate)
    {
        this->allocator = thisAllocator;
        this->elements = collections::vector<VertexElement>(thisAllocator);
        this->inputRate = thisInputRate;
        this->size = thisSize;
    }
    void VertexDeclaration::VertexDeclaration::deinit()
    {
        this->elements.deinit();
    }

    VertexDeclaration *GetVertexPositionColorDecl()
    {
        if (vertexPositionColorDecl.allocator != NULL)
        {
            return &vertexPositionColorDecl;
        }
        vertexPositionColorDecl = VertexDeclaration(
            &vertexElementsAllocator, 
            sizeof(VertexPositionColor),
            VertexInput_PerVertex);

        vertexPositionColorDecl.elements.Add({VertexElement_Vector3, 0});
        vertexPositionColorDecl.elements.Add({VertexElement_Vector4, 12});

        return &vertexPositionColorDecl;
    }
    VertexDeclaration *GetVertexPositionColorTextureDecl()
    {
        if (vertexPositionColorTextureDecl.allocator != NULL)
        {
            return &vertexPositionColorTextureDecl;
        }
        vertexPositionColorTextureDecl = VertexDeclaration(
            &vertexElementsAllocator, 
            sizeof(VertexPositionColorTexture),
            VertexInput_PerVertex);

        vertexPositionColorTextureDecl.elements.Add({VertexElement_Vector3, 0});
        vertexPositionColorTextureDecl.elements.Add({VertexElement_Vector4, 12});
        vertexPositionColorTextureDecl.elements.Add({VertexElement_Vector2, 28});

        return &vertexPositionColorTextureDecl;
    }
    VertexDeclaration *GetVertexPositionTextureColorDecl()
    {
        if (vertexPositionTextureColorDecl.allocator != NULL)
        {
            return &vertexPositionTextureColorDecl;
        }
        vertexPositionTextureColorDecl = VertexDeclaration(
            &vertexElementsAllocator, 
            sizeof(VertexPositionTextureColor),
            VertexInput_PerVertex);

        vertexPositionTextureColorDecl.elements.Add({VertexElement_Vector3, 0});
        vertexPositionTextureColorDecl.elements.Add({VertexElement_Vector2, 12});
        vertexPositionTextureColorDecl.elements.Add({VertexElement_Vector4, 20});

        return &vertexPositionTextureColorDecl;
    }
    VertexDeclaration *GetVertexPositionNormalTextureDecl()
    {
        if (vertexPositionNormalTextureDecl.allocator != NULL)
        {
            return &vertexPositionNormalTextureDecl;
        }
        vertexPositionNormalTextureDecl = VertexDeclaration(
            &vertexElementsAllocator, 
            sizeof(VertexPositionNormalTexture),
            VertexInput_PerVertex);

        vertexPositionNormalTextureDecl.elements.Add({VertexElement_Vector3, 0});
        vertexPositionNormalTextureDecl.elements.Add({VertexElement_Vector3, 12});
        vertexPositionNormalTextureDecl.elements.Add({VertexElement_Vector2, 24});

        return &vertexPositionNormalTextureDecl;
    }
}