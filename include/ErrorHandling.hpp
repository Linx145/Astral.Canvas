#pragma once
#ifndef ASTRALCANVAS_ERRORHANDLING_H
#define ASTRALCANVAS_ERRORHANDLING_H
#include "Linxc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

def_delegate(AstralCanvasErrorCallback, void, const char*);

inline void AstralDebugger_Log(const char* message, bool logTiming = true)
{
	if (logTiming)
	{
		time_t now = time(NULL);
		char* nowStr = ctime(&now);
		printf("[%s]: %s\n", nowStr, message);
	}
	else
	{
		printf("%s\n", message);
	}
}

inline void AstralDebugger_DefaultOnError(const char* message)
{
	AstralDebugger_Log(message);
	exit(1);
}
inline void AstralDebugger_DefaultOnWarning(const char* message)
{
	AstralDebugger_Log(message);
}
AstralCanvasErrorCallback AstralCanvas_GetGlobalErrorCallback();
void AstralCanvas_SetGlobalErrorCallback(AstralCanvasErrorCallback callback);

AstralCanvasErrorCallback AstralCanvas_GetGlobalWarningCallback();
void AstralCanvas_SetGlobalWarningCallback(AstralCanvasErrorCallback callback);

#define THROW_ERR(message) { AstralCanvasErrorCallback callback = AstralCanvas_GetGlobalErrorCallback(); if (callback) callback(message);}
#define LOG_WARNING(message) { AstralCanvasErrorCallback callback = AstralCanvas_GetGlobalWarningCallback(); if (callback) callback(message);}

#endif