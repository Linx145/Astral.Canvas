#include "Graphics/CurrentBackend.hpp"

AstralCanvasGraphicsBackend AstralCanvas_GetActiveBackend()
{
    return AstralCanvas_Vulkan;
}
void AstralCanvas_SetActiveBackend(AstralCanvasGraphicsBackend backend)
{
    //do nothing atm
}