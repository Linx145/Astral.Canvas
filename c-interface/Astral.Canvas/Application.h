#pragma once
#include "Linxc.h"

typedef void *AstralCanvasApplication;

exportC void AstralCanvasApplication_Init();

exportC const char *AstralCanvasApplication_GetApplicationName();

exportC void AstralCanvasApplication_SetApplicationName(const char *name);

exportC void AstralCanvasApplication_GetEngineName();

exportC void AstralCanvasApplication_SetEngineName();