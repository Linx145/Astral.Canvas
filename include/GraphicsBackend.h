#pragma once
enum AstralGraphicsBackend
{
	Astral_InvalidGraphicsBackend,
	/// <summary>
	/// Use this when available (Should be everywhere except Mac, unless your driver doesn't support it)
	/// </summary>
	Astral_Vulkan,
	/// <summary>
	/// Use this on Mac
	/// </summary>
	Astral_Metal,
	/// <summary>
	/// Use this for web/older systems. Technically an abstraction layer by itself, which may impact performance (Except on web)
	/// </summary>
	Astral_WGPU
};