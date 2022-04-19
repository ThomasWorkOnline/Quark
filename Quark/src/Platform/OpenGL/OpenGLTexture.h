#pragma once

#include "Quark/Renderer/Texture.h"

namespace Quark {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& spec);
		OpenGLTexture2D(std::string_view filepath, const TextureDescriptor& descriptor);
		~OpenGLTexture2D() override;

		uint32_t GetWidth() const override { return m_Spec.Width; }
		uint32_t GetHeight() const override { return m_Spec.Height; }

		void SetData(const void* data, size_t size) override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		TextureSpecification m_Spec;
		uint32_t m_InternalFormat = 0, m_DataFormat = 0, m_DataType = 0;
	};
}
