#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

def_delegate(AstralCanvas_ErrorCallback, void, const char*);

DynamicFunction void AstralCanvas_SetGlobalErrorCallback(AstralCanvas_ErrorCallback callback);

DynamicFunction void AstralCanvas_SetGlobalWarningCallback(AstralCanvas_ErrorCallback callback);

#ifdef __cplusplus
}
#endif