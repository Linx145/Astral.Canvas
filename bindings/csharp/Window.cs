using System;

namespace Astral.Canvas
{
    public struct Window
    {
        public IntPtr handle;

        public Window()
        {
            handle = IntPtr.Zero;
        }
        public Window(IntPtr handle)
        {
            this.handle = handle; 
        }

        public Point resolution
        {
            get
            {
                return AstralCanvas.Window_GetResolution(handle);
            }
            set
            {
                AstralCanvas.Window_SetResolution(handle, value);
            }
        }
        public Point position
        {
            get
            {
                return AstralCanvas.Window_GetPosition(handle);
            }
            set
            {
                AstralCanvas.Window_SetPosition(handle, value);
            }
        }
    }
}
