using System;

namespace Astral.Canvas
{
    public class SamplerState : IDisposable
    {
        public IntPtr handle { get; private set; }
        public readonly SampleMode sampleMode;
        public readonly bool isAnisotropic;
        public readonly RepeatMode repeatMode;
        public readonly float anisotropyLevel;
        /*public Blend sourceAlphaBlend;
        public Blend destAlphaBlend;
        public Blend sourceColorBlend;
        public Blend destColorBlend;*/
        public SamplerState()
        {
            handle = IntPtr.Zero;
            sampleMode = SampleMode.Linear;
            isAnisotropic = false;
            repeatMode = RepeatMode.Repeat;
            anisotropyLevel = 0f;
        }
        public SamplerState(IntPtr handle, SampleMode sampleMode, RepeatMode repeatMode, bool isAnisotropic, float anisotropyLevel)
        {
            this.handle = handle;
            this.sampleMode = sampleMode;
            this.repeatMode = repeatMode;
            this.isAnisotropic = isAnisotropic;
            this.anisotropyLevel = anisotropyLevel;
        }
        public SamplerState(SampleMode sampleMode, RepeatMode repeatMode, bool isAnisotropic, float anisotropyLevel) //Blend sourceColorBlend, Blend destColorBlend, Blend sourceAlphaBlend, Blend destAlphaBlend
        {
            this.handle = AstralCanvas.SamplerState_Init(sampleMode, repeatMode, isAnisotropic, anisotropyLevel);
            this.sampleMode = sampleMode;
            this.repeatMode = repeatMode;
            this.isAnisotropic = isAnisotropic;
            this.anisotropyLevel = anisotropyLevel;
        }

        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                AstralCanvas.SamplerState_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }

        /*public static readonly SamplerState PointClamp = new SamplerState(AstralCanvas.Sampler_GetPointClamp(), SampleMode.Point, RepeatMode.ClampToEdgeColor, false, 0f);

        public static readonly SamplerState LinearClamp = new SamplerState(AstralCanvas.Sampler_GetLinearClamp(), SampleMode.Linear, RepeatMode.ClampToEdgeColor, false, 0f);

        public static readonly SamplerState PointWrap = new SamplerState(AstralCanvas.Sampler_GetPointWrap(), SampleMode.Point, RepeatMode.Repeat, false, 0f);

        public static readonly SamplerState LinearWrap = new SamplerState(AstralCanvas.Sampler_GetLinearWrap(), SampleMode.Linear, RepeatMode.Repeat, false, 0f);*/
    }
}
