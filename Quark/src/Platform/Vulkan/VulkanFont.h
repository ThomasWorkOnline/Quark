#pragma once

#include "Quark/Renderer/Font.h"

namespace Quark {

	class VulkanFont final : public Font
	{
	public:
		VulkanFont(std::string_view filepath, uint32_t fontSize) {}
		virtual ~VulkanFont() override = default;

		virtual void Attach(uint32_t textureSlot = 0) const override {}
		virtual void Detach() const override {}

		virtual const Glyph& GetGlyph(uint8_t charcode) const override { return m_NullGlyph; }
		virtual uint32_t GetGlyphCount() const override { return 0; }

		virtual uint32_t GetFontSize() const override { return 0; }
		virtual uint32_t GetAtlasWidth() const override { return 0; }
		virtual uint32_t GetAtlasHeight() const override { return 0; }

		virtual uint32_t GetRendererID() const override { return 0; }

		virtual bool operator==(const Font& other) const override
		{
			return false;
		}

	private:
		Glyph m_NullGlyph{};
	};
}
