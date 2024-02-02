#pragma once
#ifdef MACOS
#include "Linxc.h"
#include "string.hpp"

void AstralCanvasMetal_DestroyShaderProgram(void* vertex, void* fragment);
bool AstralCanvasMetal_CreateShaderProgram(string vertexSource, string fragmentSource, void** vertexOut, void** fragmentOut);
#endif
