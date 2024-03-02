#pragma once
#include "Linxc.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

namespace AstralCanvas
{
    def_delegate(ErrorCallback, void, const char*);

    void Debugger_Log(const char *message, bool logTiming);
    void Debugger_DefaultOnError(const char *message);
    void Debugger_DefaultOnWarning(const char *message);

    AstralCanvas::ErrorCallback GetGlobalErrorCallback();
    void SetGlobalErrorCallback(AstralCanvas::ErrorCallback callback);

    AstralCanvas::ErrorCallback GetGlobalWarningCallback();
    void SetGlobalWarningCallback(AstralCanvas::ErrorCallback callback);
}

#define THROW_ERR(message) { AstralCanvas::ErrorCallback callback = AstralCanvas::GetGlobalErrorCallback(); if (callback) callback(message);}
#define LOG_WARNING(message) { AstralCanvas::ErrorCallback callback = AstralCanvas::GetGlobalWarningCallback(); if (callback) callback(message);}