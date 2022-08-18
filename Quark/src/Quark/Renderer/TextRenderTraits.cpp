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

			QK_ASSERT_NO_DEFAULT("No alignment mode was specified");
		}

		return 0;
	}

	int32_t TextRenderTraits::GetOriginY() const
	{
		switch (VAlign)
		{
			case VerticalTextAlignment::Bottom: return 0;
			case VerticalTextAlignment::Top:    return Height;
			case VerticalTextAlignment::Center: return Height / 2;

			QK_ASSERT_NO_DEFAULT("No alignment mode was specified");
		}

		return 0;
	}
}
