#pragma once

#include "Quark/Renderer/TextureArray.h"

namespace Quark {

	class OpenGLTexture2DArray : public Texture2DArray
	{
	public:
		OpenGLTexture2DArray(const TextureArraySpecification& spec);
		~OpenGLTexture2DArray() override;

		uint32_t GetWidth() const override { return m_Spec.Width; }
		uint32_t GetHeight() const override{ return m_Spec.Height; }
		uint32_t GetLayerCount() const override { return m_Spec.Layers; }

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const TextureArray& other) const override
		{
			return m_RendererID == ((OpenGLTexture2DArray&)other).m_RendererID;
		}

		void SetData(const void* data, size_t size, uint32_t layer) override;
		void GenerateMipmaps() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

	private:
		uint32_t m_RendererID = 0;
		TextureArraySpecification m_Spec;
		uint32_t m_InternalFormat, m_DataFormat;
	};
}
