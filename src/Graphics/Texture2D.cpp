#include "Graphics/Texture2D.hpp"

Texture2D AstralCanvas_CreateTextureFromData(u8* data, usize width, usize height, SamplerState *samplerForTexture, ImageFormat imageFormat, bool storeData)
{
    Texture2D result = {};
    result.width = width;
    result.height = height;
    result.ownsHandle = true;
    result.bytes = data;
}