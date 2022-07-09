#pragma once

#include "Quark/Core/Core.h"
#include "Quark/Renderer/ColorFormats.h"
#include "Quark/Renderer/TextureFormats.h"

#include <glad/glad.h>

namespace Quark {

	constexpr GLenum DataFormatToOpenGL(ColorDataFormat format)
	{
		switch (format)
		{
			case ColorDataFormat::RGB:   return GL_RGB;
			case ColorDataFormat::RGBA:  return GL_RGBA;
			case ColorDataFormat::BGR:   return GL_BGR;
			case ColorDataFormat::BGRA:  return GL_BGRA;
			case ColorDataFormat::Red:   return GL_RED;
			case ColorDataFormat::Depth: return GL_DEPTH_COMPONENT;
			default:
				QK_CORE_FATAL("Invalid color data format");
				return GL_NONE;
		}
	}

	constexpr GLenum InternalFormatToOpenGL(InternalColorFormat format)
	{
		switch (format)
		{
			case InternalColorFormat::RGB8:            return GL_RGB8;
			case InternalColorFormat::RGB10:           return GL_RGB10;
			case InternalColorFormat::RGB12:           return GL_RGB12;
			case InternalColorFormat::RGB16:           return GL_RGB16;
			case InternalColorFormat::RGB32:           return GL_RGB32I;
			case InternalColorFormat::RGBA8:           return GL_RGBA8;
			case InternalColorFormat::RGBA12:          return GL_RGBA12;
			case InternalColorFormat::RGBA16:          return GL_RGBA16;
			case InternalColorFormat::SRGB8:           return GL_SRGB8;
			case InternalColorFormat::SRGBA8:          return GL_SRGB8_ALPHA8;
			case InternalColorFormat::Red8:            return GL_R8;
			case InternalColorFormat::RGB16f:          return GL_RGB16F;
			case InternalColorFormat::RGB32f:          return GL_RGB32F;
			case InternalColorFormat::RGBA16f:         return GL_RGBA16F;
			case InternalColorFormat::RGBA32f:         return GL_RGBA32F;
			case InternalColorFormat::Depth24:         return GL_DEPTH_COMPONENT24;
			case InternalColorFormat::Depth24Stencil8: return GL_DEPTH24_STENCIL8;
			default:
				QK_CORE_FATAL("Invalid internal color format");
				return GL_NONE;
		}
	}

	constexpr GLenum InternalFormatToOpenGLDataType(InternalColorFormat format)
	{
		switch (format)
		{
			case InternalColorFormat::RGB8:
			case InternalColorFormat::RGB10:
			case InternalColorFormat::RGB12:
			case InternalColorFormat::RGB16:
			case InternalColorFormat::RGB32:
			case InternalColorFormat::RGBA8:
			case InternalColorFormat::RGBA12:
			case InternalColorFormat::RGBA16:
			case InternalColorFormat::SRGB8:
			case InternalColorFormat::SRGBA8:
			case InternalColorFormat::Red8:
			case InternalColorFormat::Depth24:
			case InternalColorFormat::Depth24Stencil8:
				return GL_UNSIGNED_BYTE;
			case InternalColorFormat::RGB16f:
			case InternalColorFormat::RGB32f:
			case InternalColorFormat::RGBA16f:
			case InternalColorFormat::RGBA32f:
				return GL_FLOAT;
			default:
				QK_CORE_FATAL("Invalid internal color format");
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
