#pragma once

#include "Quark/Renderer/Texture.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace Quark {

	class OpenGLTexture2D final : public Texture2D
	{
	public:
		OpenGLTexture2D(const Texture2DSpecification& spec);
		OpenGLTexture2D(std::string_view filepath, const TextureRenderModes& renderModes);
		virtual ~OpenGLTexture2D() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual void SetData(const void* data, size_t size) override;
		virtual void GenerateMipmaps() override;

		virtual bool operator==(const Texture& other) const override;

		// Non-Copyable
		OpenGLTexture2D(const OpenGLTexture2D&) = delete;
		OpenGLTexture2D& operator=(const OpenGLTexture2D&) = delete;

	private:
		GLuint m_RendererID = 0;
		GLenum m_InternalFormat = 0, m_DataFormat = 0;
	};
}
