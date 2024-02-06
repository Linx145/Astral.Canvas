#pragma once
#include "Linxc.h"

extern "C"
{
    typedef union
    {
        struct
        {
            u8 R;
            u8 G;
            u8 B;
            u8 A;
        };
        u32 packed;
    } AstralCanvasColor;
}