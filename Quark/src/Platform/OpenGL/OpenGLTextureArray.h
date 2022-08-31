#pragma once

#include "Quark/Renderer/Texture.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace Quark {

	class OpenGLTexture2DArray final : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(const Texture2DArraySpecification& spec);
		virtual ~OpenGLTexture2DArray() final override;

		virtual void Attach(uint32_t textureSlot = 0) const final override;
		virtual void Detach() const final override;

		virtual void SetData(const void* data, size_t size, uint32_t layer) final override;
		virtual void GenerateMipmaps() final override;

		virtual bool operator==(const Texture& other) const final override;

		// Non-Copyable
		OpenGLTexture2DArray(const OpenGLTexture2DArray&) = delete;
		OpenGLTexture2DArray& operator=(const OpenGLTexture2DArray&) = delete;

	private:
		GLuint m_RendererID = 0;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
