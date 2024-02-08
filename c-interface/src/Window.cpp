#include "Astral.Canvas/Window.h"
#include "Windowing/Window.hpp"

exportC AstralCanvasPoint2 AstralCanvasWindow_GetResolution(AstralCanvasWindow ptr)
{
    return {((AstralCanvas::Window *)ptr)->resolution.X, ((AstralCanvas::Window *)ptr)->resolution.Y};
}

exportC void AstralCanvasWindow_SetResolution(AstralCanvasWindow ptr, AstralCanvasPoint2 resolution)
{
    glfwSetWindowSize(((AstralCanvas::Window *)ptr)->handle, resolution.X, resolution.Y);
    ((AstralCanvas::Window *)ptr)->resolution.X = resolution.X;
    ((AstralCanvas::Window *)ptr)->resolution.Y = resolution.Y;
}

exportC AstralCanvasPoint2 AstralCanvasWindow_GetPosition(AstralCanvasWindow ptr)
{
    return {((AstralCanvas::Window *)ptr)->position.X, ((AstralCanvas::Window *)ptr)->position.Y};
}

exportC void AstralCanvasWindow_SetPosition(AstralCanvasWindow ptr, AstralCanvasPoint2 position)
{
    glfwSetWindowPos(((AstralCanvas::Window *)ptr)->handle, position.X, position.Y);
    ((AstralCanvas::Window *)ptr)->position.X = position.X;
    ((AstralCanvas::Window *)ptr)->position.Y = position.Y;
}

exportC AstralCanvasRectangle AstralCanvasWindow_AsRectangle(AstralCanvasWindow ptr)
{
    Maths::Rectangle rect = ((AstralCanvas::Window *)ptr)->AsRectangle();
    return {rect.X, rect.Y, rect.Width, rect.Height};
}

exportC void AstralCanvasWindow_Deinit(AstralCanvasWindow ptr)
{
    ((AstralCanvas::Window *)ptr)->deinit();
}