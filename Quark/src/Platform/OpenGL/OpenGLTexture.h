#pragma once

#include "Quark/Renderer/Texture.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace Quark {

	class OpenGLTexture
	{
	public:
		GLuint GetRendererID() const { return m_RendererID; }
		GLenum GetTarget() const { return m_Target; }

	protected:
		GLuint m_RendererID = 0;
		GLenum m_Target = 0;
	};

	class OpenGLTexture2D final : public Texture2D, public OpenGLTexture
	{
	public:
		OpenGLTexture2D(const Texture2DSpecification& spec);
		OpenGLTexture2D(std::string_view filepath);
		virtual ~OpenGLTexture2D() final override;

		virtual void SetData(const void* data, size_t size) final override;
		virtual void GenerateMipmaps() final override;

		virtual const void* GetHandle() const final override { return static_cast<const OpenGLTexture*>(this); }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLTexture2D(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D& operator=(const OpenGLTexture2D&) = delete;

	private:
		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};

	class OpenGLTexture2DArray final : public Texture2DArray, public OpenGLTexture
	{
	public:
		OpenGLTexture2DArray(const Texture2DArraySpecification& spec);
		virtual ~OpenGLTexture2DArray() final override;

		virtual void SetData(const void* data, size_t size, uint32_t layer) final override;
		virtual void GenerateMipmaps() final override;

		virtual const void* GetHandle() const final override { return static_cast<const OpenGLTexture*>(this); }

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLTexture2DArray(const OpenGLTexture2DArray&) = delete;
		OpenGLTexture2DArray& operator=(const OpenGLTexture2DArray&) = delete;

	private:
		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};
}
