#pragma once

#include "Quark/Core/Core.h"

#include <string>
#include <unordered_map>

namespace Quark {

	struct Glyph
	{
		Vec2i Size;
		Vec2i Bearing;
		Vec2i Advance;
		uint32_t OffsetX;
	};

	class Font
	{
	public:
		virtual ~Font() = default;

		virtual void Attach(uint32_t textureSlot = 0) const = 0;
		virtual void Detach() const = 0;

		virtual const Glyph& GetGlyph(uint8_t charcode) const = 0;
		virtual uint32_t GetGlyphCount() const = 0;

		virtual uint32_t GetFontSize() const = 0;
		virtual uint32_t GetAtlasWidth() const = 0;
		virtual uint32_t GetAtlasHeight() const = 0;

		virtual std::string_view GetStyleName() const = 0;
		virtual std::string_view GetFamilyName() const = 0;

		virtual bool operator==(const Font& other) const = 0;

		static Scope<Font> Create(std::string_view filepath, uint32_t fontSize);
	};

	class FontLibrary
	{
	public:
		Ref<Font> Load(std::string_view name, std::string_view filepath, uint32_t fontSize);
		Ref<Font> Get(std::string_view name) const;

		void Add(Ref<Font> font, std::string_view name);
		bool Exists(std::string_view name) const;

		size_t Size() const { return m_Fonts.size(); }

	private:
		std::unordered_map<size_t, Ref<Font>> m_Fonts;
	};
}
