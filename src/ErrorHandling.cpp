#include "ErrorHandling.hpp"

void AstralCanvas::Debugger_DefaultOnError(const char* message)
{
	AstralCanvas::Debugger_Log(message, true);
	getchar();
	exit(1);
}
void AstralCanvas::Debugger_DefaultOnWarning(const char* message)
{
	AstralCanvas::Debugger_Log(message, true);
}

AstralCanvas::ErrorCallback AstralCanvas_GlobalErrorCallback = &AstralCanvas::Debugger_DefaultOnError;
AstralCanvas::ErrorCallback AstralCanvas_GlobalWarningCallback = &AstralCanvas::Debugger_DefaultOnWarning;

void AstralCanvas::Debugger_Log(const char *message, bool logTiming)
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

AstralCanvas::ErrorCallback AstralCanvas::GetGlobalErrorCallback()
{
	return AstralCanvas_GlobalErrorCallback;
}
void AstralCanvas::SetGlobalErrorCallback(AstralCanvas::ErrorCallback callback)
{
	AstralCanvas_GlobalErrorCallback = callback;
}

AstralCanvas::ErrorCallback AstralCanvas::GetGlobalWarningCallback()
{
	return AstralCanvas_GlobalWarningCallback;
}
void AstralCanvas::SetGlobalWarningCallback(AstralCanvas::ErrorCallback callback)
{
	AstralCanvas_GlobalWarningCallback = callback;
}