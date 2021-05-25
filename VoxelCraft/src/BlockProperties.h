#pragma once

#include <Quark.h>

struct BlockProperties
{
	Quark::SubTexture2D Faces[6];
	bool Transparent = true;
	const char* BreakSound;
};

class BlockPropertiesFactory
{
public:
	static BlockProperties Create(Quark::SubTexture2D texture, bool transparent, const char* breakSound)
	{
		BlockProperties prop = {
			{ texture, texture, texture, texture, texture, texture },
			transparent,
			breakSound
		};

		return prop;
	}

	static BlockProperties Create(Quark::SubTexture2D top, Quark::SubTexture2D bottom, Quark::SubTexture2D sides, bool transparent, const char* breakSound)
	{
		BlockProperties prop = {
			{ sides, sides, sides, sides, top, bottom },
			transparent,
			breakSound
		};

		return prop;
	}
};
