#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

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

#ifdef __cplusplus
}
#endif