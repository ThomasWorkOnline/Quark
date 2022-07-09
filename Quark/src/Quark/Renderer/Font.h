#pragma once

#include "Quark/Core/Core.h"

#include <string>
#include <unordered_map>

namespace Quark {

	struct Glyph
	{
		glm::ivec2 Size;
		glm::ivec2 Bearing;
		glm::ivec2 Advance;
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

		virtual bool operator==(const Font& other) const = 0;

		static Ref<Font> Create(std::string_view filepath, uint32_t fontSize);
	};

	class FontLibrary
	{
	public:
		Ref<Font> Load(const std::string& name, std::string_view filepath, uint32_t fontSize);
		void Add(const Ref<Font>& font, const std::string& name);

		const Ref<Font>& Get(const std::string& name);

		bool Exists(const std::string& name) const;
		size_t Size() const { return m_Fonts.size(); }

	private:
		std::unordered_map<std::string, Ref<Font>> m_Fonts;
	};
}
