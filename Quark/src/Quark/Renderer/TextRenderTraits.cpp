#include "qkpch.h"
#include "TextRendererTraits.h"

namespace Quark {

	int32_t TextRenderTraits::GetOriginX() const
	{
		switch (HAlign)
		{
			case HorizontalTextAlignment::Left:   return -Width;
			case HorizontalTextAlignment::Right:  return 0;
			case HorizontalTextAlignment::Center: return -Width / 2;

			QK_ASSERT_DEFAULT("No alignment mode was specified", 0);
		}
	}

	int32_t TextRenderTraits::GetOriginY() const
	{
		switch (VAlign)
		{
			case VerticalTextAlignment::Bottom: return 0;
			case VerticalTextAlignment::Top:    return Height;
			case VerticalTextAlignment::Center: return Height / 2;

			QK_ASSERT_DEFAULT("No alignment mode was specified", 0);
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
