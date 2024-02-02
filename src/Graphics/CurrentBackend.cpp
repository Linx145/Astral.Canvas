#include "Graphics/CurrentBackend.hpp"

namespace AstralCanvas
{
    GraphicsBackend GetActiveBackend()
    {
        #ifdef MACOS
        return Backend_Metal;
        #endif
        return Backend_Vulkan;
    }
    void AstralCanvas_SetActiveBackend(GraphicsBackend backend)
    {
        //do nothing atm
    }
}