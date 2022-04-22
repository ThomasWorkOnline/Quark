#include "qkpch.h"
#include "TextRendererTraits.h"

namespace Quark {

	int32_t TextRenderTraits::GetOriginX() const
	{
		switch (HAlign)
		{
			case Quark::HorizontalTextAlignment::Left:   return -Width;
			case Quark::HorizontalTextAlignment::Right:  return 0;
			case Quark::HorizontalTextAlignment::Center: return -Width / 2.0f;
			default:
				QK_CORE_ASSERT(false, "No alignment mode was specified.");
				return 0.0f;
		}
	}

	int32_t TextRenderTraits::GetOriginY() const
	{
		switch (VAlign)
		{
			case Quark::VerticalTextAlignment::Bottom: return 0.0f;
			case Quark::VerticalTextAlignment::Top:    return Height;
			case Quark::VerticalTextAlignment::Center: return Height / 2.0f;
			default:
				QK_CORE_ASSERT(false, "No alignment mode was specified.");
				return 0.0f;
		}
	}

	void TextRenderTraits::CalculateLabelDimensions(std::string_view text)
	{
		int32_t width = 0, height = 0;
		for (auto it = text.begin(); it != text.end(); it++)
		{
			auto& g = FontStyle->GetGlyph(*it);
			width += g.Advance.x >> 6;
			height = std::max(height, g.Size.y);
		}

		Width = width;
		Height = height;
	}
}
