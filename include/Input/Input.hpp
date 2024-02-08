#pragma once
#include "Application.hpp"
#include "Input/InputState.hpp"
#include "Maths/Vec2.hpp"

namespace AstralCanvas
{
    inline AstralCanvas::Window *Input_GetWindow()
    {
        return &AstralCanvas::GetAppInstance()->windows.ptr[0];
    }
    inline bool Input_IsKeyDown(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyDown(key);
    }
    inline bool Input_IsKeyPressed(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyPressed(key);
    }
    inline bool Input_IsKeyReleased(const Keys key)
    {
        return Input_GetWindow()->windowInputState.IsKeyReleased(key);
    }

    inline bool Input_IsMouseDown(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseDown(button);
    }
    inline bool Input_IsMousePressed(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMousePressed(button);
    }
    inline bool Input_IsMouseReleased(const MouseButtons button)
    {
        return Input_GetWindow()->windowInputState.IsMouseReleased(button);
    }

    inline void Input_SimulateMousePress(const MouseButtons button)
    {
        Input_GetWindow()->windowInputState.SimulateMousePress(button);
    }
    inline void Input_SimulateMouseRelease(const MouseButtons button)
    {
        Input_GetWindow()->windowInputState.SimulateMouseRelease(button);
    }

    inline bool Input_ControllerIsButtonDown(const u32 controllerIndex, const ControllerButtons button)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].buttonStates[(usize)button];
    }
    inline bool Input_ControllerIsButtonPress(const u32 controllerIndex, const ControllerButtons button)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].buttonStates[(usize)button]
        && !Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].buttonStates[(usize)button];
    }
    inline bool Input_ControllerIsButtonRelease(const u32 controllerIndex, const ControllerButtons button)
    {
        return !Input_GetWindow()->windowInputState.controllerStates[controllerIndex].buttonStates[(usize)button]
        && Input_GetWindow()->windowInputState.oldControllerStates[controllerIndex].buttonStates[(usize)button];
    }

    inline bool Input_ControllerIsR2Down(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].R2DownAmount > -0.99f;
    }
    inline float Input_ControllerGetR2DownAmount(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].R2DownAmount;
    }

    inline bool Input_ControllerIsL2Down(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].L2DownAmount > -0.99f;
    }
    inline float Input_ControllerGetL2DownAmount(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].L2DownAmount;
    }

    inline bool Input_ControllerIsConnected(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].connected;
    }
    inline Maths::Vec2 Input_GetLeftStickAxis(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].leftStickAxis;
    }
    inline Maths::Vec2 Input_GetRightStickAxis(const u32 controllerIndex)
    {
        return Input_GetWindow()->windowInputState.controllerStates[controllerIndex].rightStickAxis;
    }
}