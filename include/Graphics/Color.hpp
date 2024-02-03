#pragma once
#include "Linxc.h"
#include "Maths/Vec4.hpp"

#define ONE_OVER_255 1.0f / 255.0f
namespace AstralCanvas
{
    struct Color
    {
        u8 R;
        u8 G;
        u8 B;
        u8 A;

        inline Color()
        {
            R = 0;
            G = 0;
            B = 0;
            A = 0;
        }
        inline Color(u8 r, u8 g, u8 b)
        {
            R = r;
            G = g;
            B = b;
            A = 255;
        }
        inline Color(u8 r, u8 g, u8 b, u8 a)
        {
            R = r;
            G = g;
            B = b;
            A = a;
        }
        inline Color(Maths::Vec4 vec4)
        {
            R = (u8)(vec4.X * 255.0f);
            G = (u8)(vec4.Y * 255.0f);
            B = (u8)(vec4.Z * 255.0f);
            A = (u8)(vec4.W * 255.0f);
        }
        /// Creates a color from a hexadecimal value, formatted as AABBGGRR
        inline Color FromInverseHexadecimal(u32 hexadecimal)
        {
            R = (u8)((hexadecimal) & 0xFF);
            G = (u8)((hexadecimal >> 8) & 0xFF);
            B = (u8)((hexadecimal >> 16) & 0xFF);
            A = (u8)((hexadecimal >> 24) & 0xFF);
        }
        /// Creates a color from a hexadecimal value, formatted as RRGGBBAA
        inline Color FromHexadecimal(u32 hexadecimal)
        {
            A = (u8)((hexadecimal) & 0xFF);
            B = (u8)((hexadecimal >> 8) & 0xFF);
            G = (u8)((hexadecimal >> 16) & 0xFF);
            R = (u8)((hexadecimal >> 24) & 0xFF);
        }
        inline Maths::Vec4 ToVector4()
        {
            const float oneOver255 = ONE_OVER_255;
            return Maths::Vec4((float)R * oneOver255, (float)G * oneOver255, (float)B * oneOver255, (float)A * oneOver255);
        }
        inline bool operator==(Color other)
        {
            return this->R == other.R && this->G == other.G && this->B == other.B && this->A == other.A;
        }
        inline bool operator!=(Color other)
        {
            return this->R != other.R || this->G != other.G || this->B != other.B || this->A != other.A;
        }
        inline Color operator*(float amount)
        {
            return Color((u8)((float)R * amount), (u8)((float)G * amount), (u8)((float)B * amount), A);
        }
        inline void operator*=(float amount)
        {
            R = (u32)(R * amount);
            G = (u32)(G * amount);
            B = (u32)(B * amount);
        }
    };
}

#define COLOR_TRANSPARENT AstralCanvas::Color()
#define COLOR_BLACK AstralCanvas::Color(0, 0, 0)
#define COLOR_WHITE AstralCanvas::Color(255, 255, 255)
#define COLOR_RED AstralCanvas::Color(255, 0, 0)
#define COLOR_GREEN AstralCanvas::Color(0, 255, 0)
#define COLOR_BLUE AstralCanvas::Color(0, 0, 255)