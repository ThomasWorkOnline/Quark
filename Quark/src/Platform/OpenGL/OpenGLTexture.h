#pragma once

#include "Quark/Renderer/Texture.h"

namespace Quark {

	class OpenGLTexture2D final : public Texture2D
	{
	public:
		OpenGLTexture2D(const Texture2DSpecification& spec);
		OpenGLTexture2D(std::string_view filepath, const TextureFormatDescriptor& descriptor);
		virtual ~OpenGLTexture2D() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual void SetData(const void* data, size_t size) override;
		virtual void GenerateMipmaps() override;

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Texture2D& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		Texture2DSpecification m_Spec;
		uint32_t m_InternalFormat = 0, m_DataFormat = 0, m_DataType = 0;
	};
}
