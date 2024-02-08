#include "Astral.Canvas/Input.h"
#include "Input/Input.hpp"

exportC bool AstralCanvasInput_IsKeyDown(AstralCanvas_Keys key)
{
    return AstralCanvas::Input_IsKeyDown((AstralCanvas::Keys)key);
}

exportC bool AstralCanvasInput_IsKeyPressed(AstralCanvas_Keys key)
{
    return AstralCanvas::Input_IsKeyPressed((AstralCanvas::Keys)key);
}

exportC bool AstralCanvasInput_IsKeyReleased(AstralCanvas_Keys key)
{
    return AstralCanvas::Input_IsKeyReleased((AstralCanvas::Keys)key);
}

exportC bool AstralCanvasInput_IsMouseDown(AstralCanvas_MouseButtons button)
{
    return AstralCanvas::Input_IsMouseDown((AstralCanvas::MouseButtons)button);
}

exportC bool AstralCanvasInput_IsMousePressed(AstralCanvas_MouseButtons button)
{
    return AstralCanvas::Input_IsMousePressed((AstralCanvas::MouseButtons)button);
}

exportC bool AstralCanvasInput_IsMouseReleased(AstralCanvas_MouseButtons button)
{
    return AstralCanvas::Input_IsMouseReleased((AstralCanvas::MouseButtons)button);
}

exportC void AstralCanvasInput_SimulateMousePress(AstralCanvas_MouseButtons button)
{
    AstralCanvas::Input_SimulateMousePress((AstralCanvas::MouseButtons)button);
}

exportC void AstralCanvasInput_SimulateMouseRelease(AstralCanvas_MouseButtons button)
{
    AstralCanvas::Input_SimulateMouseRelease((AstralCanvas::MouseButtons)button);
}

exportC bool AstralCanvasInput_ControllerIsButtonDown(const u32 controllerIndex, const AstralCanvas_ControllerButtons button)
{
    return AstralCanvas::Input_ControllerIsButtonDown(controllerIndex, (AstralCanvas::ControllerButtons)button);
}
exportC bool AstralCanvasInput_ControllerIsButtonPress(const u32 controllerIndex, const AstralCanvas_ControllerButtons button)
{
    return AstralCanvas::Input_ControllerIsButtonPress(controllerIndex, (AstralCanvas::ControllerButtons)button);
}
exportC bool AstralCanvasInput_ControllerIsButtonRelease(const u32 controllerIndex, const AstralCanvas_ControllerButtons button)
{
    return AstralCanvas::Input_ControllerIsButtonRelease(controllerIndex, (AstralCanvas::ControllerButtons)button);
}
exportC bool AstralCanvasInput_ControllerIsR2Down(const u32 controllerIndex)
{
    return AstralCanvas::Input_ControllerIsR2Down(controllerIndex);
}
exportC float AstralCanvasInput_ControllerGetR2DownAmount(const u32 controllerIndex)
{
    return AstralCanvas::Input_ControllerGetR2DownAmount(controllerIndex);
}
exportC bool AstralCanvasInput_ControllerIsL2Down(const u32 controllerIndex)
{
    return AstralCanvas::Input_ControllerIsL2Down(controllerIndex);
}
exportC float AstralCanvasInput_ControllerGetL2DownAmount(const u32 controllerIndex)
{
    return AstralCanvas::Input_ControllerGetL2DownAmount(controllerIndex);
}
exportC bool AstralCanvasInput_ControllerIsConnected(const u32 controllerIndex)
{
    return AstralCanvas::Input_ControllerIsConnected(controllerIndex);
}
exportC AstralCanvasJoystickAxis AstralCanvasInput_GetLeftStickAxis(const u32 controllerIndex)
{
    Maths::Vec2 result = AstralCanvas::Input_GetLeftStickAxis(controllerIndex);
    return {result.X, result.Y};
}
exportC AstralCanvasJoystickAxis AstralCanvasInput_GetRightStickAxis(const u32 controllerIndex)
{
    Maths::Vec2 result = AstralCanvas::Input_GetRightStickAxis(controllerIndex);
    return {result.X, result.Y};
}