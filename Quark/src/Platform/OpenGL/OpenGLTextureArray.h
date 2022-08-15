#pragma once

#include "Quark/Renderer/TextureArray.h"

typedef unsigned int GLenum;
typedef unsigned int GLuint;

namespace Quark {

	class OpenGLTexture2DArray final : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(const Texture2DArraySpecification& spec);
		virtual ~OpenGLTexture2DArray() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual void SetData(const void* data, size_t size, uint32_t layer) override;
		virtual void GenerateMipmaps() override;

		virtual bool operator==(const Texture2DArray& other) const override
		{
			return m_RendererID == reinterpret_cast<decltype(*this)&>(other).m_RendererID;
		}

	private:
		GLuint m_RendererID = 0;
		GLenum m_InternalFormat, m_DataFormat;
	};
}
