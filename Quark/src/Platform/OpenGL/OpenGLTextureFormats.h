#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/TextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	constexpr GLenum GetTextureInternalFormat(TextureInternalFormat format)
	{
		switch (format)
		{
			case TextureInternalFormat::RGB8:            return GL_RGB8;
			case TextureInternalFormat::RGB10:           return GL_RGB10;
			case TextureInternalFormat::RGB12:           return GL_RGB12;
			case TextureInternalFormat::RGB16:           return GL_RGB16;
			case TextureInternalFormat::RGBA8:           return GL_RGBA8;
			case TextureInternalFormat::RGBA12:          return GL_RGBA12;
			case TextureInternalFormat::RGBA16:          return GL_RGBA16;
			case TextureInternalFormat::SRGB8:           return GL_SRGB8;
			case TextureInternalFormat::SRGBA8:          return GL_SRGB8_ALPHA8;
			case TextureInternalFormat::Red8:            return GL_R8;
			case TextureInternalFormat::RGB16f:          return GL_RGB16F;
			case TextureInternalFormat::RGB32f:          return GL_RGB32F;
			case TextureInternalFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
			default:
				QK_CORE_FATAL("Invalid internal texture format");
				return GL_NONE;
		}
	}

	constexpr GLenum GetTextureFormat(TextureDataFormat format)
	{
		switch (format)
		{
			case TextureDataFormat::RGB:  return GL_RGB;
			case TextureDataFormat::RGBA: return GL_RGBA;
			case TextureDataFormat::BGR:  return GL_BGR;
			case TextureDataFormat::BGRA: return GL_BGRA;
			case TextureDataFormat::Red:  return GL_RED;
			default:
				QK_CORE_FATAL("Invalid texture data format");
				return GL_NONE;
		}
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
			default:
				QK_CORE_FATAL("Invalid texture filtering mode");
				return GL_NONE;
		}
	}

	constexpr GLenum GetTextureTilingMode(TextureTilingMode mode)
	{
		switch (mode)
		{
			case TextureTilingMode::ClampToBorder: return GL_CLAMP_TO_BORDER;
			case TextureTilingMode::ClampToEdge:   return GL_CLAMP_TO_EDGE;
			case TextureTilingMode::Repeat:        return GL_REPEAT;
			default:
				QK_CORE_FATAL("Invalid texture tiling mode");
				return GL_NONE;
		}
	}

	constexpr bool IsTextureDepthFormat(TextureInternalFormat format)
	{
		switch (format)
		{
			case TextureInternalFormat::Depth24Stencil8: return true;
			default:                                     return false;
		}
	}

	constexpr bool IsTextureAlphaFormat(TextureDataFormat format)
	{
		switch (format)
		{
			case TextureDataFormat::RGBA: return true;
			default:                       return false;
		}
	}

	constexpr bool TextureHasMips(TextureFilteringMode mode)
	{
		switch (mode)
		{
			case TextureFilteringMode::Nearest:
			case TextureFilteringMode::Linear:               return false;
			case TextureFilteringMode::NearestMipmapNearest:
			case TextureFilteringMode::NearestMipmapLinear:
			case TextureFilteringMode::LinearMipmapNearest:
			case TextureFilteringMode::LinearMipmapLinear:   return true;
			default:
				QK_CORE_FATAL("Invalid texture filtering mode");
				return false;
		}
	}
}
