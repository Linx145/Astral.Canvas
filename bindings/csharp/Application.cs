using System;
using System.Runtime.InteropServices;

namespace Astral.Canvas
{
    public class Application
    {
        public IntPtr handle;
        public int windowsCount;
        public Graphics graphicsDevice;

        public Application(string appName, string engineName, uint appVersion, uint engineVersion, float fps)
        {
            handle = AstralCanvas.Application_Init(appName, engineName, appVersion, engineVersion, fps);
            graphicsDevice = new Graphics(this);
        }

        public string applicationName
        {
            get
            {
                return Marshal.PtrToStringAnsi(AstralCanvas.Application_GetApplicationName(handle));
            }
        }
        public string engineName
        {
            get
            {
                return Marshal.PtrToStringAnsi(AstralCanvas.Application_GetEngineName(handle));
            }
        }
        public float framesPerSecond
        {
            get
            {
                return AstralCanvas.Application_GetFramesPerSecond(handle);
            }
            set
            {
                AstralCanvas.Application_SetFramesPerSecond(handle, value);
            }
        }
        public void AddWindow(int width, int height, bool resizeable)
        {
            AstralCanvas.Application_AddWindow(handle, width, height, resizeable);
            windowsCount += 1;
        }
        public Window GetWindow(int index)
        {
            if (index >= windowsCount)
            {
                throw new IndexOutOfRangeException();
            }
            return new Window(AstralCanvas.Application_GetWindow(handle, (uint)index));
        }

        public void Run(AstralCanvas.UpdateFunction onUpdate, AstralCanvas.UpdateFunction onDraw, AstralCanvas.InitFunction onProgramInitialize, AstralCanvas.DeinitFunction onProgramEnd)
        {
            AstralCanvas.Application_Run(handle, onUpdate, onDraw, onProgramInitialize, onProgramEnd);
        }
    }
}
