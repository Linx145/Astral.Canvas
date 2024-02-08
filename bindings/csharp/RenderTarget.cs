using System;

namespace Astral.Canvas
{
    public class RenderTarget : IDisposable
    {
        public IntPtr handle;
        public Texture2D backendTexture;
        public Texture2D depthBuffer;
        public uint width;
        public uint height;
        public bool isBackbuffer;
        public bool hasBeenUsedBefore
        {
            get
            {
                return AstralCanvas.RenderTarget_GetUseStatus(handle);
            }
            set
            {
                AstralCanvas.RenderTarget_SetUseStatus(handle, value);
            }
        }

        public RenderTarget(Texture2D backendTexture, Texture2D depthBuffer, bool isBackbuffer)
        {
            this.handle = AstralCanvas.RenderTarget_CreateFromTextures(backendTexture.handle, depthBuffer == null ? IntPtr.Zero : depthBuffer.handle, isBackbuffer);
            this.backendTexture = backendTexture;
            this.depthBuffer = depthBuffer;
            this.width = backendTexture.width;
            this.height = backendTexture.height;
            this.isBackbuffer = isBackbuffer;
        }
        public RenderTarget(uint width, uint height, ImageFormat imageFormat, ImageFormat depthFormat)
        {
            this.handle = AstralCanvas.RenderTarget_Create(width, height, imageFormat, depthFormat);
            this.backendTexture = new Texture2D(AstralCanvas.RenderTarget_GetHandle(handle));
            this.depthBuffer = new Texture2D(AstralCanvas.RenderTarget_GetDepthBuffer(handle));
            this.width = width;
            this.height = height;
            this.isBackbuffer = false;
        }

        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                //also deinits backend texture and depth buffer
                AstralCanvas.RenderTarget_Deinit(handle);
                this.backendTexture = null;
                this.depthBuffer = null;
            }
            handle = IntPtr.Zero;
        }
    }
}
