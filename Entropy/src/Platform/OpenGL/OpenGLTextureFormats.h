#pragma once

#include "../../Entropy/Core/Core.h"
#include "../../Entropy/Renderer/TextureFormats.h"

#include <GL/glew.h>

namespace Entropy {

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

		NT_FATAL("Invalid internal texture format");
	}

	static GLenum GetTextureDataFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::RGB8:	return GL_RGB;
		case TextureDataFormat::RGBA8:	return GL_RGBA;
		}

		NT_FATAL("Invalid texture color format");
	}

	static GLenum GetTextureFilteringFormat(TextureFilteringFormat format)
	{
		switch (format)
		{
		case TextureFilteringFormat::Nearest:				return GL_NEAREST;
		case TextureFilteringFormat::Linear:				return GL_LINEAR;
		case TextureFilteringFormat::NearestMipmapNearest:	return GL_NEAREST_MIPMAP_NEAREST;
		case TextureFilteringFormat::NearestMipmapLinear:	return GL_NEAREST_MIPMAP_LINEAR;
		case TextureFilteringFormat::LinearMipmapNearest:	return GL_LINEAR_MIPMAP_NEAREST;
		case TextureFilteringFormat::LinearMipmapLinear:	return GL_LINEAR_MIPMAP_LINEAR;
		}

		NT_FATAL("Invalid texture filtering format");
	}

	static GLenum GetTextureTilingFormat(TextureTilingFormat format)
	{
		switch (format)
		{
		case TextureTilingFormat::ChampToBorder: return GL_CLAMP_TO_BORDER;
		case TextureTilingFormat::ClampToEdge:   return GL_CLAMP_TO_EDGE;
		case TextureTilingFormat::Repeat:        return GL_REPEAT;
		}

		NT_FATAL("Invalid texture tiling format");
	}

	static bool IsTextureDepthFormat(TextureDataFormat format)
	{
		switch (format)
		{
		case TextureDataFormat::Depth24Stencil8: return true;
		}
		return false;
	}
}
