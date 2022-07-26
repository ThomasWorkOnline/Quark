#pragma once

#include "Quark/Renderer/TextureArray.h"

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

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override { return m_Spec.Height; }
		virtual uint32_t GetLayerCount() const override { return m_Spec.Layers; }

		virtual bool operator==(const Texture2DArray& other) const override
		{
			return m_RendererID == ((OpenGLTexture2DArray&)other).m_RendererID;
		}

	private:
		uint32_t m_RendererID = 0;
		Texture2DArraySpecification m_Spec;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}
