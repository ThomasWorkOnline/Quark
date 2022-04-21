#pragma once

#include "Quark/Core/Core.h"
#include "Font.h"

namespace Quark {

	enum class HorizontalTextAlignment
	{
		Left = 0,
		Right,
		Center,

		Default = Left
	};

	enum class VerticalTextAlignment
	{
		Bottom = 0,
		Top,
		Center,

		Default = Bottom
	};

	struct TextRenderTraits
	{
		Ref<Font> FontStyle;
		glm::vec4 Color = glm::vec4(1.0f);

		int32_t Width = 0, Height = 0;
		HorizontalTextAlignment HAlign{};
		VerticalTextAlignment	VAlign{};

		std::pair<int32_t, int32_t> GetOrigin() const { return { GetOriginX(), GetOriginY() }; }

		int32_t GetOriginX() const;
		int32_t GetOriginY() const;

		void CalculateLabelDimensions(std::string_view text);
	};
}
