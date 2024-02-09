﻿using System;

namespace Astral.Canvas
{
    public class Shader : IDisposable
    {
        public IntPtr handle;

        public Shader()
        {
            handle = IntPtr.Zero;
        }
        public unsafe Shader(ShaderType shaderType, string jsonString)
        {
            IntPtr temp = IntPtr.Zero;
            int result = AstralCanvas.Shader_FromString(shaderType, jsonString, &temp);
            if (result != 0 || temp == IntPtr.Zero)
            {
                throw new InvalidOperationException("Error loading shader with error code " + result.ToString());
            }

            handle = temp;
        }
        public IntPtr ShaderModule1
        {
            get
            {
                return AstralCanvas.Shader_GetModule1(handle);
            }
        }
        public IntPtr ShaderModule2
        {
            get
            {
                return AstralCanvas.Shader_GetModule2(handle);
            }
        }
        public IntPtr ShaderPipelineLayout
        {
            get
            {
                return AstralCanvas.Shader_GetPipelineLayout(handle);
            }
        }
        public ShaderType type
        {
            get
            {
                return AstralCanvas.Shader_GetType(handle);
            }
        }
        public IntPtr pipelineLayout
        {
            get
            {
                return AstralCanvas.Shader_GetPipelineLayout(handle);
            }
        }
        public void Dispose()
        {
            if (handle != IntPtr.Zero)
            {
                AstralCanvas.Shader_Deinit(handle);
            }
            handle = IntPtr.Zero;
        }
        public int GetVariableBindings(string variableName)
        {
            return AstralCanvas.Shader_GetVariableBinding(handle, variableName);
        }

    }
}
