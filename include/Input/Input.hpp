#pragma once
#include "Application.hpp"
#include "Input/InputState.hpp"

namespace AstralCanvas
{
    inline AstralCanvasWindow *Input_GetWindow()
    {
        return &AstralCanvas_GetAppInstance()->windows.ptr[0];
    }
    inline bool Input_IsKeyDown(Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyDown(key);
    }
    inline bool Input_IsKeyPressed(Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyPressed(key);
    }
    inline bool Input_IsKeyReleased(Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyReleased(key);
    }

    inline bool Input_IsMouseDown(MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseDown(button);
    }
    inline bool Input_IsMousePressed(MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMousePressed(button);
    }
    inline bool Input_IsMouseReleased(MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseReleased(button);
    }
}