#pragma once

#include "../../Quark/Renderer/Texture.h"

namespace Quark {

	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const TextureSpecification& spec);
		OpenGLTexture2D(const std::string& filepath, const TextureRenderModes& modes);
		virtual ~OpenGLTexture2D();

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }

		virtual void SetData(void* data, uint32_t size) override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const Texture& other) const override
		{
			return m_RendererID == ((OpenGLTexture2D&)other).m_RendererID;
		}

	private:
		uint32_t GetTarget() const;

	private:
		uint32_t m_RendererID = 0;
		TextureSpecification m_Spec;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}
