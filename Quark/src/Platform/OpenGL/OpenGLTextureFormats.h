#pragma once

#include "../../Quark/Core/Core.h"
#include "../../Quark/Renderer/TextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	static GLenum GetTextureSampleMode(bool multisampled)
	{
		return multisampled ? GL_TEXTURE_2D_MULTISAMPLE : GL_TEXTURE_2D;
	}

	static GLenum GetTextureInternalFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::RGB8:				return GL_RGB8;
		case TextureDataFormat::RGBA8:				return GL_RGBA8;
		case TextureDataFormat::Depth24Stencil8:	return GL_DEPTH24_STENCIL8;
		}

		QK_FATAL("Invalid internal texture format");
		return GL_NONE;
	}

	static GLenum GetTextureDataFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::RGB8:	return GL_RGB;
		case TextureDataFormat::RGBA8:	return GL_RGBA;
		}

		QK_FATAL("Invalid texture color format");
		return GL_NONE;
	}

	static GLenum GetTextureFilteringMode(TextureFilteringMode mode)
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

	static GLenum GetTextureTilingMode(TextureTilingMode mode)
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

	static bool IsTextureDepthFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::Depth24Stencil8: return true;
		}
		return false;
	}

	static bool IsTextureAlphaFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::RGBA8:	return true;
		}

		QK_FATAL("Invalid texture data format");
		return false;
	}
}
