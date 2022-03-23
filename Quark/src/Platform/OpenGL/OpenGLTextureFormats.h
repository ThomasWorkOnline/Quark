#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/TextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	constexpr GLenum GetTextureSampleMode(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	constexpr GLenum GetTextureInternalFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGB8:				return GL_RGB8;
			case TextureFormat::RGBA8:				return GL_RGBA8;
			case TextureFormat::SRGB8:				return GL_SRGB8;
			case TextureFormat::SRGBA8:				return GL_SRGB8_ALPHA8;
			case TextureFormat::Depth24Stencil8:	return GL_DEPTH24_STENCIL8;
		}

		QK_FATAL("Invalid internal texture format");
		return GL_NONE;
	}

	constexpr GLenum GetTextureFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGB8:	return GL_RGB;
			case TextureFormat::RGBA8:	return GL_RGBA;
			case TextureFormat::SRGB8:	return GL_RGB;
			case TextureFormat::SRGBA8:	return GL_RGBA;
		}

		QK_FATAL("Invalid texture color format");
		return GL_NONE;
	}

	constexpr GLenum GetTextureFilteringMode(TextureFilteringMode mode)
	{
		switch (mode)
		{
			case TextureFilteringMode::Nearest:					return GL_NEAREST;
			case TextureFilteringMode::Linear:					return GL_LINEAR;
			case TextureFilteringMode::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
			case TextureFilteringMode::NearestMipmapLinear:		return GL_NEAREST_MIPMAP_LINEAR;
			case TextureFilteringMode::LinearMipmapNearest:		return GL_LINEAR_MIPMAP_NEAREST;
			case TextureFilteringMode::LinearMipmapLinear:		return GL_LINEAR_MIPMAP_LINEAR;
		}

		QK_FATAL("Invalid texture filtering mode");
		return GL_NONE;
	}

	constexpr GLenum GetTextureTilingMode(TextureTilingMode mode)
	{
		switch (mode)
		{
			case TextureTilingMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
			case TextureTilingMode::ClampToEdge:   return GL_CLAMP_TO_EDGE;
			case TextureTilingMode::Repeat:        return GL_REPEAT;
		}

		QK_FATAL("Invalid texture tiling mode");
		return GL_NONE;
	}

	constexpr bool IsTextureDepthFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::Depth24Stencil8: return true;
		}
		return false;
	}

	constexpr bool IsTextureAlphaFormat(TextureFormat format)
	{
		switch (format)
		{
			case TextureFormat::RGBA8:	return true;
			case TextureFormat::SRGBA8:	return true;
		}
		return false;
	}
}
