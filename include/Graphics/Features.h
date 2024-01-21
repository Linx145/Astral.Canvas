#pragma once

enum AstralCanvas_GraphicsFeatures
{
    //Allows multidraw indirect calls
    AstralCanvas_Feature_MultidrawIndirect,
    //Allows arrays of dissimilar-sized textures in shaders
    AstralCanvas_Feature_TextureArrays,
    //Allows indexing of arrays using any integer variable/value, not just uniforms (non-changing shader inputs)
    AstralCanvas_Feature_ArrayDynamicIndexing
};