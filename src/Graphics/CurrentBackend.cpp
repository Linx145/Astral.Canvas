#include "Graphics/CurrentBackend.hpp"

namespace AstralCanvas
{
    GraphicsBackend GetActiveBackend()
    {
        return Backend_Vulkan;
    }
    void AstralCanvas_SetActiveBackend(GraphicsBackend backend)
    {
        //do nothing atm
    }
}