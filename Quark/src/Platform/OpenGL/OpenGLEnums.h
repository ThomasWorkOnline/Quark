#pragma once

#include "Quark/Renderer/Buffer.h"
#include "Quark/Renderer/Formats.h"
#include "Quark/Renderer/Pipeline.h"
#include "Quark/Renderer/RenderModes.h"
#include "Quark/Renderer/Shader.h"

#include <glad/glad.h>

namespace Quark {

	inline constexpr GLenum s_OpenGLBaseShaderDataTypeLUT[] = {
		/*None*/            GL_NONE,

		/*Float*/           GL_FLOAT,
		/*Float2*/          GL_FLOAT,
		/*Float3*/          GL_FLOAT,
		/*Float4*/          GL_FLOAT,

		/*Double*/          GL_DOUBLE,
		/*Double2*/         GL_DOUBLE,
		/*Double3*/         GL_DOUBLE,
		/*Double4*/         GL_DOUBLE,

		/*Mat3f*/           GL_FLOAT,
		/*Mat4f*/           GL_FLOAT,
		/*Mat3d*/           GL_DOUBLE,
		/*Mat4d*/           GL_DOUBLE,

		/*Int*/             GL_INT,
		/*Int2*/            GL_INT,
		/*Int3*/            GL_INT,
		/*Int4*/            GL_INT,
		/*Bool*/            GL_BOOL
	};

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
		/*RGB32UInt*/       GL_UNSIGNED_INT,

		/*RGBA8*/           GL_UNSIGNED_BYTE,
		/*RGBA16*/          GL_UNSIGNED_BYTE,

		/*RGB8SRGB*/        GL_UNSIGNED_BYTE,
		/*RGBA8SRGB*/       GL_UNSIGNED_BYTE,

		/*BGR8SRGB*/        GL_UNSIGNED_BYTE,
		/*BGRA8SRGB*/       GL_UNSIGNED_BYTE,

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
		/*RGB32UInt*/       GL_RGB_INTEGER,

		/*RGBA8*/           GL_RGBA,
		/*RGBA16*/          GL_RGBA,

		/*RGB8SRGB*/        GL_RGB,
		/*RGBA8SRGB*/       GL_RGBA,

		/*BGR8SRGB*/        GL_BGR,
		/*BGRA8SRGB*/       GL_BGRA,

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
		/*RGB32UInt*/       GL_RGB32UI,

		/*RGBA8*/           GL_RGBA8,
		/*RGBA16*/          GL_RGBA16,

		/*RGB8SRGB*/        GL_SRGB8,
		/*RGBA8SRGB*/       GL_SRGB8_ALPHA8,

		/*BGR8SRGB*/        GL_SRGB8,
		/*BGRA8SRGB*/       GL_SRGB8_ALPHA8,

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
		/*LessOrEqual*/          GL_LEQUAL,
		/*Greater*/              GL_GREATER,
		/*GreaterOrEqual*/       GL_GEQUAL
	};

	inline constexpr GLenum s_OpenGLPrimitiveTopologyLUT[] = {
		/*None*/                 GL_NONE,
		/*PointList*/            GL_POINTS,
		/*LineList*/             GL_LINES,
		/*LineStrip*/            GL_LINE_STRIP,
		/*TriangleList*/         GL_TRIANGLES,
		/*TriangleStrip*/        GL_TRIANGLE_STRIP,
		/*TriangleFan*/          GL_TRIANGLE_FAN
	};

	inline constexpr GLenum s_OpenGLShaderStageLUT[] = {
		/*None*/                 GL_NONE,
		/*Vertex*/               GL_VERTEX_SHADER,
		/*Geometry*/             GL_GEOMETRY_SHADER,
		/*Fragment*/             GL_FRAGMENT_SHADER,
		/*Compute*/              GL_COMPUTE_SHADER
	};

	inline constexpr GLenum s_OpenGLSamplerFilterModeLUT[] = {
		/*None*/                 GL_NONE,
		/*Nearest*/              GL_NEAREST,
		/*Linear*/               GL_LINEAR,
		/*NearestMipmapNearest*/ GL_NEAREST_MIPMAP_NEAREST,
		/*NearestMipmapLinear*/  GL_NEAREST_MIPMAP_LINEAR,	
		/*LinearMipmapNearest*/  GL_LINEAR_MIPMAP_NEAREST,
		/*LinearMipmapLinear*/   GL_LINEAR_MIPMAP_LINEAR
	};

	inline constexpr GLenum s_OpenGLSamplerAddressModeLUT[] = {
		/*None*/                 GL_NONE,
		/*ClampToBorder*/        GL_CLAMP_TO_BORDER,
		/*ClampToEdge*/          GL_CLAMP_TO_EDGE,
		/*Repeat*/               GL_REPEAT
	};

	constexpr GLenum CullModeToOpenGL(RenderCullMode mode)
	{
		return s_OpenGLCullModeLUT[static_cast<size_t>(mode)];
	}

	constexpr GLenum DataFormatToOpenGLDataType(ColorFormat format)
	{
		return s_OpenGLDataFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DataFormatToOpenGLStorageFormat(ColorFormat format)
	{
		return s_OpenGLStorageFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DataFormatToOpenGLInternalFormat(ColorFormat format)
	{
		return s_OpenGLInternalFormatLUT[static_cast<size_t>(format)];
	}

	constexpr GLenum DepthCompareFunctionToOpenGL(DepthCompareFunction func)
	{
		return s_DepthFunctionLUT[static_cast<size_t>(func)];
	}

	constexpr GLenum PrimitiveTopologyToOpenGL(PrimitiveTopology topology)
	{
		return s_OpenGLPrimitiveTopologyLUT[static_cast<size_t>(topology)];
	}

	constexpr GLenum ShaderStageToOpenGL(ShaderStage stage)
	{
		return s_OpenGLShaderStageLUT[static_cast<size_t>(stage)];
	}

	constexpr GLenum SamplerFilterModeToOpenGL(SamplerFilterMode mode)
	{
		return s_OpenGLSamplerFilterModeLUT[static_cast<size_t>(mode)];
	}

	constexpr GLenum SamplerAddressModeToOpenGL(SamplerAddressMode mode)
	{
		return s_OpenGLSamplerAddressModeLUT[static_cast<size_t>(mode)];
	}

	constexpr GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		return s_OpenGLBaseShaderDataTypeLUT[static_cast<size_t>(type)];
	}
}
