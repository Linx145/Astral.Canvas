#include "Graphics/CurrentBackend.hpp"

namespace AstralCanvas
{
    GraphicsBackend GetActiveBackend()
    {
        #ifdef MACOS
        return Backend_Metal;
        #endif
#ifdef ASTRALCANVAS_VULKAN
        return Backend_Vulkan;
#endif
        return Backend_OpenGL;
    }
    void AstralCanvas_SetActiveBackend(GraphicsBackend backend)
    {
        //do nothing atm
    }
}