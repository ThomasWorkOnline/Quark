#pragma once

#include <Quark.h>

struct BlockProperties
{
	Quark::SubTexture2D Faces[6];
	bool Transparent = true;
	const char* BreakSound;

	static inline BlockProperties Create(const Quark::SubTexture2D& texture, bool transparent, const char* breakSound)
	{
		BlockProperties prop = {
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			breakSound
		};

		return prop;
	}

	static inline BlockProperties Create(const Quark::SubTexture2D& top, const Quark::SubTexture2D& bottom, const Quark::SubTexture2D& sides, bool transparent, const char* breakSound)
	{
		BlockProperties prop = {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			breakSound
		};

		return prop;
	}
};
