#pragma once

#include "Quark/Core/Core.h"

#include "Formats.h"

namespace Quark {

	// Supported APIs
	enum class RHI
	{
		None = 0,
		Direct3D12,
		Metal,
		OpenGL,
		Vulkan
	};

	struct RHIVersion
	{
		uint32_t Major;
		uint32_t Minor;
		uint32_t Patch;
	};

	struct FramebufferCapabilities
	{
		uint32_t    MaxWidth;
		uint32_t    MaxHeight;
		uint32_t    MaxLayers;
		uint32_t    MaxAttachments;
		SampleCount MaxMSAA;
	};

	struct SamplerCapabilities
	{
		uint32_t MaxTextureUnits;
		float    MaxAnisotropy;
	};

	struct UniformBufferCapabilities
	{
		size_t   MaxBufferSize;
		uint32_t MaxPerStageBuffers;
	};

	struct TextureCapabilities
	{
		uint32_t Max2DSize;
		uint32_t Max3DSize;
		uint32_t MaxArrayLayers;
		uint32_t MaxCubemapSize;
	};

	struct GraphicsAPICapabilities
	{
		FramebufferCapabilities    Framebuffer;
		SamplerCapabilities        Sampler;
		TextureCapabilities        Texture;
		UniformBufferCapabilities  UniformBuffer;
	};
}
