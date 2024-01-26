#pragma once
#include "Graphics/Enums.hpp"

namespace AstralCanvas
{
    GraphicsBackend GetActiveBackend();
    void SetActiveBackend(GraphicsBackend backend);
}