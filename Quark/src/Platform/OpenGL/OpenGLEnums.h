#pragma once

#include "Quark/Renderer/Buffer.h"
#include "Quark/Renderer/Formats.h"
#include "Quark/Renderer/RenderModes.h"
#include "Quark/Renderer/Shader.h"

#include <glad/glad.h>

namespace Quark {

	inline constexpr GLenum s_OpenGLCullModeLUT[] = {
		/*None*/            GL_NONE,
		/*Front*/           GL_FRONT,
		/*Back*/            GL_BACK,
		/*FrontAndBack*/    GL_FRONT_AND_BACK
	};

	inline constexpr GLenum s_OpenGLDataFormatLUT[] = {
		/*None*/            GL_NONE,

		/*RGB8*/            GL_UNSIGNED_BYTE,
		/*RGB16*/           GL_UNSIGNED_BYTE,
		/*RGB32*/           GL_UNSIGNED_INT,

		/*RGBA8*/           GL_UNSIGNED_BYTE,
		/*RGBA16*/          GL_UNSIGNED_BYTE,

		/*RGB8_SRGB*/       GL_UNSIGNED_BYTE,
		/*RGBA8_SRGB*/      GL_UNSIGNED_BYTE,

		/*BGR8_SRGB*/       GL_UNSIGNED_BYTE,
		/*BGRA8_SRGB*/      GL_UNSIGNED_BYTE,

		/*RGB16f*/          GL_FLOAT,
		/*RGB32f*/          GL_FLOAT,
		/*RGBA16f*/         GL_FLOAT,
		/*RGBA32f*/         GL_FLOAT,

		/*Red8*/            GL_UNSIGNED_BYTE,

		/*Depth24*/         GL_UNSIGNED_BYTE,
		/*Depth24Stencil8*/ GL_UNSIGNED_BYTE
	};

	inline constexpr GLenum s_OpenGLStorageFormatLUT[] = {
		/*None*/            GL_NONE,

		/*RGB8*/            GL_RGB,
		/*RGB16*/           GL_RGB,
		/*RGB32*/           GL_RGB_INTEGER,

		/*RGBA8*/           GL_RGBA,
		/*RGBA16*/          GL_RGBA,

		/*RGB8_SRGB*/       GL_RGB,
		/*RGBA8_SRGB*/      GL_RGBA,

		/*BGR8_SRGB*/       GL_BGR,
		/*BGRA8_SRGB*/      GL_BGRA,

		/*RGB16f*/          GL_RGB,
		/*RGB32f*/          GL_RGB,
		/*RGBA16f*/         GL_RGBA,
		/*RGBA32f*/         GL_RGBA,

		/*Red8*/            GL_RED,

		/*Depth24*/         GL_DEPTH_COMPONENT,
		/*Depth24Stencil8*/ GL_DEPTH_STENCIL
	};

	inline constexpr GLenum s_OpenGLInternalFormatLUT[] = {
		/*None*/            GL_NONE,

		/*RGB8*/            GL_RGB8,
		/*RGB16*/           GL_RGB16,
		/*RGB32*/           GL_RGB32UI,

		/*RGBA8*/           GL_RGBA8,
		/*RGBA16*/          GL_RGBA16,

		/*RGB8_SRGB*/       GL_SRGB8,
		/*RGBA8_SRGB*/      GL_SRGB8_ALPHA8,

		/*BGR8_SRGB*/       GL_SRGB8,
		/*BGRA8_SRGB*/      GL_SRGB8_ALPHA8,

		/*RGB16f*/          GL_RGB16F,
		/*RGB32f*/          GL_RGB32F,
		/*RGBA16f*/         GL_RGBA16F,
		/*RGBA32f*/         GL_RGBA32F,

		/*Red8*/            GL_R8,

		/*Depth24*/         GL_DEPTH_COMPONENT24,
		/*Depth24Stencil8*/ GL_DEPTH24_STENCIL8
	};

	inline constexpr GLenum s_DepthFunctionLUT[] = {
		/*Never*/                GL_NEVER,
		/*Always*/               GL_ALWAYS,
		/*NotEqual*/             GL_NOTEQUAL,
		/*Less*/                 GL_LESS,
		/*LessEqual*/            GL_LEQUAL,
		/*Greater*/              GL_GREATER,
		/*GreaterEqual*/         GL_GEQUAL
	};

	inline constexpr GLenum s_OpenGLShaderStageLUT[] = {
		/*None*/                 GL_NONE,
		/*Vertex*/               GL_VERTEX_SHADER,
		/*Geometry*/             GL_GEOMETRY_SHADER,
		/*Fragment*/             GL_FRAGMENT_SHADER,
		/*Compute*/              GL_COMPUTE_SHADER
	};

	inline constexpr GLenum s_OpenGLFilteringModeLUT[] = {
		/*None*/                 GL_NONE,
		/*Nearest*/              GL_NEAREST,
		/*Linear*/               GL_LINEAR,
		/*NearestMipmapNearest*/ GL_NEAREST_MIPMAP_NEAREST,
		/*NearestMipmapLinear*/  GL_NEAREST_MIPMAP_LINEAR,	
		/*LinearMipmapNearest*/  GL_LINEAR_MIPMAP_NEAREST,
		/*LinearMipmapLinear*/   GL_LINEAR_MIPMAP_LINEAR
	};

	inline constexpr GLenum s_OpenGLTilingModeLUT[] = {
		/*None*/                 GL_NONE,
		/*ClampToBorder*/        GL_CLAMP_TO_BORDER,
		/*ClampToEdge*/          GL_CLAMP_TO_EDGE,
		/*Repeat*/               GL_REPEAT
	};

	constexpr GLenum CullModeToOpenGL(RenderCullMode mode)
	{
		return s_OpenGLCullModeLUT[static_cast<size_t>(mode)];
	}

	constexpr GLenum DataFormatToOpenGLDataType(ColorDataFormat format)
	{
		return s_OpenGLDataFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DataFormatToOpenGLStorageFormat(ColorDataFormat format)
	{
		return s_OpenGLStorageFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DataFormatToOpenGLInternalFormat(ColorDataFormat format)
	{
		return s_OpenGLInternalFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DepthFunctionToOpenGL(RenderDepthFunction func)
	{
		return s_DepthFunctionLUT[static_cast<size_t>(func)];
	}

	constexpr GLenum ShaderStageToOpenGL(ShaderStage stage)
	{
		return s_OpenGLShaderStageLUT[static_cast<size_t>(stage)];
	}

	constexpr GLenum FilteringModeToOpenGL(TextureFilteringMode mode)
	{
		return s_OpenGLFilteringModeLUT[static_cast<size_t>(mode)];
	}

	constexpr GLenum TilingModeToOpenGL(TextureTilingMode mode)
	{
		return s_OpenGLTilingModeLUT[static_cast<size_t>(mode)];
	}
}
