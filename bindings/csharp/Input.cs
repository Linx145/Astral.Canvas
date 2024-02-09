using System;
using System.Runtime.CompilerServices;
using System.Numerics;

namespace Astral.Canvas
{
    public static class Input
    {

        /// <summary>
        /// Returns true if a key is just pressed or is being held down.
        /// </summary>
        /// <param name="key">The key to check</param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsKeyDown(Keys key) => AstralCanvas.Input_IsKeyDown(key);

        /// <summary>
        /// Returns true for a frame if a key has just been pressed
        /// </summary>
        /// <param name="key">The key to check</param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsKeyPressed(Keys key) => AstralCanvas.Input_IsKeyPressed(key);

        /// <summary>
        /// Returns true for a frame if a key has just been released
        /// </summary>
        /// <param name="key">The key to check</param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsKeyReleased(Keys key) => AstralCanvas.Input_IsKeyReleased(key);

        /// <summary>
        /// Returns true if the specified mouse button is held down.
        /// </summary>
        /// <param name="button">The mouse button to check</param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsMouseDown(MouseButtons button) => AstralCanvas.Input_IsMouseDown(button);

        /// <summary>
        /// Returns true for a frame if the specified mouse button has just been pressed
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsMousePressed(MouseButtons button) => AstralCanvas.Input_IsMousePressed(button);

        /// <summary>
        /// Returns true for a frame if the specified mouse button has just been released
        /// </summary>
        /// <param name="button"></param>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool IsMouseReleased(MouseButtons button) => AstralCanvas.Input_IsMouseReleased(button);
        /// <summary>
        /// Returns a character whenever the user types an input/holds down the button
        /// </summary>
        /// <returns></returns>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static uint AwaitInputChar() => AstralCanvas.Input_AwaitInputChar();

        public static Vector2 mousePosition => AstralCanvas.Input_GetMousePosition();
        public static Vector2 worldMousePosition;

        /// <summary>
        /// Simulates the user pressing the mouse, thus IsMouseDown will become 
        /// true and IsMousePressed as well, for that frame. 
        /// <br>Up to the user to call SimulateMouseRelease to release the mouse.</br>
        /// </summary>
        /// <param name="button"></param>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void SimulateMousePress(MouseButtons button) => AstralCanvas.Input_SimulateMousePress(button);
        /// <summary>
        /// Simulates the user releasing the mouse, thus IsMouseDown will become 
        /// false and IsMouseReleased as well, for that frame. 
        /// <br>Up to the user to call SimulateMouseRelease to release the mouse.</br>
        /// </summary>
        /// <param name="button"></param>
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void SimulateMouseRelease(MouseButtons button) => AstralCanvas.Input_SimulateMouseRelease(button);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static void SetMousePosition(Vector2 position) => AstralCanvas.Input_SetMousePosition(position);
        public static Vector2 mouseScroll => AstralCanvas.Input_GetMouseScroll();

        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsConnected(uint controllerID) => AstralCanvas.Input_ControllerIsConnected(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 ControllerGetLeftStickAxis(uint controllerID) => AstralCanvas.Input_GetLeftStickAxis(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static Vector2 ControllerGetRightStickAxis(uint controllerID) => AstralCanvas.Input_GetRightStickAxis(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float ControllerGetL2DownAmount(uint controllerID) => AstralCanvas.Input_ControllerGetL2DownAmount(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static float ControllerGetR2DownAmount(uint controllerID) => AstralCanvas.Input_ControllerGetR2DownAmount(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsL2Down(uint controllerID) => AstralCanvas.Input_ControllerIsL2Down(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsR2Down(uint controllerID) => AstralCanvas.Input_ControllerIsR2Down(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsL2Pressed(uint controllerID) => AstralCanvas.Input_ControllerIsL2Pressed(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsR2Pressed(uint controllerID) => AstralCanvas.Input_ControllerIsR2Pressed(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsL2Released(uint controllerID) => AstralCanvas.Input_ControllerIsL2Released(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsR2Released(uint controllerID) => AstralCanvas.Input_ControllerIsR2Released(controllerID);
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsButtonDown(uint controllerID, ControllerButtons button)
        {
            return AstralCanvas.Input_ControllerIsButtonDown(controllerID, button);
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsButtonPressed(uint controllerID, ControllerButtons button)
        {
            return AstralCanvas.Input_ControllerIsButtonPress(controllerID, button);
        }
        [MethodImpl(MethodImplOptions.AggressiveInlining)]
        public static bool ControllerIsButtonReleased(uint controllerID, ControllerButtons button)
        {
            return AstralCanvas.Input_ControllerIsButtonRelease(controllerID, button);
        }
    }
}
