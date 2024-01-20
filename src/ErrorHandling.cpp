#include "ErrorHandling.hpp"

AstralCanvasErrorCallback AstralCanvas_GlobalErrorCallback = &AstralDebugger_DefaultOnError;
AstralCanvasErrorCallback AstralCanvas_GlobalWarningCallback = &AstralDebugger_DefaultOnWarning;

AstralCanvasErrorCallback AstralCanvas_GetGlobalErrorCallback()
{
	return AstralCanvas_GlobalErrorCallback;
}
void AstralCanvas_SetGlobalErrorCallback(AstralCanvasErrorCallback callback)
{
	AstralCanvas_GlobalErrorCallback = callback;
}

AstralCanvasErrorCallback AstralCanvas_GetGlobalWarningCallback()
{
	return AstralCanvas_GlobalWarningCallback;
}
void AstralCanvas_SetGlobalWarningCallback(AstralCanvasErrorCallback callback)
{
	AstralCanvas_GlobalWarningCallback = callback;
}