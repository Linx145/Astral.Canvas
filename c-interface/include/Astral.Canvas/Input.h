#pragma once
#include "Linxc.h"

#ifdef __cplusplus
extern "C"
{
#endif

typedef struct
{
    float X;
    float Y;
} AstralCanvasJoystickAxis;

typedef enum
{
    AstralCanvas_ControllerButton_A,
    AstralCanvas_ControllerButton_B,
    AstralCanvas_ControllerButton_X,
    AstralCanvas_ControllerButton_Y,
    AstralCanvas_ControllerButton_L1,
    AstralCanvas_ControllerButton_R1,
    AstralCanvas_ControllerButton_Back,
    AstralCanvas_ControllerButton_Start,
    AstralCanvas_ControllerButton_Center,
    AstralCanvas_ControllerButton_LeftAxis,
    AstralCanvas_ControllerButton_RightAxis,
    AstralCanvas_ControllerButton_DpadUp,
    AstralCanvas_ControllerButton_DpadRight,
    AstralCanvas_ControllerButton_DpadDown,
    AstralCanvas_ControllerButton_DpadLeft
} AstralCanvas_ControllerButtons;
typedef enum
{
    AstralCanvas_Keys_Unknown = -1,
    AstralCanvas_Keys_Space = 32,
    AstralCanvas_Keys_Apostrophe = 39,
    AstralCanvas_Keys_Comma = 44,
    AstralCanvas_Keys_Minus = 45,
    AstralCanvas_Keys_Period = 46,
    AstralCanvas_Keys_Slash = 47,
    AstralCanvas_Keys_Number0 = 48,
    AstralCanvas_Keys_Number1 = 49,
    AstralCanvas_Keys_Number2 = 50,
    AstralCanvas_Keys_Number3 = 51,
    AstralCanvas_Keys_Number4 = 52,
    AstralCanvas_Keys_Number5 = 53,
    AstralCanvas_Keys_Number6 = 54,
    AstralCanvas_Keys_Number7 = 55,
    AstralCanvas_Keys_Number8 = 56,
    AstralCanvas_Keys_Number9 = 57,
    AstralCanvas_Keys_Semicolon = 59,
    AstralCanvas_Keys_Equal = 61,
    AstralCanvas_Keys_A = 65,
    AstralCanvas_Keys_B = 66,
    AstralCanvas_Keys_C = 67,
    AstralCanvas_Keys_D = 68,
    AstralCanvas_Keys_E = 69,
    AstralCanvas_Keys_F = 70,
    AstralCanvas_Keys_G = 71,
    AstralCanvas_Keys_H = 72,
    AstralCanvas_Keys_I = 73,
    AstralCanvas_Keys_J = 74,
    AstralCanvas_Keys_K = 75,
    AstralCanvas_Keys_L = 76,
    AstralCanvas_Keys_M = 77,
    AstralCanvas_Keys_N = 78,
    AstralCanvas_Keys_O = 79,
    AstralCanvas_Keys_P = 80,
    AstralCanvas_Keys_Q = 81,
    AstralCanvas_Keys_R = 82,
    AstralCanvas_Keys_S = 83,
    AstralCanvas_Keys_T = 84,
    AstralCanvas_Keys_U = 85,
    AstralCanvas_Keys_V = 86,
    AstralCanvas_Keys_W = 87,
    AstralCanvas_Keys_X = 88,
    AstralCanvas_Keys_Y = 89,
    AstralCanvas_Keys_Z = 90,
    AstralCanvas_Keys_LeftBracket = 91,
    AstralCanvas_Keys_BackSlash = 92,
    AstralCanvas_Keys_RightBracket = 93,
    AstralCanvas_Keys_GraveAccent = 96,
    AstralCanvas_Keys_World1 = 161,
    AstralCanvas_Keys_World2 = 162,
    AstralCanvas_Keys_Escape = 0x100,
    AstralCanvas_Keys_Enter = 257,
    AstralCanvas_Keys_Tab = 258,
    AstralCanvas_Keys_Backspace = 259,
    AstralCanvas_Keys_Insert = 260,
    AstralCanvas_Keys_Delete = 261,
    AstralCanvas_Keys_Right = 262,
    AstralCanvas_Keys_Left = 263,
    AstralCanvas_Keys_Down = 264,
    AstralCanvas_Keys_Up = 265,
    AstralCanvas_Keys_PageUp = 266,
    AstralCanvas_Keys_PageDown = 267,
    AstralCanvas_Keys_Home = 268,
    AstralCanvas_Keys_End = 269,
    AstralCanvas_Keys_CapsLock = 280,
    AstralCanvas_Keys_ScrollLock = 281,
    AstralCanvas_Keys_NumLock = 282,
    AstralCanvas_Keys_PrintScreen = 283,
    AstralCanvas_Keys_Pause = 284,
    AstralCanvas_Keys_F1 = 290,
    AstralCanvas_Keys_F2 = 291,
    AstralCanvas_Keys_F3 = 292,
    AstralCanvas_Keys_F4 = 293,
    AstralCanvas_Keys_F5 = 294,
    AstralCanvas_Keys_F6 = 295,
    AstralCanvas_Keys_F7 = 296,
    AstralCanvas_Keys_F8 = 297,
    AstralCanvas_Keys_F9 = 298,
    AstralCanvas_Keys_F10 = 299,
    AstralCanvas_Keys_F11 = 300,
    AstralCanvas_Keys_F12 = 301,
    AstralCanvas_Keys_F13 = 302,
    AstralCanvas_Keys_F14 = 303,
    AstralCanvas_Keys_F15 = 304,
    AstralCanvas_Keys_F16 = 305,
    AstralCanvas_Keys_F17 = 306,
    AstralCanvas_Keys_F18 = 307,
    AstralCanvas_Keys_F19 = 308,
    AstralCanvas_Keys_F20 = 309,
    AstralCanvas_Keys_F21 = 310,
    AstralCanvas_Keys_F22 = 311,
    AstralCanvas_Keys_F23 = 312,
    AstralCanvas_Keys_F24 = 313,
    AstralCanvas_Keys_F25 = 314,
    AstralCanvas_Keys_Keypad0 = 320,
    AstralCanvas_Keys_Keypad1 = 321,
    AstralCanvas_Keys_Keypad2 = 322,
    AstralCanvas_Keys_Keypad3 = 323,
    AstralCanvas_Keys_Keypad4 = 324,
    AstralCanvas_Keys_Keypad5 = 325,
    AstralCanvas_Keys_Keypad6 = 326,
    AstralCanvas_Keys_Keypad7 = 327,
    AstralCanvas_Keys_Keypad8 = 328,
    AstralCanvas_Keys_Keypad9 = 329,
    AstralCanvas_Keys_KeypadDecimal = 330,
    AstralCanvas_Keys_KeypadDivide = 331,
    AstralCanvas_Keys_KeypadMultiply = 332,
    AstralCanvas_Keys_KeypadSubtract = 333,
    AstralCanvas_Keys_KeypadAdd = 334,
    AstralCanvas_Keys_KeypadEnter = 335,
    AstralCanvas_Keys_KeypadEqual = 336,
    AstralCanvas_Keys_ShiftLeft = 340,
    AstralCanvas_Keys_ControlLeft = 341,
    AstralCanvas_Keys_AltLeft = 342,
    AstralCanvas_Keys_SuperLeft = 343,
    AstralCanvas_Keys_ShiftRight = 344,
    AstralCanvas_Keys_ControlRight = 345,
    AstralCanvas_Keys_AltRight = 346,
    AstralCanvas_Keys_SuperRight = 347,
    AstralCanvas_Keys_Menu = 348,
    AstralCanvas_Keys_LastKey = 348
} AstralCanvas_Keys;
typedef enum
{
    AstralCanvas_MouseButton_Left,
    AstralCanvas_MouseButton_Right,
    AstralCanvas_MouseButton_Middle
} AstralCanvas_MouseButtons;
typedef enum
{
    AstralCanvas_KeyState_None,
    AstralCanvas_KeyState_Released,
    AstralCanvas_KeyState_Pressed,
    AstralCanvas_KeyState_Repeat
} AstralCanvas_KeyState;

DynamicFunction bool AstralCanvasInput_IsKeyDown(AstralCanvas_Keys key);

DynamicFunction bool AstralCanvasInput_IsKeyPressed(AstralCanvas_Keys key);

DynamicFunction bool AstralCanvasInput_IsKeyReleased(AstralCanvas_Keys key);

DynamicFunction bool AstralCanvasInput_IsMouseDown(AstralCanvas_MouseButtons button);

DynamicFunction bool AstralCanvasInput_IsMousePressed(AstralCanvas_MouseButtons button);

DynamicFunction bool AstralCanvasInput_IsMouseReleased(AstralCanvas_MouseButtons button);

DynamicFunction void AstralCanvasInput_SimulateMousePress(AstralCanvas_MouseButtons button);

DynamicFunction void AstralCanvasInput_SimulateMouseRelease(AstralCanvas_MouseButtons button);

DynamicFunction bool AstralCanvasInput_ControllerIsButtonDown(const u32 controllerIndex, const AstralCanvas_ControllerButtons button);
DynamicFunction bool AstralCanvasInput_ControllerIsButtonPress(const u32 controllerIndex, const AstralCanvas_ControllerButtons button);
DynamicFunction bool AstralCanvasInput_ControllerIsButtonRelease(const u32 controllerIndex, const AstralCanvas_ControllerButtons button);

DynamicFunction bool AstralCanvasInput_ControllerIsR2Down(const u32 controllerIndex);
DynamicFunction float AstralCanvasInput_ControllerGetR2DownAmount(const u32 controllerIndex);
DynamicFunction bool AstralCanvasInput_ControllerIsL2Down(const u32 controllerIndex);
DynamicFunction float AstralCanvasInput_ControllerGetL2DownAmount(const u32 controllerIndex);

DynamicFunction bool AstralCanvasInput_ControllerIsConnected(const u32 controllerIndex);

DynamicFunction AstralCanvasJoystickAxis AstralCanvasInput_GetLeftStickAxis(const u32 controllerIndex);
DynamicFunction AstralCanvasJoystickAxis AstralCanvasInput_GetRightStickAxis(const u32 controllerIndex);

#ifdef __cplusplus
}
#endif