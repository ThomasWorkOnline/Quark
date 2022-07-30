#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/ColorFormats.h"
#include "Quark/Renderer/TextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	constexpr GLenum DataFormatToOpenGLStorageFormat(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB8: 
			case ColorDataFormat::RGB10:
			case ColorDataFormat::RGB12:
			case ColorDataFormat::RGB16:
			case ColorDataFormat::RGB32:
			case ColorDataFormat::RGB8_SRGB:
				return GL_RGB;
			case ColorDataFormat::RGBA8:
			case ColorDataFormat::RGBA12:
			case ColorDataFormat::RGBA16:
			case ColorDataFormat::RGBA8_SRGB:
				return GL_RGBA;
			case ColorDataFormat::BGR8_SRGB:
			case ColorDataFormat::BGRA8_SRGB:
				return GL_BGR;
			case ColorDataFormat::Red8:
				return GL_RED;
			case ColorDataFormat::Depth24:
			case ColorDataFormat::Depth24Stencil8:
				return GL_DEPTH_COMPONENT;
			default:
				QK_CORE_FATAL("Invalid color data format");
				return GL_NONE;
		}
	}

	constexpr GLenum DataFormatToOpenGLInternalFormat(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB8:            return GL_RGB8;
			case ColorDataFormat::RGB10:           return GL_RGB10;
			case ColorDataFormat::RGB12:           return GL_RGB12;
			case ColorDataFormat::RGB16:           return GL_RGB16;
			case ColorDataFormat::RGB32:           return GL_RGB32I;
			case ColorDataFormat::RGBA8:           return GL_RGBA8;
			case ColorDataFormat::RGBA12:          return GL_RGBA12;
			case ColorDataFormat::RGBA16:          return GL_RGBA16;
			case ColorDataFormat::RGB8_SRGB:       return GL_SRGB8;
			case ColorDataFormat::RGBA8_SRGB:      return GL_SRGB8_ALPHA8;
			case ColorDataFormat::BGR8_SRGB:       return GL_SRGB8;
			case ColorDataFormat::BGRA8_SRGB:      return GL_SRGB8_ALPHA8;
			case ColorDataFormat::Red8:            return GL_R8;
			case ColorDataFormat::RGB16f:          return GL_RGB16F;
			case ColorDataFormat::RGB32f:          return GL_RGB32F;
			case ColorDataFormat::RGBA16f:         return GL_RGBA16F;
			case ColorDataFormat::RGBA32f:         return GL_RGBA32F;
			case ColorDataFormat::Depth24:         return GL_DEPTH_COMPONENT24;
			case ColorDataFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
			default:
				QK_CORE_FATAL("Invalid color data format");
				return GL_NONE;
		}
	}

	constexpr GLenum DataFormatToOpenGLDataType(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB8:
			case ColorDataFormat::RGB10:
			case ColorDataFormat::RGB12:
			case ColorDataFormat::RGB16:
			case ColorDataFormat::RGB32:
			case ColorDataFormat::RGBA8:
			case ColorDataFormat::RGBA12:
			case ColorDataFormat::RGBA16:
			case ColorDataFormat::RGB8_SRGB:
			case ColorDataFormat::RGBA8_SRGB:
			case ColorDataFormat::Red8:
			case ColorDataFormat::Depth24:
			case ColorDataFormat::Depth24Stencil8:
				return GL_UNSIGNED_BYTE;
			case ColorDataFormat::RGB16f:
			case ColorDataFormat::RGB32f:
			case ColorDataFormat::RGBA16f:
			case ColorDataFormat::RGBA32f:
				return GL_FLOAT;
			default:
				QK_CORE_FATAL("Invalid color data format");
				return GL_NONE;
		}
	}

	constexpr GLenum FilteringModeToOpenGL(TextureFilteringMode mode)
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

	constexpr GLenum TilingModeToOpenGL(TextureTilingMode mode)
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
}
