#pragma once

#include "Quark/Renderer/Font.h"

#include <unordered_map>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

namespace Quark {

	class OpenGLFont : public Font
	{
	public:
		OpenGLFont(std::string_view filepath, uint32_t width, uint32_t height);
		~OpenGLFont() override;

		void Attach(uint32_t textureSlot = 0) const override;
		void Detach() const override;

		const Character& GetCharacter(uint32_t charcode) const override { return m_Characters.at(charcode); }
		uint32_t GetCharacterCount() const override { return m_Characters.size(); }

		uint32_t GetPixelWidth() const override { return m_Width; }
		uint32_t GetPixelHeight() const override { return m_Height; }

		uint32_t GetAtlasWidth() const override { return m_AtlasWidth; }
		uint32_t GetAtlasHeight() const override { return m_AtlasHeight; }

		uint32_t GetRendererID() const override { return m_RendererID; }

		bool operator==(const Font& other) const override
		{
			return m_RendererID == ((OpenGLFont&)other).m_RendererID;
		}

	private:
		void Init();
		void Shutdown();

	private:
		Face m_Face;
		std::unordered_map<uint32_t, Character> m_Characters;
		uint32_t m_Width, m_Height;
		uint32_t m_AtlasWidth, m_AtlasHeight;
		uint32_t m_RendererID = 0;
	};
}
