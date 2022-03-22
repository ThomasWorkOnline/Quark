#pragma once

#include "../../Quark/Renderer/TextureArray.h"

namespace Quark {

	class OpenGLTexture2DArray : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(const TextureArraySpecification& spec);
		virtual ~OpenGLTexture2DArray() override;

		virtual uint32_t GetWidth() const override { return m_Spec.Width; }
		virtual uint32_t GetHeight() const override{ return m_Spec.Height; }
		virtual uint32_t GetLayerCount() const override { return m_Spec.Layers; }

		virtual uint32_t GetRendererID() const override { return m_RendererID; }

		virtual bool operator==(const TextureArray& other) const override
		{
			return m_RendererID == ((OpenGLTexture2DArray&)other).m_RendererID;
		}

		virtual void SetData(void* data, size_t size, uint32_t layer) override;
		virtual void GenerateMipmaps() override;

		virtual void Attach(uint32_t textureSlot = 0) const override;
		virtual void Detach() const override;

	private:
		uint32_t m_RendererID = 0;
		TextureArraySpecification m_Spec;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}
