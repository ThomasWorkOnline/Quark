#pragma once

#include "Quark/Core/Core.h"
#include "Texture.h"

#include <unordered_map>

struct FT_FaceRec_;
typedef struct FT_FaceRec_* Face;

namespace Quark {

	struct Glyph
	{
		Vec2i Size;
		Vec2i Bearing;
		Vec2i Advance;
		uint32_t OffsetX;
	};

	class Font : public Texture
	{
	public:
		static constexpr uint8_t GlyphCount = 255;

		Font(std::string_view filepath, uint32_t fontSize);
		virtual ~Font();

		const Glyph& GetGlyph(uint8_t charcode) const;

		uint32_t GetGlyphCount() const;
		uint32_t GetFontSize() const;

		uint32_t GetAtlasWidth() const { return m_AtlasWidth; }
		uint32_t GetAtlasHeight() const { return m_AtlasHeight; }

		std::string_view GetStyleName() const;
		std::string_view GetFamilyName() const;

		static Scope<Font> Create(std::string_view filepath, uint32_t fontSize);

	protected:
		Face m_Face;
		Glyph m_Glyphs[GlyphCount]{};

		uint32_t m_AtlasWidth = 0, m_AtlasHeight = 0;
	};

	class FontLibrary
	{
	public:
		Ref<Font> Load(std::string_view name, std::string_view filepath, uint32_t fontSize);
		Ref<Font> Get(std::string_view name) const;

		void Add(std::string_view name, const Ref<Font>& font);
		bool Exists(std::string_view name) const;

		size_t Size() const { return m_Fonts.size(); }

	private:
		std::unordered_map<size_t, Ref<Font>> m_Fonts;
	};
}
